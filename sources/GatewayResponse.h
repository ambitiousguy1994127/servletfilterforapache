#pragma once

#include "json/json.h"
#include <map>
#include <string>

namespace idfc
{
    class GatewayResponse
    {
    private:
        Json::Value subrequest;
        Json::Value subresponse;
        
    public:
        GatewayResponse(const std::string& jsonResponse);
        
    public:
        const Json::Value& getSubrequest() const;
        const Json::Value& getSubresponse() const;
        int getSubresponseStatus() const;
        std::string getSubresponseBody() const;
        bool isSubresponseBodySet() const;
        std::map<std::string, std::string> getSubresponseHeaders() const;
        std::map<std::string, std::string> getSubresponseCookies() const;
    };
}
