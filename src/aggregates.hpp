#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class aggregate_component : public sf::Transform, 
                            public sf::Drawable
{
public:
    aggregate_component(sf::Shape& shape, std::string name) : drawable(&shape), name(name), x(0.f), y(0.f) {};
    std::string get_name() const { return this->name; };
    sf::Shape* get_shape() const { return this->drawable; };
    void move(float x, float y);
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    float x, y;
protected:
    aggregate_component(std::string name) : name(name), x(0), y(0) {};
    const std::string name;
    sf::Shape* drawable;
};

class aggregate_collection : public aggregate_component
{
public:
    aggregate_collection(std::string name) : aggregate_component(name) {};
    void add(aggregate_component& component);
    int size() { return this->component_vector.size(); };
private:
    std::vector<aggregate_component> component_vector;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};