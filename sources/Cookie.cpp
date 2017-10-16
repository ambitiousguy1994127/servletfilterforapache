#include "Cookie.h"
#include "StringProcessor.h"

namespace idfc
{
    Cookie::Cookie(const std::string& unparsedCurlCookie)
    {
        if (!unparsedCurlCookie.empty()) 
        {
            auto properties = StringProcessor::split(unparsedCurlCookie, "\t");
            domain = StringProcessor::trimmed(properties[0]);
            auto securityFlag = StringProcessor::toLowerCase(StringProcessor::trimmed(properties[3]));
            isSecure = (securityFlag == "true");
            expires = StringProcessor::trimmed(properties[4]);
            name = StringProcessor::trimmed(properties[5]);
            value = StringProcessor::trimmed(properties[6]);
        }
    } 
}