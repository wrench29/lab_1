#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>
#include <queue>

#include "aggregates.hpp"
#include "memento.hpp"

class Scene
{
private:
    static Scene* self_instance;
    Scene();
    ~Scene()
    {
        delete this->self_instance;
        delete this->component_circle;
        delete this->component_triangle;  
        delete this->component_rectangle;  
        delete this->component_pentagon;  
    };
    std::mutex mtx_for_queue;
    std::queue<std::vector<std::string>> command_queue;

    bool b_exit;

    sf::RenderWindow window;
    std::vector<AbstractShape*> shapes;
    void draw_process();
    void update_state();
    void threaded_input();

    Shape *component_circle,
          *component_triangle,
          *component_rectangle,
          *component_pentagon;
public:
    Scene(Scene& other) = delete;
    void operator=(const Scene&) = delete;
    static Scene* getInstance();
    void loop();
    
    Memento* save_state() const;
    void restore_state(Memento* memento);
};
