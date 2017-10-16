#include <httpd.h>
#include <http_core.h>
#include <http_log.h>
#include <http_protocol.h>
#include <http_request.h>

#include "Exceptions.h"
#include "IdfcFiltersPool.h"
#include "Logger.h"

using namespace idfc;

static const char* setEnabled(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setAccountName(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setGatewayUrl(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setGatewayToken(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setPluginId(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setSecretKey(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setCACertPath(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setIgnoredExtensions(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setIgnoredHosts(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setIgnoredUrls(cmd_parms* command, void* /*config*/, const char* argument);
static const char* setExtendedDumpFile(cmd_parms* command, void* /*config*/, const char* argument);
static void registerHooks(apr_pool_t* /*pool*/);

static const command_rec moduleDirectives[] =
{
    AP_INIT_TAKE1("IdfcEnabled",            reinterpret_cast<cmd_func>(setEnabled), NULL, OR_ALL, 
        "Enable or disable mod_idfc"),
    AP_INIT_TAKE1("IdfcAccountName",        reinterpret_cast<cmd_func>(setAccountName), NULL, OR_ALL, ""),
    AP_INIT_TAKE1("IdfcGatewayUrl",         reinterpret_cast<cmd_func>(setGatewayUrl), NULL, OR_ALL, 
        "E.g. https://www.idfconnect.net/ssorest/service/gateway/evaluate"),
    AP_INIT_TAKE1("IdfcGatewayToken",       reinterpret_cast<cmd_func>(setGatewayToken), NULL, OR_ALL, ""),
    AP_INIT_TAKE1("IdfcPluginId",           reinterpret_cast<cmd_func>(setPluginId), NULL, OR_ALL, ""),
    AP_INIT_TAKE1("IdfcSecretKey",          reinterpret_cast<cmd_func>(setSecretKey), NULL, OR_ALL, ""),
    AP_INIT_TAKE1("IdfcCACertPath",         reinterpret_cast<cmd_func>(setCACertPath), NULL, OR_ALL, 
        "Path to certificate in order to verify gateway"),
    AP_INIT_TAKE1("IdfcIgnoredExtensions",  reinterpret_cast<cmd_func>(setIgnoredExtensions), NULL, OR_ALL, ""),
    AP_INIT_TAKE1("IdfcIgnoredHosts",       reinterpret_cast<cmd_func>(setIgnoredHosts), NULL, OR_ALL, ""),
    AP_INIT_TAKE1("IdfcIgnoredUrls",        reinterpret_cast<cmd_func>(setIgnoredUrls), NULL, OR_ALL, ""),
    AP_INIT_TAKE1("IdfcExtendedDumpFile",   reinterpret_cast<cmd_func>(setExtendedDumpFile), NULL, OR_ALL,
        "If set, write ALL requests and responses to/from gateway server to the specified file. Use ONLY in DEBUG environment!"),
    { NULL }
};

extern "C" 
{
    AP_DECLARE_MODULE(idfc) =
    {
        STANDARD20_MODULE_STUFF,
        NULL,
        NULL,
        NULL,
        NULL,
        moduleDirectives,
        registerHooks
    };
}

namespace idfc
{
    int getApacheModuleIndex()
    {
        return APLOG_MODULE_INDEX;
    }

    static std::shared_ptr<IdfcFilter> getIdfcFilterFrom(server_rec* server)
    {
        static IdfcFiltersPool idfcFiltersPool;
        auto rawVirtualServerName = server->server_hostname;
        if (!rawVirtualServerName)
            throw std::logic_error("ServerName is not set, check virtual server settings");
        std::string virtualServerName(rawVirtualServerName);
        Logger::debug(server, "Virtual server name: %s", virtualServerName.c_str());
        return idfcFiltersPool[virtualServerName];
    }
}

static bool isOn(const char* value)
{
    return (::strcasecmp(value, "on") == 0);
}

static const char* setEnabled(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto idfcFilter = getIdfcFilterFrom(command->server);
    if (isOn(argument))
        idfcFilter->enable();
    else
        idfcFilter->disable();
    return NULL;
}

static const char* setAccountName(cmd_parms* command, void* /*config*/, const char* argument)
{
    Logger::debug(command->server, "Settings value: AccountName = %s", argument);
    auto idfcFilter = getIdfcFilterFrom(command->server);
    idfcFilter->setAccountName(argument);
    return NULL;
}

static const char* setGatewayUrl(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto idfcFilter = getIdfcFilterFrom(command->server);
    idfcFilter->setGatewayUrl(argument);
    return NULL;
}

static const char* setGatewayToken(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto idfcFilter = getIdfcFilterFrom(command->server);
    idfcFilter->setGatewayToken(argument);
    return NULL;
}

static const char* setPluginId(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto idfcFilter = getIdfcFilterFrom(command->server);
    idfcFilter->setPluginId(argument);
    return NULL;
}

static const char* setSecretKey(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto idfcFilter = getIdfcFilterFrom(command->server);
    idfcFilter->setSecretKey(argument);
    return NULL;
}

static const char* setCACertPath(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto idfcFilter = getIdfcFilterFrom(command->server);
    idfcFilter->setCACertPath(argument);
    return NULL;
}

static const char* setIgnoredExtensions(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto idfcFilter = getIdfcFilterFrom(command->server);
    idfcFilter->setIgnoredExtensions(argument);
    return NULL;
}

static const char* setIgnoredHosts(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto idfcFilter = getIdfcFilterFrom(command->server);
    idfcFilter->setIgnoredHosts(argument);
    return NULL;
}

static const char* setIgnoredUrls(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto idfcFilter = getIdfcFilterFrom(command->server);
    idfcFilter->setIgnoredUrls(argument);
    return NULL;
}

static const char* setExtendedDumpFile(cmd_parms* command, void* /*config*/, const char* argument)
{
    auto idfcFilter = getIdfcFilterFrom(command->server);
    idfcFilter->setExtendedDumpFile(argument);
    return NULL;
}

static int processRequest(request_rec* request)
{ 
    int httpResult = HTTP_SERVICE_UNAVAILABLE;
    try
    {
        auto idfcFilter = getIdfcFilterFrom(request->server);
        httpResult = idfcFilter->process(request);
    }
    catch (const IdfcException& e)
    {
        ap_log_error(APLOG_MARK, APLOG_ERR, 0, request->server, "%s", e.what());
    }
    catch (...)
    {
        ap_log_error(APLOG_MARK, APLOG_ERR, 0, request->server, "Unhandled exception has been caught");
    }
    return httpResult;
}

static void registerHooks(apr_pool_t* /*pool*/)
{
    ::ap_hook_handler(processRequest, NULL, NULL, APR_HOOK_LAST);
}
