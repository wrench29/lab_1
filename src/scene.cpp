#include <iostream>
#include <thread>

#include <SFML/System/Vector2.hpp>

#include "utils.hpp"
#include "aggregates.hpp"

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

        this->draw_process();

        window.display();
        this->update_state();
    }
    this->b_exit = true;
    if (console_thread.joinable())
    {
        console_thread.join();
    }
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
            aggregate_component* comp = new aggregate_component(*shape, vec[3]);
            this->shapes.push_back(comp);
            delete color;
        }
        else if (command == "move")
        {
            std::string name = vec[1];
            int x = std::stof(vec[2]),
                y = std::stof(vec[3]);
            
            for (aggregate_component* component : this->shapes)
            {
                std::string str = component->get_name();
                if (str == name)
                {
                    component->move(x, y);
                }
            }
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
    for (aggregate_component* shape : this->shapes)
    {
        this->window.draw(*shape);
    }
}
void Scene::threaded_input()
{
    std::string command("");
    std::vector<std::string> vec;
    std::vector<std::string> aggregate_names;
    std::string current_command("");
    while (current_command != "exit")
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
        if (vec[0] != "draw" && vec[0] != "exit" && vec[0] != "move")
        {
            std::cout << "Unknown command." << std::endl;
            continue;
        }
        current_command = vec[0];
        if (vec[0] == "draw")
        {
            if (vec.size() < 4)
            {
                std::cout << "Using: draw <shape> <color> <aggregate name>" << std::endl;
                continue;
            }
            if (vec[1] != "circle" && vec[1] != "triangle" && vec[1] != "rectangle" && vec[1] != "pentagon")
            {
                std::cout << "Unknown shape." << std::endl;
                continue;
            }
            if (vec[2] != "red" && vec[2] != "green" && vec[2] != "blue" && vec[2] != "white" && vec[2] != "black")
            {
                std::cout << "Unknown color." << std::endl;
                continue;
            }
            if (vec[3].size() < 3 && vec[3].size() > 9)
            {
                std::cout << "Aggregate name length must be between 3 and 9." << std::endl;
                continue;
            }
            aggregate_names.push_back(vec[3]);
        }
        else if (vec[0] == "move")
        {
            if (vec.size() < 4)
            {
                std::cout << "Using: move <aggregate name> <X> <Y>" << std::endl;
                continue;
            }
            try {
                std::stof(vec[2]);
                std::stof(vec[3]);
            } catch (std::invalid_argument e) {
                std::cout << "<X> and <Y> must be integer" << std::endl;
            }
            bool found = false;
            for (std::string str : aggregate_names)
            {
                if (str == vec[1])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                std::cout << "Not found aggregate with this name." << std::endl;
                continue;
            }
        }
        
        this->mtx.lock();
        this->command_queue.push(vec);
        this->mtx.unlock();
    }
}