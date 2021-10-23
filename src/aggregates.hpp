#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

#include "utils.hpp"

class Scene;

class AbstractShape : public sf::Transform, public sf::Drawable
{
public:
    virtual void move(float x, float y) = 0;
    virtual std::string get_name() const = 0;
    virtual sf::Vector2f get_position() const = 0;
};

class Shape : public AbstractShape
{
public:
    Shape(Shapes shape, Colors color, std::string name);
    ~Shape();

    virtual void move(float x, float y);
    void set_color(Colors color);

    Shape* clone(std::string name) const;
    virtual std::string get_name() const 
    { 
        return this->name; 
    };
    sf::Shape* get_shape() const 
    { 
        return this->drawable; 
    };
    Shapes get_shape_enum() const 
    { 
        return this->shape; 
    };
    Colors get_color_enum() const 
    { 
        return this->color; 
    };
    virtual sf::Vector2f get_position() const 
    { 
        return local_pos;
    };
private:
    Shapes shape;
    Colors color;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    sf::Vector2f local_pos;
    bool initialized;
    const std::string name;
    sf::Shape* drawable;
};

class Aggregate : public AbstractShape
{
public:
    Aggregate(std::string name) : name(name), pos(0.f, 0.f), local_pos(0.f, 0.f) {};
    ~Aggregate();

    void add(AbstractShape* component);
    virtual void move(float x, float y);
    void remove(std::string name);
    void remove_inner(std::string name);

    virtual sf::Vector2f get_position() const 
    { 
        return pos; 
    };
    sf::Vector2f get_local_position() const
    {
        return local_pos;
    };
    virtual std::string get_name() const 
    { 
        return this->name;
    };
    int size() 
    { 
        return this->aggregates.size(); 
    };
private:
    void global_move(float x, float y);
    sf::Vector2f pos,
                 local_pos;
    const std::string name;
    std::vector<AbstractShape*> aggregates;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
friend class Caretaker;
friend std::vector<std::string> __recursive_state_aggregate(Aggregate* aggregate);
};
