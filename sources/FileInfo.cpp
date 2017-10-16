#include "Exceptions.h"
#include "FileInfo.h"

namespace idfc
{
    FileInfo::FileInfo(const std::string& fileName)
    {
        if (::stat(fileName.c_str(), &statBuffer) != 0)
            throw AssertionException(std::string("Failed to get information about file ") + fileName);
    }
    
    int FileInfo::getModificationTime() const
    {
        return statBuffer.st_mtime;
    }
}
