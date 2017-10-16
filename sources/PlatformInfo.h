#pragma once

#ifdef _WIN32
#define IDFC_WINDOWS
#elif __linux__
#define IDFC_LINUX
#else
#error "Unknown operating system"
#endif

namespace idfc
{
    class PlatformInfo
    {
    public:
        static bool isLinux()
        {
#ifdef IDFC_LINUX
            return true;
#else
            return false;
#endif
        }

        static bool isWindows()
        {
#ifdef IDFC_WINDOWS
            return true;
#else
            return false;
#endif
        }
    };
}
