#include "CustomRequestAttributes.h"
#include "Exceptions.h"

namespace idfc
{ 
    const std::string CustomRequestAttributes::kAccountNameKey("acoName");
    const std::string CustomRequestAttributes::kGatewayTokenKey("gatewayToken");
    const std::string CustomRequestAttributes::kPluginIdKey("pluginID");
    const std::string CustomRequestAttributes::kRandomTextKey("randomText");
    const std::string CustomRequestAttributes::kRandomTextSignedKey("randomTextSigned");
    const std::string CustomRequestAttributes::kContentRequestKey("content");
    const std::string CustomRequestAttributes::kContentTimestampRequestKey("contentTimestamp");
    
    const std::vector<std::string> CustomRequestAttributes::kCustomAttributesKeys
    ({
        CustomRequestAttributes::kAccountNameKey, 
        CustomRequestAttributes::kGatewayTokenKey, 
        CustomRequestAttributes::kPluginIdKey,
        CustomRequestAttributes::kRandomTextKey,
        CustomRequestAttributes::kRandomTextSignedKey,
        CustomRequestAttributes::kContentRequestKey,
        CustomRequestAttributes::kContentTimestampRequestKey
    });
        
    const std::string CustomRequestAttributes::kCustomAttributePrefix("_#IDFConnect_Filter_Agent#_");
    
    CustomRequestAttributes::CustomRequestAttributes(request_rec* request) : 
        isRequestModifiable(true),
        request(request)
    {
    }
    
    CustomRequestAttributes::CustomRequestAttributes(const request_rec* request) :
        isRequestModifiable(false),
        request(const_cast<request_rec*>(request))
    {
    }
    
    void CustomRequestAttributes::set(const std::string& key, const std::string& value)
    {
        if (!isRequestModifiable)
            throw AssertionException("Request is not modifiable");
        
        auto prefixedKey = kCustomAttributePrefix + key;
        auto detachedValue = new std::string(value);
        auto status = ::apr_pool_userdata_set(detachedValue, prefixedKey.c_str(), cleanupCustomAttribute, request->pool);
        if (status != APR_SUCCESS)
        {
            delete detachedValue;
            throw ApacheException(StringBuilder() << "Function ::apr_pool_userdata_set() returned " << status);
        }
    }
    
    apr_status_t CustomRequestAttributes::cleanupCustomAttribute(void* untypedCustomAttribute)
    {
        auto customAttribute = reinterpret_cast<std::string*>(untypedCustomAttribute);
        delete customAttribute;
        return APR_SUCCESS;
    }
    
        
    std::string CustomRequestAttributes::get(const std::string& key) const
    {
        std::string* valuePointer = nullptr;
        auto prefixedKey = kCustomAttributePrefix + key;
        auto status = ::apr_pool_userdata_get(reinterpret_cast<void**>(&valuePointer), prefixedKey.c_str(), request->pool);
        if (status != APR_SUCCESS)
            throw ApacheException(StringBuilder() << "Function ::apr_pool_userdata_get() returned " << status);
        
        std::string value;
        if (valuePointer)
            value.assign(*valuePointer);
        return value;
    }
    
    std::map<std::string, std::string> CustomRequestAttributes::enumerateNonemptyAttributes() const
    {
        std::map<std::string, std::string> result;
        for (auto& customAttributeKey : kCustomAttributesKeys)
        {
            auto value = get(customAttributeKey);
            if (!value.empty())
                result[customAttributeKey] = value;
        }
        return result;
    }
}