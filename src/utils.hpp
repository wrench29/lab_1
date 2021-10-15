#pragma once

#include <string>
#include <vector>

void ltrim(std::string str);
void rtrim(std::string str);
void trim(std::string str);
void remove_doublespaces(std::string str);
void to_lower(std::string str);
std::vector<std::string> split(std::string str, std::string separator);