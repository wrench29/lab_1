#pragma once

#include <string>
#include <vector>

enum class Shapes 
{
    Circle,
    Triangle,
    Rectangle,
    Pentagon
};

enum class Colors
{
    Red,
    Green,
    Blue,
    White,
    Black
};


void ltrim(std::string& str);
void rtrim(std::string& str);
void trim(std::string& str);
void remove_doublespaces(std::string& str);
void to_lower(std::string& str);
std::vector<std::string> split(std::string str, std::string separator);
bool file_exists(std::string path);
std::string shape_to_string(Shapes shape);
std::string color_to_string(Colors color);
Shapes string_to_shape(std::string shape);
Colors string_to_color(std::string color);
