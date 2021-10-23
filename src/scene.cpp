#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>

#include <SFML/System/Vector2.hpp>

#include "utils.hpp"
#include "aggregates.hpp"
#include "caretaker.hpp"

#include "scene.hpp"

Scene* Scene::self_instance = nullptr;

Scene::Scene() : b_exit(false) 
{
    component_circle = new Shape(Shapes::Circle, Colors::White, "");
    component_triangle = new Shape(Shapes::Triangle, Colors::White, "");
    component_rectangle = new Shape(Shapes::Rectangle, Colors::White, "");
    component_pentagon = new Shape(Shapes::Pentagon, Colors::White, "");

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
    this->mtx_for_queue.lock();
    if (this->command_queue.size() > 0)
    {
        std::vector<std::string> vec = command_queue.front();
        command_queue.pop();
        this->mtx_for_queue.unlock();
        std::string command = vec[0];
        if (command == "draw")
        {
            Shapes s_shape = string_to_shape(vec[1]);
            Colors s_color = string_to_color(vec[2]);
            Shape* comp;

            switch (s_shape)
            {
            case Shapes::Circle:
                comp = component_circle->clone(vec[3]);
                break;
            case Shapes::Triangle:
                comp = component_triangle->clone(vec[3]);
                break;
            case Shapes::Rectangle:
                comp = component_rectangle->clone(vec[3]);
                break;
            case Shapes::Pentagon:
                comp = component_pentagon->clone(vec[3]);
                break;
            }

            comp->set_color(s_color);
            
            this->shapes.push_back(comp);
        }
        else if (command == "move")
        {
            std::string name = vec[1];
            int x = std::stof(vec[2]),
                y = std::stof(vec[3]);
            
            for (AbstractShape* agr : this->shapes)
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
            Aggregate* collection = new Aggregate(name);
            for (int i = 0; i < lower_aggregates.size(); ++i)
            {
                for (int it = 0; it < this->shapes.size(); ++it)
                {
                    if (this->shapes[it]->get_name() == lower_aggregates[i])
                    {
                        collection->add(this->shapes[it]);
                        this->shapes.erase(this->shapes.begin() + it);
                        break;
                    }
                }
            }
            this->shapes.push_back(collection);
        }
        else if (command == "remove")
        {
            for (int it = 0; it < this->shapes.size(); ++it)
            {
                if (this->shapes[it]->get_name() == vec[1])
                {
                    delete this->shapes[it];
                    this->shapes.erase(this->shapes.begin() + it);
                    break;
                }
            }
        }
        else if (command == "save")
        {
            Caretaker::save_scene(vec[1]);
        }
        else if (command == "restore")
        {
            Caretaker::restore_scene(vec[1]);
        }
        else if (command == "exit")
        {
            this->window.close();
        }
    }
    else
    {
        this->mtx_for_queue.unlock();
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
        remove_doublespaces(command);
        trim(command);
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
            for (AbstractShape* aggr : this->shapes)
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
            for (AbstractShape* aggr : this->shapes)
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
                for (AbstractShape* aggr : this->shapes)
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
            for (AbstractShape* aggr : this->shapes)
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
        
        this->mtx_for_queue.lock();
        this->command_queue.push(vec);
        this->mtx_for_queue.unlock();
    }
}

// Inner function
std::vector<std::string> __recursive_state_aggregate(Aggregate* aggregate)
{
    std::vector<std::string> raw_memento;
    std::stringstream sstr;
    sstr << "begin " << aggregate->name << " " << aggregate->get_position().x << " "
         << aggregate->get_position().y;  
    raw_memento.push_back(sstr.str());
    for (AbstractShape* ab_shape : aggregate->aggregates)
    {
        std::stringstream line;
        Shape* p_shape = dynamic_cast<Shape*>(ab_shape);
        if (p_shape != nullptr)
        {
            line << p_shape->get_name() << " "
                 << shape_to_string(p_shape->get_shape_enum()) << " "
                 << color_to_string(p_shape->get_color_enum()) << " "
                 << p_shape->get_position().x << " "
                 << p_shape->get_position().y;
            raw_memento.push_back(line.str());
        }
        else
        {
            Aggregate* p_aggr = dynamic_cast<Aggregate*>(ab_shape);
            std::vector<std::string> _append = __recursive_state_aggregate(p_aggr);
            raw_memento.insert(raw_memento.end(), _append.begin(), _append.end());
        }
    }
    raw_memento.push_back("end");
    return raw_memento;
}

// Inner function
Aggregate* __recursive_state_raw_memento(std::vector<std::string> raw_memento, int &begin_index)
{
    Aggregate* aggregate = new Aggregate(split(raw_memento[begin_index++], " ")[1]);
    std::string line;
    for (; begin_index < raw_memento.size(); ++begin_index)
    {
        line = raw_memento[begin_index];
        std::vector<std::string> split_line = split(line, " ");
        if (split_line[0] == "begin")
        {
            Aggregate* new_aggregate = __recursive_state_raw_memento(raw_memento, begin_index);
            new_aggregate->move(
                std::stof(split_line[2]), 
                std::stof(split_line[3]));
            aggregate->add(new_aggregate);
        }
        else if (split_line[0] == "end")
        {
            break;
        }
        else
        {
            Shape* shape = new Shape(
                string_to_shape(split_line[1]),
                string_to_color(split_line[2]),
                split_line[0]);
            shape->move(
                std::stof(split_line[3]),
                std::stof(split_line[4]));
            aggregate->add(shape);

        }
    }
    return aggregate;
}

Memento* Scene::save_state() const
{
    std::vector<std::string> raw_memento;
    for (AbstractShape* ab_shape : this->shapes)
    {
        std::stringstream line;
        Shape* p_shape = dynamic_cast<Shape*>(ab_shape);
        if (p_shape != nullptr)
        {
            line << p_shape->get_name() << " "
                 << shape_to_string(p_shape->get_shape_enum()) << " "
                 << color_to_string(p_shape->get_color_enum()) << " "
                 << p_shape->get_position().x << " "
                 << p_shape->get_position().y;
            raw_memento.push_back(line.str());
        }
        else
        {
            Aggregate* p_aggr = dynamic_cast<Aggregate*>(ab_shape);
            std::vector<std::string> _append = __recursive_state_aggregate(p_aggr);
            raw_memento.insert(raw_memento.end(), _append.begin(), _append.end());
        }
    }
    SceneMemento* memento = new SceneMemento(raw_memento);
    return memento;
}
void Scene::restore_state(Memento* memento)
{
    for (int i = this->shapes.size() - 1; i >= 0; --i)
    {
        delete this->shapes[i];
    }
    this->shapes.clear();

    std::vector<std::string> raw_memento = memento->get_state();
    std::string line;
    for (int i = 0; i < raw_memento.size(); ++i)
    {
        line = raw_memento[i];
        std::vector<std::string> split_line = split(line, " ");
        if (split_line[0] == "begin")
        {
            Aggregate* aggregate = __recursive_state_raw_memento(raw_memento, i);
            aggregate->move(
                std::stof(split_line[2]),
                std::stof(split_line[3]));
            this->shapes.push_back(aggregate);
        }
        else
        {
            Shape* shape = new Shape(
                string_to_shape(split_line[1]),
                string_to_color(split_line[2]),
                split_line[0]);
            shape->move(
                std::stof(split_line[3]),
                std::stof(split_line[4]));
            this->shapes.push_back(shape);
        }
    }
}
