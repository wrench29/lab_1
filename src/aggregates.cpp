#include <iostream>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "aggregates.hpp"

////////////////////////
//
// Shape
//
////////////////////////

Shape::Shape(Shapes shape, Colors color, std::string name) : name(name), local_pos(0.f, 0.f) {
    sf::Shape* p_shape;
    this->shape = shape;
    switch (shape)
    {
    case Shapes::Circle:
        p_shape = new sf::CircleShape(40);
        break;
    case Shapes::Triangle:
        p_shape = new sf::CircleShape(40, 3);
        break;
    case Shapes::Rectangle:
        p_shape = new sf::RectangleShape(sf::Vector2f(80.f, 80.f));
        break;
    case Shapes::Pentagon:
        p_shape = new sf::CircleShape(40, 5);
        break;
    }
    this->drawable = p_shape;
    this->set_color(color);
}

Shape::~Shape()
{
    delete this->drawable;
}

void Shape::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform.translate(this->local_pos.x, this->local_pos.y);    
    target.draw(*drawable, states);
}

void Shape::move(float xarg, float yarg)
{
    this->local_pos.x = xarg;
    this->local_pos.y = yarg;
}

void Shape::set_color(Colors color)
{
    this->color = color;
    switch (color)
    {
    case Colors::Red:
        this->drawable->setFillColor(sf::Color(255, 0, 0));
        break;
    case Colors::Green:
        this->drawable->setFillColor(sf::Color(0, 255, 0));
        break;
    case Colors::Blue:
        this->drawable->setFillColor(sf::Color(0, 0, 255));
        break;
    case Colors::White:
        this->drawable->setFillColor(sf::Color(255, 255, 255));
        break;
    case Colors::Black:
        this->drawable->setFillColor(sf::Color(0, 0, 0));
        break;
    }
}

Shape* Shape::clone(std::string name)  const
{
    Shape* component = new Shape(this->shape, this->color, name);
    return component;
}

////////////////////////
//
// Aggregate 
//
////////////////////////

Aggregate::~Aggregate()
{
    for (AbstractShape* aggr : this->aggregates)
    {
        Aggregate* collection = dynamic_cast<Aggregate*>(aggr);
        if (collection != nullptr)
        {
            delete collection;
        }
        else
        {
            Shape* comp = dynamic_cast<Shape*>(aggr);
            delete comp;
        }
    }
}

void Aggregate::add(AbstractShape* component)
{
    this->aggregates.push_back(component);
    component->move(this->pos.x + component->get_position().x, this->pos.y + component->get_position().y);
}

void Aggregate::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (AbstractShape* component : this->aggregates)
    {
        Aggregate* collection = dynamic_cast<Aggregate*>(component);
        if (collection != nullptr)
        {
            collection->draw(target, states);
        }
        else
        {
            states = states.Default;
            Shape* comp = dynamic_cast<Shape*>(component);
            states.transform.translate(
                comp->get_position().x + this->pos.x,
                comp->get_position().y + this->pos.y);  
            target.draw(*comp->get_shape(), states);
        }
    }
}

void Aggregate::move(float xarg, float yarg)
{
    this->local_pos.x = xarg;
    this->local_pos.y = yarg;
    this->pos.x = xarg;
    this->pos.y = yarg;
    for (AbstractShape* ab_shape : this->aggregates)
    {
        Aggregate* collection = dynamic_cast<Aggregate*>(ab_shape);
        if (collection != nullptr)
        {
            collection->global_move(xarg, yarg);
        }
    }
}

void Aggregate::global_move(float x, float y)
{
    this->pos.x = this->local_pos.x + x;
    this->pos.y = this->local_pos.y + y;
    for (AbstractShape* ab_shape : this->aggregates)
    {
        Aggregate* collection = dynamic_cast<Aggregate*>(ab_shape);
        if (collection != nullptr)
        {
            collection->global_move(this->pos.x, this->pos.y);
        }
    }
}

void Aggregate::remove(std::string name)
{
    for (int i = 0; i < this->aggregates.size(); ++i)
    {
        AbstractShape* aggr = this->aggregates[i];
        if (aggr->get_name() == name)
        {
            this->aggregates.erase(this->aggregates.begin() + i);
            return;
        }
    }
}

void Aggregate::remove_inner(std::string name)
{
    for (int i = 0; i < this->aggregates.size(); ++i)
    {
        AbstractShape* aggr = this->aggregates[i];
        if (aggr->get_name() == name)
        {
            this->aggregates.erase(this->aggregates.begin() + i);
            return;
        }
        Aggregate* collection = dynamic_cast<Aggregate*>(aggr);
        if (collection != nullptr)
        {
            collection->remove_inner(name);
        }
    }
}
