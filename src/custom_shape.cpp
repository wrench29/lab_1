#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics.hpp>

#include "custom_shape.hpp"

void custom_shape::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*shape, states);
}

custom_shape::~custom_shape()
{
    //delete shape;
}