#include <algorithm>
#include <fstream>

#include "utils.hpp"

void ltrim(std::string& str)
{
    str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}
void rtrim(std::string& str)
{
    str.erase(std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), str.end());
}
void trim(std::string& str)
{
    ltrim(str);
    rtrim(str);
}
void remove_doublespaces(std::string& str)
{
    std::size_t doubleSpace = str.find("  ");
    while (doubleSpace != std::string::npos)
    {
        str.erase(doubleSpace, 1);
        doubleSpace = str.find("  ");
    }
}
void to_lower(std::string& str)
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
bool file_exists(std::string path)
{
    std::ifstream a(path);
    if (a.is_open())
    {
        a.close();
        return true;
    }
    else
    {
        return false;
    }
}
std::string shape_to_string(Shapes shape)
{
    switch (shape)
    {
    case Shapes::Circle:
        return "circle";
    case Shapes::Triangle:
        return "triangle";
    case Shapes::Rectangle:
        return "rectangle";
    case Shapes::Pentagon:
        return "pentagon";
    default:
        return "";
    }
}
std::string color_to_string(Colors color)
{
    switch (color)
    {
    case Colors::Red:
        return "red";
    case Colors::Green:
        return "green";
    case Colors::Blue:
        return "blue";
    case Colors::Black:
        return "black";
    case Colors::White:
        return "white";
    default:
        return "";
    }
}

Shapes string_to_shape(std::string shape)
{
    if (shape == "circle")
    {
        return Shapes::Circle;
    }
    else if (shape == "triangle")
    {
        return Shapes::Triangle;
    }
    else if (shape == "rectangle")
    {
        return Shapes::Rectangle;
    }
    else if (shape == "pentagon")
    {
        return Shapes::Pentagon;
    }
    return Shapes::Circle;
}
Colors string_to_color(std::string color)
{
    if (color == "red")
    {
        return Colors::Red;
    }
    else if (color == "green")
    {
        return Colors::Green;
    }
    else if (color == "blue")
    {
        return Colors::Blue;
    }
    else if (color == "black")
    {
        return Colors::Black;
    }
    else if (color == "white")
    {
        return Colors::White;
    }
    return Colors::Red;
}
