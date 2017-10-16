#pragma once

#include <httpd.h>
#include <memory>
#include <string>

#include "Cache.h"
#include "IgnorePattern.h"
#include "NetworkInfo.h"

namespace idfc
{
    class IdfcFilter
    {
    public:
        static const char* kHandlerName;
        static const std::string kGatewayTokenKey;
        
    private:
        bool isEnabled;
        std::string accountName;
        std::string gatewayUrl;
        std::string gatewayToken;
        std::string pluginIdKey;
        std::string secretKey;
        std::string caCertPath;
        std::string extentededDumpFileName;
        IgnorePattern ignorePattern;
        
    private:
        Cache fccCache;
        std::string fqdn;
        
    public:
        IdfcFilter();

    public:
        void enable();
        void disable();
        void setAccountName(const std::string& value);
        void setGatewayUrl(const std::string& value);
        void setGatewayToken(const std::string& value);
        void setPluginId(const std::string& value);
        void setSecretKey(const std::string& value);
        void setCACertPath(const std::string& value);
        void setIgnoredExtensions(const std::string& value);
        void setIgnoredHosts(const std::string& value);
        void setIgnoredUrls(const std::string& value);
        void setExtendedDumpFile(const std::string& value);
        int process(request_rec* incomingRequest);
        void appendToExtendedDump(const std::string& information) const;
    };
}