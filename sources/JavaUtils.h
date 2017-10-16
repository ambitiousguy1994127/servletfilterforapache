#include "json/json.h"
#include <string>

namespace idfc
{
    class JavaUtils
    {
    public:
        static Json::Value toJavaCookies(const std::string& unparsedCookies);
    };
}
    
