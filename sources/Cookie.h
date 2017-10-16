#pragma once

#include <string>

namespace idfc
{
    struct Cookie
    {
        std::string domain;
        bool isSecure;
        std::string expires;
        std::string name;
        std::string value;
        
        Cookie(const std::string& unparsedCurlCookie);
    };
}