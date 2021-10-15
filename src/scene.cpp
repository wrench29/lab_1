#include <iostream>
#include <thread>

#include <SFML/System/Vector2.hpp>

#include "utils.hpp"
#include "custom_shape.hpp"

#include "scene.hpp"

Scene* Scene::self_instance = nullptr;

void Scene::initialize()
{
    this->window.create(sf::VideoMode(1280, 720), "My window");
    this->window.setVerticalSyncEnabled(true);
}

void Scene::loop()
{
    std::thread console_thread(&Scene::threaded_input, this);

    while (this->window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::Resized)
            {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }

        window.clear(sf::Color(200, 230, 255));

        this->update_state();
        this->draw_process();

        window.display();
    }
    this->b_exit = true;
    console_thread.join();
}

Scene* Scene::getInstance()
{
    if (self_instance == nullptr)
    {
        self_instance = new Scene();
    }
    return self_instance;
}

void Scene::update_state()
{
    this->mtx.lock();
    if (this->command_queue.size() > 0)
    {
        std::vector<std::string> vec = command_queue.front();
        command_queue.pop();
        this->mtx.unlock();
        std::string command = vec[0];
        if (command == "draw")
        {
            std::string s_shape = vec[1],
                        s_color = vec[2];
            sf::Shape *shape;
            sf::Color *color;

            if (s_shape == "circle")
            {
                shape = new sf::CircleShape(40);
            }
            else if (s_shape == "triangle")
            {
                shape = new sf::CircleShape(40, 3);
            }
            else if (s_shape == "rectangle")
            {
                shape = new sf::RectangleShape(sf::Vector2f(50, 50));
            }
            else if (s_shape == "pentagon")
            {
                shape = new sf::CircleShape(40, 5);
            }

            if (s_color == "red")
            {
                color = new sf::Color(255, 0, 0);
            }
            else if (s_color == "green")
            {
                color = new sf::Color(0, 255, 0);
            }
            else if (s_color == "blue")
            {
                color = new sf::Color(0, 0, 255);
            }
            else if (s_color == "white")
            {
                color = new sf::Color(255, 255, 255);
            }
            else if (s_color == "black")
            {
                color = new sf::Color(0, 0, 0);
            }
            shape->setFillColor(*color);
            this->shapes.push_back(*shape);
            //delete color;
            //delete shape;
        }
        else if (command == "exit")
        {
            this->window.close();
        }
    }
    else
    {
        this->mtx.unlock();
    }
}

void Scene::draw_process()
{
    for (custom_shape shape : this->shapes)
    {
        this->window.draw(shape);
    }
}
void Scene::threaded_input()
{
    std::string command("");
    std::vector<std::string> vec;
    vec.push_back("");
    while (vec[0] != "exit")
    {
        std::cout << "# ";
        std::getline(std::cin, command);
        if (this->b_exit)
        {
            std::cout << "Shutting down." << std::endl;
            break;
        }
        trim(command);
        remove_doublespaces(command);
        to_lower(command);
        vec = split(command, " ");
        std::vector<int> command_vector;
        if (vec[0] != "draw" && vec[0] != "exit")
        {
            std::cout << "Unknown command." << std::endl;
            continue;
        }
        if (vec[0] == "draw")
        {
            if (vec.size() < 3)
            {
                std::cout << "Using: draw <shape> <color: string(red, green, blue, white, black)>" << std::endl;
                continue;
            }
            if (vec[1] != "circle")
            {
                std::cout << "Unknown shape." << std::endl;
                continue;
            }
            if (vec[2] != "red" && vec[2] != "green" && vec[2] != "blue" && vec[2] != "white" && vec[2] != "black")
            {
                std::cout << "Unknown color." << std::endl;
                continue;
            }
        }
        
        this->mtx.lock();
        this->command_queue.push(vec);
        this->mtx.unlock();
    }
}