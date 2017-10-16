#pragma once

#include <httpd.h>
#include <set>
#include <string>

namespace idfc
{
    class IgnorePattern
    {
    private:
        std::set<std::string> ignoredExtensions;
        std::set<std::string> ignoredHosts;
        std::set<std::string> ignoredUrls;
        
    public:
        bool isRequestShouldBeRejected(request_rec* request) const;
        void setIgnoredExtensions(const std::string& value);
        void setIgnoredHosts(const std::string& value);
        void setIgnoredUrls(const std::string& value);
        
    private:
        std::set<std::string> prepareValues(const std::string& initialString);
    };
}