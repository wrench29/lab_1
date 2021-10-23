#pragma once

#include <vector>
#include <string>

class Scene;
class Caretaker;

class Memento
{
public:
    virtual ~Memento() {};
    virtual std::vector<std::string> get_state() const = 0;
};

class SceneMemento : Memento
{
public:
    virtual ~SceneMemento() {};
    virtual std::vector<std::string> get_state() const
    {
        return this->state;
    }
private:
    SceneMemento(std::vector<std::string> state) : state(state) {};
    std::vector<std::string> state;

friend class Scene;
friend class Caretaker;
};
