#include "JavaUtils.h"
#include "StringProcessor.h"

namespace idfc
{
    Json::Value JavaUtils::toJavaCookies(const std::string& unparsedCookies)
    {
        Json::Value javaCookies;
        auto unparsedPairs = StringProcessor::split(unparsedCookies, ";");
        for (const auto& unparsedPair : unparsedPairs)
        {
            auto cookieKeyValue = StringProcessor::split(unparsedPair, "=");
            if (cookieKeyValue.size() == 2)
            {
                Json::Value javaCookie;
                javaCookie["Name"] = StringProcessor::trimmed(cookieKeyValue[0]);
                javaCookie["Value"] = StringProcessor::trimmed(cookieKeyValue[1]);
                // TODO: Research how to define fiels 'Secure' and 'MaxAge'
                javaCookies.append(javaCookie);
            }
        }
        return javaCookies;
    }
}
