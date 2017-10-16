#pragma once

#include "json/json.h"
#include <string>

namespace idfc
{
    class RequestParameters
    {
    public:
        static std::string joinHttpHeaders(const Json::Value& tree);
        static std::string joinHttpHeadersWithProcessing(const Json::Value& tree);
        static Json::Value toSingleElementArray(const std::string& source);
    };
}
