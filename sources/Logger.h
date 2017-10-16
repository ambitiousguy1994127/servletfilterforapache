#pragma once

#include <httpd.h>
#include <http_log.h>
#include <http_request.h>

namespace idfc
{
    extern int getApacheModuleIndex();

    class Logger
    {
    public:
        template <typename... Arguments> static void debug(const request_rec* request, const char* fmt, Arguments... arguments)
        {
            debug(request->server, fmt, arguments...);
        }
        
        template <typename... Arguments> static void debug(const server_rec* server, const char* fmt, Arguments... arguments)
        {
            write(APLOG_DEBUG, server, fmt, arguments...);
        }

        template <typename... Arguments> static void info(const request_rec* request, const char* fmt, Arguments... arguments)
        {
            info(request->server, fmt, arguments...);
        }

        template <typename... Arguments> static void info(const server_rec* request, const char* fmt, Arguments... arguments)
        {
            write(APLOG_INFO, request, fmt, arguments...);
        }

        template <typename... Arguments> static void write(int level, const server_rec* server, const char* fmt, Arguments... arguments)
        {
            ap_log_error(__FILE__, __LINE__, getApacheModuleIndex(), APLOG_DEBUG, 0, server, fmt, arguments...);
        }
    };
}