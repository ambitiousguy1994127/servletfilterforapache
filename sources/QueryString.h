#pragma once

#include <map>
#include <string>

namespace idfc
{
    class QueryString
    {
    public:
        static std::map<std::string, std::string> parse(const std::string& unparsedQueryString);
    };
}