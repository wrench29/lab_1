#include <algorithm>

#include "utils.hpp"

void ltrim(std::string str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}
void rtrim(std::string str)
{
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), str.end());
}
void trim(std::string str)
{
    ltrim(str);
    rtrim(str);
}
void remove_doublespaces(std::string str)
{
    std::size_t doubleSpace = str.find("  ");
    while (doubleSpace != std::string::npos)
    {
        str.erase(doubleSpace, 1);
        doubleSpace = str.find("  ");
    }
}
void to_lower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { 
        return std::tolower(c); 
    });
}
std::vector<std::string> split(std::string str, std::string separator)
{
    size_t pos = 0;
    std::string token;
    std::string strc(str.c_str());
    std::vector<std::string> vec;
    while ((pos = strc.find(separator)) != std::string::npos) 
    {
        token = strc.substr(0, pos);
        vec.push_back(token);
        strc.erase(0, pos + separator.length());
    }
    vec.push_back(strc);
    return vec;
}
