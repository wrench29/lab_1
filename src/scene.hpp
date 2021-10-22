#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>
#include <queue>

#include "aggregates.hpp"

class Scene
{
private:
    static Scene* self_instance;
    Scene();
    std::mutex mtx_queue;
    std::queue<std::vector<std::string>> command_queue;

    bool b_exit;

    sf::RenderWindow window;
    std::vector<Aggregate*> shapes;
    void draw_process();
    void update_state();
    void threaded_input();

    AggregateComponent *component_circle,
                       *component_triangle,
                       *component_rectangle,
                       *component_pentagon;
public:
    Scene(Scene& other) = delete;
    void operator=(const Scene&) = delete;
    static Scene* getInstance();
    void loop();
};