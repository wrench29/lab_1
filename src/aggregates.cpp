#include "aggregates.hpp"

void aggregate_component::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    target.draw(*drawable, states);
}

void aggregate_collection::add(aggregate_component& component)
{
    this->component_vector.push_back(component);
}

void aggregate_collection::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (aggregate_component component : this->component_vector)
    {
        target.draw(*component.get_shape(), states);
    }
}
