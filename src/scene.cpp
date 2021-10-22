#include <iostream>
#include <thread>
#include <fstream>

#include <SFML/System/Vector2.hpp>

#include "utils.hpp"
#include "aggregates.hpp"

#include "scene.hpp"

Scene* Scene::self_instance = nullptr;

Scene::Scene() : b_exit(false) 
{
    component_circle = new AggregateComponent(Shapes::Circle, Colors::White, "");
    component_triangle = new AggregateComponent(Shapes::Triangle, Colors::White, "");
    component_rectangle = new AggregateComponent(Shapes::Rectangle, Colors::White, "");
    component_pentagon = new AggregateComponent(Shapes::Pentagon, Colors::White, "");

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
    this->mtx_queue.lock();
    if (this->command_queue.size() > 0)
    {
        std::vector<std::string> vec = command_queue.front();
        command_queue.pop();
        this->mtx_queue.unlock();
        std::string command = vec[0];
        if (command == "draw")
        {
            std::string s_shape = vec[1],
                        s_color = vec[2];
            AggregateComponent* comp;

            if (s_shape == "circle")
            {
                comp = component_circle->clone(vec[3]);
            }
            else if (s_shape == "triangle")
            {
                comp = component_triangle->clone(vec[3]);
            }
            else if (s_shape == "rectangle")
            {
                comp = component_rectangle->clone(vec[3]);
            }
            else if (s_shape == "pentagon")
            {
                comp = component_pentagon->clone(vec[3]);
            }

            if (s_color == "red")
            {
                comp->set_color(Colors::Red);
            }
            else if (s_color == "green")
            {
                comp->set_color(Colors::Green);;
            }
            else if (s_color == "blue")
            {
                comp->set_color(Colors::Blue);
            }
            else if (s_color == "white")
            {
                comp->set_color(Colors::White);
            }
            else if (s_color == "black")
            {
                comp->set_color(Colors::Black);
            }
            
            this->shapes.push_back(comp);
        }
        else if (command == "move")
        {
            std::string name = vec[1];
            int x = std::stof(vec[2]),
                y = std::stof(vec[3]);
            
            for (Aggregate* agr : this->shapes)
            {
                if (agr->get_name() == name)
                {
                    agr->move(x, y);
                    break;
                }
            }
        }
        else if (command == "aggregate")
        {
            std::string name = vec[1];
            std::vector<std::string> lower_aggregates(vec.begin() + 2, vec.end());
            AggregateCollection* collection = new AggregateCollection(name);
            std::vector<int> indices;
            for (int i = 0; i < lower_aggregates.size(); ++i)
            {
                for (int it = 0; it < this->shapes.size(); ++it)
                {
                    if (this->shapes[it]->get_name() == lower_aggregates[i])
                    {
                        collection->add(this->shapes[it]);
                        indices.push_back(it);
                        break;
                    }
                }
            }
            for (int i = (indices.size() - 1); i >= 0; --i)
            {
                this->shapes.erase(this->shapes.begin() + indices[i]);
            }
            this->shapes.push_back(collection);
        }
        else if (command == "remove")
        {
            for (int it = 0; it < this->shapes.size(); ++it)
            {
                if (this->shapes[it]->get_name() == vec[1])
                {
                    this->shapes.erase(this->shapes.begin() + it);
                    break;
                }
            }
        }
        else if (command == "save")
        {
            Caretaker::save_state_to_file(this->shapes, vec[1]);
        }
        else if (command == "restore")
        {
            this->shapes.clear();
            this->shapes = Caretaker::restore_state_from_file(vec[1]);
        }
        else if (command == "exit")
        {
            this->window.close();
        }
    }
    else
    {
        this->mtx_queue.unlock();
    }
}

void Scene::draw_process()
{
    for (int i = 0; i < this->shapes.size(); ++i)
    {
        this->window.draw(*dynamic_cast<sf::Drawable*>(this->shapes[i]));
    }
}
void Scene::threaded_input()
{
    std::string command("");
    std::vector<std::string> vec;
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
        if (vec[0] != "draw" && vec[0] != "exit" && vec[0] != "move" && vec[0] != "aggregate" && vec[0] != "remove" && vec[0] != "save" && vec[0] != "restore")
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
            for (Aggregate* aggr : this->shapes)
            {
                if (aggr->get_name() == vec[1])
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
        else if (vec[0] == "aggregate")
        {
            if (vec.size() < 4)
            {
                std::cout << "Using: aggregate <new name> <aggregate 1> <aggregate 2> ..." << std::endl;
                continue;
            }
            
            bool collision = false;
            for (Aggregate* aggr : this->shapes)
            {
                if (aggr->get_name() == vec[1])
                {
                    collision = true;
                    std::cout << "Name " << vec[1] << " already in use." << std::endl;
                    break;
                }
            }
            if (collision)
            {
                continue;
            }

            bool found = false;
            for (int i = 2; i < vec.size(); ++i)
            {
                found = false;
                for (Aggregate* aggr : this->shapes)
                {
                    if (aggr->get_name() == vec[i])
                    {
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    std::cout << "Name " << vec[i] << " not found." << std::endl;
                    break;
                }
            }
            if (!found)
            {
                continue;
            }
        }
        else if (vec[0] == "remove")
        {
            if (vec.size() != 2)
            {
                std::cout << "Using: remove <aggregate>" << std::endl;
                continue;
            }

            bool found = false;
            for (Aggregate* aggr : this->shapes)
            {
                if (aggr->get_name() == vec[1])
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                std::cout << "Name " << vec[1] << " not found." << std::endl;
                continue;
            }
        }
        else if (vec[0] == "save")
        {
            if (vec.size() != 2)
            {
                std::cout << "Using: save <savename>" << std::endl;
                continue;
            }
            if (file_exists(vec[1]))
            {
                std::cout << "Save " << vec[1] << " already exists." << std::endl;
                continue; 
            }
        }
        else if (vec[0] == "restore")
        {
            if (vec.size() != 2)
            {
                std::cout << "Using: restore <savename>" << std::endl;
                continue;
            }
            if (!file_exists(vec[1]))
            {
                std::cout << "Save " << vec[1] << " doesn't exists." << std::endl;
                continue; 
            }
        }
        
        this->mtx_queue.lock();
        this->command_queue.push(vec);
        this->mtx_queue.unlock();
    }
}
