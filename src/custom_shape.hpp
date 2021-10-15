#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics.hpp>

class custom_shape : public sf::Drawable
{
private:
    sf::Shape* shape;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
public:
    custom_shape(sf::Shape &shape) : shape(&shape) {};
    ~custom_shape();
};