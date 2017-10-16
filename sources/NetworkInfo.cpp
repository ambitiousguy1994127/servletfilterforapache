#include "Exceptions.h"
#include "NetworkInfo.h"
#include "StringBuilder.h"

#ifdef IDFC_LINUX
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#endif

#ifdef IDFC_WINDOWS
#include <Windows.h>
#include <array>
#endif

namespace idfc
{
    std::string NetworkInfo::getFQDN()
    {
#ifdef IDFC_LINUX
        std::lock_guard<std::mutex> locker(networkApiMutex);

        std::array<char, 1024> hostname;
        hostname[hostname.size() - 1] = 0;
        ::gethostname(hostname.data(), hostname.size() - 1);

        struct addrinfo hints;
        ::memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_CANONNAME;

        std::string fqdn;
        struct addrinfo* info = nullptr;
        if (::getaddrinfo(hostname.data(), "http", &hints, &info) == 0)
        {
            if (info) fqdn = info->ai_canonname;
            ::freeaddrinfo(info);
        }
        return fqdn;
#endif

#ifdef IDFC_WINDOWS
        std::array<char, 1024> buffer;
        DWORD valueLength = buffer.size();
        if (!::GetComputerNameExA(ComputerNameDnsFullyQualified, buffer.data(), &valueLength))
            throw NetworkException(StringBuilder() << "::GetComputerNameEx() returned error " << ::GetLastError());
        return std::string(buffer.data());
#endif

        throw AssertionException("Platform is not supported");
    }
}
