#include <array>
#include <apr.h>
#include <http_core.h>
#include <sys/types.h>

#include "CurlList.h"
#include "CustomRequestAttributes.h"
#include "Exceptions.h"
#include "GatewayRequest.h"
#include "JavaUtils.h"
#include "Logger.h"
#include "QueryString.h"
#include "RequestParameters.h"
#include "StringBuilder.h"
#include "StringProcessor.h"
#include "TypesConverter.h"

const char *ap_run_http_scheme(const request_rec *r);

namespace idfc
{
    GatewayRequest::GatewayRequest(const request_rec* sourceRequest, const std::string& fqdn) : 
        server(sourceRequest->server),
        scheme(getScheme(sourceRequest))
    {
        // attributes = AttributesHelper.GetAllAttributes(context),
        auto attributes = CustomRequestAttributes(sourceRequest).enumerateNonemptyAttributes();
        for (auto& attribute : attributes)
            jsonData["attributes"][attribute.first] = attribute.second;
        
        // method = request.HttpMethod,
        auto requestMethod = TypesConverter::toStringSafety(sourceRequest->method);
        jsonData["method"] = requestMethod;
        // url = request.Url,
        auto url = scheme + "://" + std::string(sourceRequest->hostname) + sourceRequest->unparsed_uri;
        jsonData["url"] = url;
        // protocol = request.Params["SERVER_PROTOCOL"],
        auto serverProtocol = TypesConverter::toStringSafety(sourceRequest->protocol);
        jsonData["protocol"] = serverProtocol;
        // characterEncoding = request.ContentEncoding.BodyName,
        jsonData["characterEncoding"] = TypesConverter::toStringSafety(sourceRequest->content_encoding);
        // contentLength = request.ContentLength,
        jsonData["contentLength"] = static_cast<Json::Value::UInt>(sourceRequest->clength);
        // contentType = request.ContentType,
        auto contentType = TypesConverter::toStringSafety(sourceRequest->content_type);
        jsonData["contentType"] = contentType;
        // contextPath = GetContextPath(request),
        jsonData["contextPath"] = getContextPath(sourceRequest);
        // localAddr = request.ServerVariables["LOCAL_ADDR"],
        auto localAddr = TypesConverter::toStringSafety(sourceRequest->connection->local_ip);
        jsonData["localAddr"] = localAddr;
        // localName = request.Url.Host,
        jsonData["localName"] = TypesConverter::toStringSafety(sourceRequest->connection->local_host);
        // localPort = request.Url.Port,
        jsonData["localPort"] = sourceRequest->server->port;
        // remoteAddr = request.ServerVariables["REMOTE_ADDR"],
        auto remoteAddr = TypesConverter::toStringSafety(sourceRequest->useragent_ip);
        jsonData["remoteAddr"] = remoteAddr;
        // remoteHost = request.ServerVariables["REMOTE_HOST"],
        auto remoteHost = ap_get_remote_host(sourceRequest->connection, sourceRequest->per_dir_config, REMOTE_NAME, NULL);
        jsonData["remoteHost"] = remoteHost;
        // remotePort = request.ServerVariables["REMOTE_PORT"],
        auto remotePort = sourceRequest->useragent_addr->port;
        jsonData["remotePort"] = remotePort;
        // secure = request.IsSecureConnection,
        jsonData["secure"] = isSecureProtocol();
        // sessionidfromcookie = false,
        jsonData["sessionidfromcookie"] = false;
        // sessionidfromurl = false,
        jsonData["sessionidfromurl"] = false;
        // sessionidvalid = false,
        jsonData["sessionidvalid"] = false;
        // scheme = request.Url.Scheme,
        jsonData["scheme"] = scheme;
        // serverName = GetFQDN(),
        jsonData["serverName"] = fqdn;
        // serverPort = request.Url.Port,
        auto serverPort = sourceRequest->server->port;
        jsonData["serverPort"] = serverPort;
        // servletPath = string.Empty,
        jsonData["servletPath"] = std::string();
        
        // locales = request.UserLanguages,
        auto emptyArray = Json::Value(Json::arrayValue);
        auto contentLanguages = TypesConverter::toVector(sourceRequest->content_languages);
        if (!contentLanguages.empty())
        {
            Json::Value jsonLocales;
            for (auto& contentLanguage : contentLanguages)
                jsonLocales.append(contentLanguage);
            jsonData["locales"] = jsonLocales;
        }
        else jsonData["locales"] = emptyArray;

        // headers = request.Headers.ToDictionary(),
        std::string unparsedCookies;
        auto& jsonParameters = jsonData["parameters"];
        auto headers = TypesConverter::toMap(sourceRequest->headers_in);
        Json::Value jsonHeaders;
        for (auto& header : headers)
        {
            auto headerName = header.first;
            auto& headerValue = header.second;
            if (headerName == "Cookie") unparsedCookies.assign(headerValue);
            jsonHeaders[headerName] = RequestParameters::toSingleElementArray(headerValue);
            auto httpHeaderName = std::string("HTTP_") + StringProcessor::toUpperCase(headerName);
            StringProcessor::replace(httpHeaderName, '-', '_');
            jsonParameters[httpHeaderName] = RequestParameters::toSingleElementArray(headerValue);
        }
        jsonData["headers"] = jsonHeaders;
        
        // cookies = request.Cookies.ToJavaCookiesList()
        if (!unparsedCookies.empty())
            jsonData["cookies"] = JavaUtils::toJavaCookies(unparsedCookies);
        else
            jsonData["cookies"] = emptyArray;
        
        auto allHttp =  RequestParameters::joinHttpHeadersWithProcessing(jsonHeaders);
        jsonParameters["ALL_HTTP"] = RequestParameters::toSingleElementArray(allHttp);
        auto allRaw = RequestParameters::joinHttpHeaders(jsonHeaders);;
        jsonParameters["ALL_RAW"] = RequestParameters::toSingleElementArray(allRaw);
        // NOTICE: APPL_MD_PATH, APPL_PHYSICAL_PATH are IIS server variables
        auto emptyStringInArray = RequestParameters::toSingleElementArray("");
        jsonParameters["APPL_MD_PATH"] = emptyStringInArray;
        jsonParameters["APPL_PHYSICAL_PATH"] = RequestParameters::toSingleElementArray(sourceRequest->filename);
        jsonParameters["AUTH_TYPE"] = emptyStringInArray;
        jsonParameters["AUTH_USER"] = emptyStringInArray;
        jsonParameters["AUTH_PASSWORD"] = emptyStringInArray;
        jsonParameters["LOGON_USER"] = emptyStringInArray;
        jsonParameters["REMOTE_USER"] = emptyStringInArray;
        jsonParameters["CERT_COOKIE"] = emptyStringInArray;
        jsonParameters["CERT_FLAGS"] = emptyStringInArray;
        jsonParameters["CERT_ISSUER"] = emptyStringInArray;
        jsonParameters["CERT_KEYSIZE"] = emptyStringInArray;
        jsonParameters["CERT_SECRETKEYSIZE"] = emptyStringInArray;
        jsonParameters["CERT_SERIALNUMBER"] = emptyStringInArray;
        jsonParameters["CERT_SERVER_ISSUER"] = emptyStringInArray;
        jsonParameters["CERT_SERVER_SUBJECT"] = emptyStringInArray;
        jsonParameters["CERT_SUBJECT"] = emptyStringInArray;
        jsonParameters["CONTENT_LENGTH"] = RequestParameters::toSingleElementArray(StringBuilder() << sourceRequest->clength);
        jsonParameters["CONTENT_TYPE"] = RequestParameters::toSingleElementArray(contentType);
        // TODO: Implement getting the following value from request or configuration file
        jsonParameters["GATEWAY_INTERFACE"] = RequestParameters::toSingleElementArray("CGI/1.1");
        jsonParameters["HTTPS"] = RequestParameters::toSingleElementArray(isSecureProtocol() ? "on" : "off");
        // TODO: Implement setting HTTPS_* if "HTTPS" is on
        jsonParameters["HTTPS_KEYSIZE"] = emptyStringInArray;
        jsonParameters["HTTPS_SECRET_KEYSIZE"] = emptyStringInArray;
        jsonParameters["HTTPS_SERVER_ISSUER"] = emptyStringInArray;
        jsonParameters["HTTPS_SERVER_SUBJECT"] = emptyStringInArray;
        // NOTICE: INSTANCE_ID, INSTANCE_META_PATH are IIS server variables
        jsonParameters["INSTANCE_ID"] = emptyStringInArray;
        jsonParameters["INSTANCE_META_PATH"] = emptyStringInArray;
        jsonParameters["LOCAL_ADDR"] = RequestParameters::toSingleElementArray(localAddr);
        // NOTICE: PATH_TRANSLATED is a IIS server variable
        jsonParameters["PATH_TRANSLATED"] = emptyStringInArray;
        auto unparsedQueryString = TypesConverter::toStringSafety(sourceRequest->parsed_uri.query);
        jsonParameters["QUERY_STRING"] = RequestParameters::toSingleElementArray(unparsedQueryString);
        jsonParameters["REMOTE_ADDR"] = RequestParameters::toSingleElementArray(remoteAddr);
        jsonParameters["REMOTE_HOST"] = RequestParameters::toSingleElementArray(remoteHost);
        jsonParameters["REMOTE_PORT"] = RequestParameters::toSingleElementArray(StringBuilder() << remotePort);
        jsonParameters["REQUEST_METHOD"] = RequestParameters::toSingleElementArray(requestMethod);
        // NOTICE: SCRIPT_NAME is a IIS server variable
        auto scriptName = TypesConverter::toStringSafety(sourceRequest->parsed_uri.path);
        jsonParameters["SCRIPT_NAME"] = RequestParameters::toSingleElementArray(scriptName);
        jsonParameters["SERVER_NAME"] = RequestParameters::toSingleElementArray(fqdn);
        jsonParameters["SERVER_PORT"] = RequestParameters::toSingleElementArray(StringBuilder() << serverPort);
        jsonParameters["SERVER_PORT_SECURE"] = RequestParameters::toSingleElementArray(isSecureProtocol() ? "1" : "0");;
        jsonParameters["SERVER_PROTOCOL"] = RequestParameters::toSingleElementArray(serverProtocol);
        jsonParameters["SERVER_SOFTWARE"] = RequestParameters::toSingleElementArray("Apache");
        jsonParameters["URL"] = RequestParameters::toSingleElementArray(url);
        
        if (requestMethod != "GET")
        {
            auto queryParameters = QueryString::parse(unparsedQueryString);
            
            auto smAgentNameIterator = queryParameters.find("SMAGENTNAME");
            if (smAgentNameIterator != queryParameters.cend())
                jsonParameters["SMAGENTNAME"] = RequestParameters::toSingleElementArray(smAgentNameIterator->second);
            
            auto targetIterator = queryParameters.find("TARGET");
            if (targetIterator != queryParameters.cend())
                jsonParameters["TARGET"] = RequestParameters::toSingleElementArray(targetIterator->second);
        }
    }
   
    // TODO: Try to implement this function via ap_http_scheme()
    std::string GatewayRequest::getScheme(const request_rec* request)
    {
        auto port = getServerPort(request);
        switch (port)
        {
            case 80: 
                return "http";
            case 443: 
                return "https";
            default: 
                return "";
        }
    }
    
    bool GatewayRequest::isSecureProtocol() const
    {
        return (scheme == "https");
    }
    
    std::string GatewayRequest::getContextPath(const request_rec* request) const
    {
        auto url = scheme + "://" + request->hostname;
        auto currentPort = getServerPort(request);
        auto defaultPort = isSecureProtocol() ? 443 : 80;
        if (currentPort != 0 && currentPort != defaultPort)
            url += std::string(StringBuilder() << ':' << currentPort);
        return url;
    }
    
    int GatewayRequest::getServerPort(const request_rec* request)
    {
        auto port = request->server->port;
        if (port == 0)
        {
            auto addresses = request->server->addrs;
            if (addresses)
                // NOTICE: Maybe here we should enumerate all address
                port = addresses->host_port;
        }
        return port;
    }

    const Json::Value& GatewayRequest::getPayload() const
    {
        return jsonData;
    }
    
    void GatewayRequest::setCACertPath(const std::string& path)
    {
        caCertPath = path;
    }
    
    std::string GatewayRequest::sendTo(const std::string& gatewayUrl) const
    {
        CurlWrapper curl;
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_URL, gatewayUrl.c_str()));
        
        Logger::debug(server, "Prepairing HTTP-headers...");
        CurlList headers;
        headers.append("Accept: */*");
        headers.append("Content-Type: application/json");
        headers.append("Charsets: utf-8");
        headers.append("User-Agent: mod_idfc/0.0");
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers.data()));
    
        auto postData = jsonData.toStyledString();
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str()));
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, postData.size()));
        
        CurlWrapper::WriteBuffer rawResponse;
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWrapper::writeCallback));
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_WRITEDATA, &rawResponse));
        
        // Start the cookie engine
        CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""));
        
        if (!caCertPath.empty())
        {
            // To create certificate in PEM-format from P12-format use the following command:
            //   openssl pkcs12 -in test.p12 -out test.cacert.pem -nokeys
            CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_CAINFO, caCertPath.c_str()));
            // We don't verify gateway bacause his certificate expired
            CurlWrapper::verifyResult(::curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0));
        }
        
        CurlWrapper::verifyResult(::curl_easy_perform(curl));
        std::string response(rawResponse.data(), rawResponse.size());       
        return response;
    }
    
    std::vector<Cookie> GatewayRequest::extractCookiesFrom(CurlWrapper& curl)
    {
        struct curl_slist* rawCookies;
        CurlWrapper::verifyResult(::curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &rawCookies));
        
        std::vector<Cookie> cookies;
        auto rawCookie = rawCookies;
        while (rawCookie) 
        {
            cookies.emplace_back(rawCookie->data);
            rawCookie = rawCookie->next;
        }
        
        ::curl_slist_free_all(rawCookies);
        return cookies;
    }
}