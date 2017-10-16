#pragma once

#include <sstream>

namespace idfc
{
    class StringBuilder
    {
    private:
        std::stringstream buffer;

    public:
        StringBuilder() {}
        virtual ~StringBuilder() {}
    private:
        StringBuilder(const StringBuilder&);
        StringBuilder& operator=(StringBuilder&);

    public:
        template<typename T> StringBuilder& operator<<(const T& value)
        {
            buffer << value;
            return *this;
        }

    public:
        operator std::string() const
        {
            return buffer.str();
        }
    };
}
