#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace xtrap
{
    class BacktraceStack : public std::vector<std::string>
    {
    public:
        operator bool() const
        {
            return !empty();
        }
    };

    inline std::ostream& operator << (std::ostream& os, const BacktraceStack& stack)
    {
        for (auto& entry : stack)
        {
            os << entry << std::endl;
        }
        return os;
    }
}