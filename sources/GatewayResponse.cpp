#include "Exceptions.h"
#include "GatewayResponse.h"

namespace idfc
{
    GatewayResponse::GatewayResponse(const std::string& jsonResponse)
    {
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(jsonResponse, root))
            throw JsonCppException(std::string("Failed to parse gateway response: ") + reader.getFormattedErrorMessages());

        subrequest = root["request"];
        subresponse = root["response"];
    }
    
    const Json::Value& GatewayResponse::getSubrequest() const
    {
        return subrequest;
    }
    
    const Json::Value& GatewayResponse::getSubresponse() const
    {
        return subresponse;
    }
    
    int GatewayResponse::getSubresponseStatus() const
    {
        auto jsonValue = subresponse["status"];
        if (jsonValue.isNull())
            throw AssertionException("Subresponse doesn't contain key \"status\"");
        try
        {
            return jsonValue.asInt();
        }
        catch (...)
        {
            throw AssertionException("Value \"status\" in subrespose is not convertable to integer");
        }
    }
    
    std::string GatewayResponse::getSubresponseBody() const
    {
        auto jsonValue = subresponse["body"];
        if (jsonValue.isNull())
            throw AssertionException("Subresponse doesn't contain key \"body\"");
        try
        {
            return jsonValue.asString();
        }
        catch (...)
        {
            throw AssertionException("Value \"body\" in subrespose is not convertable to string");
        }
    }
    
    bool GatewayResponse::isSubresponseBodySet() const
    {
        auto jsonValue = subresponse["body"];
        return !jsonValue.isNull();
    }
    
    std::map<std::string, std::string> GatewayResponse::getSubresponseHeaders() const
    {
        std::map<std::string, std::string> headers;
        try
        {
            auto jsonHeaders = subresponse["headers"];
            if (!jsonHeaders.isNull())
            {
                for (auto i = jsonHeaders.begin(); i != jsonHeaders.end(); ++i)
                {
                    auto name = i.key().asString();
                    auto value = (*i)[static_cast<Json::Value::ArrayIndex>(0)].asString();
                    headers[name] = value;
                }
            }
        }
        catch (const std::exception& e)
        {
            throw JsonCppException(StringBuilder() << "Failed to parse subresponse's JSON-headers: " << e.what());
        }
        return headers;
    }
    
    std::map<std::string, std::string> GatewayResponse::getSubresponseCookies() const
    {
        std::map<std::string, std::string> cookies;
        try
        {
            auto jsonCookies = subresponse["cookies"];
            if (!jsonCookies.isNull())
            {
                for (Json::Value::ArrayIndex i = 0; i < jsonCookies.size(); ++i)
                {
                    auto name = jsonCookies[i]["name"].asString();
                    auto value = jsonCookies[i]["value"].asString();
                    cookies[name] = value;
                }
            }
        }
        catch (const std::exception& e)
        {
            throw JsonCppException(StringBuilder() << "Failed to parse subresponse's JSON-cookies: " << e.what());
        }
        return cookies;
    }
}
