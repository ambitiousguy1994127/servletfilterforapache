#include "RequestParameters.h"
#include "StringProcessor.h"

namespace idfc
{
    std::string RequestParameters::joinHttpHeaders(const Json::Value& tree)
    {
        std::string result;
        for (auto& key : tree.getMemberNames())
        {
            auto value = tree[key][static_cast<Json::Value::ArrayIndex>(0)].asString();
            result += key + ':' + value + "\r\n";
        }
        return result;
    }
    
    std::string RequestParameters::joinHttpHeadersWithProcessing(const Json::Value& tree)
    {
        std::string result;
        for (auto& key : tree.getMemberNames())
        {
            auto value = tree[key][static_cast<Json::Value::ArrayIndex>(0)].asString();
            auto processedKey = StringProcessor::toUpperCase(key);
            StringProcessor::replace(processedKey, '-', '_');
            result += std::string("HTTP_") + processedKey + ':' + value + "\r\n";
        }
        return result;
    }
    
    Json::Value RequestParameters::toSingleElementArray(const std::string& source)
    {
        Json::Value result;
        result.append(source);
        return result;
    }
}
