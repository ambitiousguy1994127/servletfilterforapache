#include <fstream>
#include <streambuf>

#include "File.h"

namespace idfc
{
    std::string File::readAll(const std::string& fileName)
    {
        std::ifstream fileStream(fileName.c_str());
        return std::string((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
    }
}