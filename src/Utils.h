#ifndef UTILS_H
#define UTILS_H

#include <string>

class Utils
{
public:
    static std::string trim(const std::string &str)
    {
        size_t first = str.find_first_not_of(' ');
        if (std::string::npos == first)
        {
            return str;
        }
        size_t last = str.find_last_not_of(' ');
        return str.substr(first, (last - first + 1));
    }
};

#endif // UTILS_H
