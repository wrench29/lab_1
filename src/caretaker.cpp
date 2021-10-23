#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "utils.hpp"
#include "scene.hpp"
#include "memento.hpp"
#include "caretaker.hpp"

void Caretaker::save_scene(std::string name)
{
    std::ofstream file(name);
    if (!file.is_open())
    {
        std::cout << "\nCan't open the file." << std::endl;
        return;
    }

    Scene* scene = Scene::getInstance();
    Memento* current_memento = scene->save_state();
    std::vector<std::string> raw_memento = current_memento->get_state();
    delete current_memento;
    for (std::string line : raw_memento)
    {
        file << line << "\n";
    }
    file.close();
}
void Caretaker::restore_scene(std::string name)
{
    std::ifstream file(name);
    if (!file.is_open())
    {
        std::cout << "\nCan't open the file." << std::endl;
        return;
    }

    std::string line;
    std::vector<std::string> raw_memento;
    bool empty = true;
    while (std::getline(file, line))
    {
        raw_memento.push_back(line);
        if (empty)
        {
            empty = false;
        }
    }

    if (empty)
    {
        std::cout << "\nFile is empty." << std::endl;
        return;
    }

    Scene* scene = Scene::getInstance();
    SceneMemento* memento = new SceneMemento(raw_memento);
    scene->restore_state(memento);
    delete memento;
}
