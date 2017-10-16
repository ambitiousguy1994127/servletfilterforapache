#pragma once

#include <apr_tables.h>
#include <apr_strings.h>
#include <httpd.h>

#include <map>
#include <string>

namespace idfc
{
    class RequestHeadersWrapper
    {
    public:
        enum class TargetHeaders { In, Out };
        
    private:
        request_rec* request;
        
    public:
        RequestHeadersWrapper(request_rec* request) : request(request)
        {
        }
        
    public:
        void copy(const std::map<std::string, std::string>& sourceHeaders, TargetHeaders targetHeaders)
        {
            auto destinationHeaders = (targetHeaders == TargetHeaders::In ? request->headers_in : request->headers_out);
            for (auto& sourceHeader : sourceHeaders)
            {
                auto& name = sourceHeader.first;
                auto& value = sourceHeader.second;
                ::apr_table_set(destinationHeaders, name.c_str(), value.c_str());
            }
        }
        
    public:
        void copyCookies(const std::map<std::string, std::string>& sourceCookies, TargetHeaders targetHeaders)
        {
            std::string mergedCookies;
            auto destinationHeaders = (targetHeaders == TargetHeaders::In ? request->headers_in : request->headers_out);
            auto rawCurrentCookies = ::apr_table_get(destinationHeaders, "Cookie");
            if (rawCurrentCookies) mergedCookies.assign(rawCurrentCookies);
            for (const auto& sourceCookie : sourceCookies)
            {
                if (!mergedCookies.empty()) mergedCookies += "; ";
                mergedCookies += sourceCookie.first + "=" + sourceCookie.second;
            }
            ::apr_table_set(destinationHeaders, "Cookie", mergedCookies.c_str());
        }
    };
}
