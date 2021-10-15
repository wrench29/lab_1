#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class aggregate_component : public sf::Drawable
{
public:
    aggregate_component(sf::Shape& shape, std::string name) : drawable(&shape), name(name) {};
    ~aggregate_component() { delete drawable; };
    std::string get_name() const { return this->name; };
    sf::Shape* get_shape() const { return this->drawable; };
private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
protected:
    aggregate_component(std::string name) : name(name) {};
    const std::string name;
    sf::Shape* drawable;
};

class aggregate_collection : public aggregate_component
{
public:
    aggregate_collection(std::string name) : aggregate_component(name) {};
    ~aggregate_collection() { for (aggregate_component component : component_vector) delete &component; };
    void add(aggregate_component& component);
    int size() { return this->component_vector.size(); };
private:
    std::vector<aggregate_component> component_vector;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};