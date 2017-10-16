#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include "json/json.h"
#include <http_core.h>
#include <http_protocol.h>
#include <http_request.h>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>

#include "Base64.h"
#include "Cryptography/Cryptor.h"
#include "CurlWrapper.h"
#include "CustomRequestAttributes.h"
#include "Exceptions.h"
#include "File.h"
#include "FileInfo.h"
#include "GatewayRequest.h"
#include "GatewayResponse.h"
#include "IdfcFilter.h"
#include "Logger.h"
#include "RandomSequence.h"
#include "RequestHeadersWrapper.h"

namespace idfc
{    
    static const std::string kFccFileStore("fcc_file_login");
    static const std::string kFccFileTimeStampStore("fcc_file_login_timestamp");   

    const char* IdfcFilter::kHandlerName = "idfc";
    const std::string IdfcFilter::kGatewayTokenKey("gatewayToken");
    
    IdfcFilter::IdfcFilter() : isEnabled(false), fqdn(NetworkInfo().getFQDN())
    {
    }

    void IdfcFilter::enable()
    {
        isEnabled = true;
    }
    
    void IdfcFilter::disable()
    {
        isEnabled = false;
    }
    
    void IdfcFilter::setAccountName(const std::string& value)
    {
        accountName = value;
    }
    
    void IdfcFilter::setGatewayUrl(const std::string& value)
    {
        gatewayUrl = value;
    }

    void IdfcFilter::setGatewayToken(const std::string& value)
    {
        gatewayToken = value;
    }
    
    void IdfcFilter::setPluginId(const std::string& value)
    {
        pluginIdKey = value;
    }
    
    void IdfcFilter::setSecretKey(const std::string& value)
    {
        secretKey = value;
    }
    
    void IdfcFilter::setCACertPath(const std::string& value)
    {
        caCertPath = value;
    }
    
    void IdfcFilter::setIgnoredExtensions(const std::string& value)
    {
        ignorePattern.setIgnoredExtensions(value);
    }
    
    void IdfcFilter::setIgnoredHosts(const std::string& value)
    {
        ignorePattern.setIgnoredHosts(value);
    }
    
    void IdfcFilter::setIgnoredUrls(const std::string& value)
    {
        ignorePattern.setIgnoredUrls(value);
    }

    void IdfcFilter::setExtendedDumpFile(const std::string& value)
    {
        extentededDumpFileName = value;
    }
        
    int IdfcFilter::process(request_rec* incomingRequest)
    {
        if (!isEnabled)
        {
            Logger::debug(incomingRequest, "Module is disabled");
            return (DECLINED);
        }

        if (ignorePattern.isRequestShouldBeRejected(incomingRequest))
        {
            Logger::debug(incomingRequest, "Request rejected due to filter settings");
            return (DECLINED);
        }
        
        CustomRequestAttributes customAttributes(incomingRequest);
        customAttributes.set(CustomRequestAttributes::kAccountNameKey, accountName);
        customAttributes.set(CustomRequestAttributes::kGatewayTokenKey, gatewayToken);
        customAttributes.set(CustomRequestAttributes::kPluginIdKey, pluginIdKey);

        GatewayRequest gatewayRequest(incomingRequest, fqdn);
        appendToExtendedDump(std::string("GatewayRequest: ") + gatewayRequest.getPayload().toStyledString());
        gatewayRequest.setCACertPath(caCertPath);
        
        auto unparsedGatewayResponse = gatewayRequest.sendTo(gatewayUrl);
        appendToExtendedDump(std::string("GatewayResponse: ") + unparsedGatewayResponse);
        
        GatewayResponse gatewayResponse(unparsedGatewayResponse);
        auto responseHeaders = gatewayResponse.getSubresponseHeaders();
        auto gatewayTokenIterator = responseHeaders.find(kGatewayTokenKey);
        if (gatewayTokenIterator != responseHeaders.cend())
            gatewayToken = gatewayTokenIterator->second;
        
        gatewayResponse.getSubresponseCookies();
        
        auto httpStatus = gatewayResponse.getSubresponseStatus();
        if (httpStatus == HTTP_CONTINUE)
        {
            Logger::debug(incomingRequest, "Response from gateway is HTTP_CONTINUE");

            RequestHeadersWrapper requestHeadersWrapper(incomingRequest);
            requestHeadersWrapper.copy(responseHeaders, RequestHeadersWrapper::TargetHeaders::In);
            
            auto additionalCookies = gatewayResponse.getSubresponseCookies();
            requestHeadersWrapper.copyCookies(additionalCookies, RequestHeadersWrapper::TargetHeaders::Out);
            
            return (DECLINED);
        }
        else if (httpStatus == HTTP_NOT_EXTENDED)
        {
            Logger::debug(incomingRequest, "Response from gateway is HTTP_NOT_EXTENDED");

            auto responseBody = gatewayResponse.getSubresponseBody();
            if (responseBody.find("Signature Needed") != std::string::npos)
            {
                auto randomText = RandomSequence::generate(26);
                Cryptor::HMACSHA1Digest digest;
                Cryptor::computeHMACSHA1(randomText, secretKey, digest);
                auto signedRandomText = base64_encode(digest.data(), static_cast<unsigned int>(digest.size()));
                auto encodedSignedRandomText = CurlWrapper().getUrlEncodedVersionOf(signedRandomText);
                customAttributes.set(CustomRequestAttributes::kPluginIdKey, pluginIdKey);
                customAttributes.set(CustomRequestAttributes::kRandomTextKey, randomText);
                customAttributes.set(CustomRequestAttributes::kRandomTextSignedKey, encodedSignedRandomText);    
            }
            else
            {
                std::string fileName(incomingRequest->filename);
                auto modificationTime = FileInfo(fileName).getModificationTime();
                
                auto isCacheActual = false;
                auto cachedPackedFile = fccCache.find(kFccFileStore);
                auto cachedPackedTimestamp = fccCache.find(kFccFileTimeStampStore);
                int cachedTimestamp;
                if (cachedPackedTimestamp && cachedPackedFile)
                {
                    cachedTimestamp = *reinterpret_cast<const int*>(cachedPackedTimestamp->payload.data());
                    isCacheActual = (cachedTimestamp >= modificationTime);
                }
                if (!isCacheActual)
                {
                    static const int kCachedFileLifeTimeSec = 3600;
                    auto fileContent = File::readAll(fileName);
                    std::vector<char> vectorizedFileContent(fileContent.cbegin(), fileContent.cend());
                    cachedPackedFile = fccCache.add(kFccFileStore, vectorizedFileContent, kCachedFileLifeTimeSec);
                    
                    std::vector<char> vectorizedModificationTime;
                    vectorizedModificationTime.resize(sizeof(modificationTime));
                    static_assert(sizeof(modificationTime) == sizeof(int), "Impossible to save modification time to int");
                    *reinterpret_cast<int*>(vectorizedModificationTime.data()) = modificationTime;
                    cachedPackedTimestamp = fccCache.add(kFccFileTimeStampStore, vectorizedModificationTime, kCachedFileLifeTimeSec);
                    
                    cachedTimestamp = modificationTime;
                    isCacheActual = true;
                }
                std::string cachedFile(cachedPackedFile->payload.data(), cachedPackedFile->payload.size());
                customAttributes.set(CustomRequestAttributes::kContentRequestKey, cachedFile);
                customAttributes.set(CustomRequestAttributes::kContentTimestampRequestKey, StringBuilder() << cachedTimestamp);
            }
            return process(incomingRequest);
        }
        else
        {
            RequestHeadersWrapper requestHeadersWrapper(incomingRequest);
            requestHeadersWrapper.copy(responseHeaders, RequestHeadersWrapper::TargetHeaders::Out);
            
            auto additionalCookies = gatewayResponse.getSubresponseCookies();
            requestHeadersWrapper.copyCookies(additionalCookies, RequestHeadersWrapper::TargetHeaders::Out);
            
            if (gatewayResponse.isSubresponseBodySet())
            {
                auto moduleResponse = base64_decode(gatewayResponse.getSubresponseBody());
                ap_rwrite(moduleResponse.c_str(), static_cast<int>(moduleResponse.size()), incomingRequest);
            }
            return httpStatus;
        }
        
        return OK;
    }

    void IdfcFilter::appendToExtendedDump(const std::string& information) const
    {
        if (!extentededDumpFileName.empty())
        {
            std::ofstream dumpFile(extentededDumpFileName, std::ofstream::out | std::ofstream::app);
            if (dumpFile.is_open())
            {
                dumpFile << information << std::endl;
                dumpFile.close();
            }
        }
    }
}
