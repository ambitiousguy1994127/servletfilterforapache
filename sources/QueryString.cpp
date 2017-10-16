#include "QueryString.h"
#include "StringProcessor.h"

namespace idfc
{
    std::map<std::string, std::string> QueryString::parse(const std::string& unparsedQueryString)
    {
        std::map<std::string, std::string> parameters;
        auto keyValuePairs= StringProcessor::split(unparsedQueryString, "&");
        for (auto& unparsedKeyValuePair : keyValuePairs)
        {
            auto keyValuePair = StringProcessor::split(unparsedKeyValuePair, "=");
            auto& key = keyValuePair[0];
            if (keyValuePair.size() == 1)
            {
                parameters[key] = "";
            }
            else if (keyValuePair.size() == 2)
            {
                auto& value = keyValuePair[1];
                parameters[key] = value;
            }
        }
        return parameters;
    }
}
