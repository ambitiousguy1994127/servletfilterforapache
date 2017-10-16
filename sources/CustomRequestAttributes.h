#pragma once

#include <httpd.h>
#include <map>
#include <string>
#include <vector>

namespace idfc
{
    class CustomRequestAttributes
    {
    public:
        static const std::string kAccountNameKey;
        static const std::string kGatewayTokenKey;
        static const std::string kPluginIdKey;
        static const std::string kRandomTextKey;
        static const std::string kRandomTextSignedKey;
        static const std::string kContentRequestKey;
        static const std::string kContentTimestampRequestKey;
        
        static const std::vector<std::string> kCustomAttributesKeys;
    
    private:
        static const std::string kCustomAttributePrefix;
    
    private:
        bool isRequestModifiable;
        request_rec* request;
        
    public:
        CustomRequestAttributes(request_rec* request);
        CustomRequestAttributes(const request_rec* request);
        
    public:
        void set(const std::string& key, const std::string& value);
        std::string get(const std::string& key) const;
        std::map<std::string, std::string> enumerateNonemptyAttributes() const;
        
    private:
        static apr_status_t cleanupCustomAttribute(void* untypedCustomAttribute);
    };
}