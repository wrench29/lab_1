#pragma once

#include <string>

class Caretaker
{
public:
    Caretaker() = delete;
    static void save_scene(std::string name);
    static void restore_scene(std::string name);
};
