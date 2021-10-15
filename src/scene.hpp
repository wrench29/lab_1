#pragma once

#include <SFML/Graphics.hpp>
#include <mutex>
#include <queue>

#include "custom_shape.hpp"

class Scene
{
private:
    static Scene* self_instance;
    Scene() : b_exit(false) {};
    
    std::mutex mtx;
    std::queue<std::vector<std::string>> command_queue;

    bool b_exit;

    sf::RenderWindow window;
    std::vector<custom_shape> shapes;
    void draw_process();
    void update_state();
    void threaded_input();
public:
    Scene(Scene& other) = delete;
    void operator=(const Scene&) = delete;
    static Scene* getInstance();
    void initialize();
    void loop();
};