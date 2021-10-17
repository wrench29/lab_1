#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

class Aggregate : public sf::Transform, public sf::Drawable
{
public:
    virtual void move(float x, float y) = 0;
    virtual std::string get_name() const = 0;
    virtual bool is_collection() const = 0;
    virtual sf::Vector2f get_position() const = 0;
protected:
    float x, y;
};

class AggregateComponent : public Aggregate
{
public:
    AggregateComponent(sf::Shape& shape, std::string name) : drawable(&shape), name(name), local_x(0.f), local_y(0.f) {};
    virtual std::string get_name() const { return this->name; };
    sf::Shape* get_shape() const { return this->drawable; };
    virtual bool is_collection() const { return false; };
    virtual sf::Vector2f get_position() const { return sf::Vector2f(this->local_x, this->local_y); };
    virtual void move(float x, float y);
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    float local_x, local_y;
protected:
    const std::string name;
    sf::Shape* drawable;
};

class AggregateCollection : public Aggregate
{
public:
    AggregateCollection(std::string name) : name(name), x(0.f), y(0.f) {};
    void add(Aggregate* component);
    virtual std::string get_name() const { return this->name; };
    int size() { return this->aggregates.size(); };
    virtual bool is_collection() const { return true; };
    virtual sf::Vector2f get_position() const { return sf::Vector2f(this->x, this->y); };
    virtual void move(float x, float y);
    void move(float x, float y, std::string name);
    Aggregate* find(std::string name);
    Aggregate* find_all(std::string name);
    void remove(std::string name);
    void remove_inner(std::string name);
private:
    float x, y;
    const std::string name;
    std::vector<Aggregate*> aggregates;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};