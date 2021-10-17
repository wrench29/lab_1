#include <iostream>

#include <SFML/Graphics.hpp>

#include "aggregates.hpp"

void AggregateComponent::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform.translate(this->local_x, this->local_y);    
    target.draw(*drawable, states);
}

void AggregateComponent::move(float xarg, float yarg)
{
    this->local_x = xarg;
    this->local_y = yarg;
}
void AggregateCollection::add(Aggregate* component)
{
    this->aggregates.push_back(component);
    component->move(this->x + component->get_position().x, this->y + component->get_position().y);
}

void AggregateCollection::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    for (Aggregate* component : this->aggregates)
    {
        if (component->is_collection())
        {
            dynamic_cast<AggregateCollection*>(component)->draw(target, states);
        }
        else
        {
            states = states.Default;
            AggregateComponent* comp = dynamic_cast<AggregateComponent*>(component);
            sf::Vector2f local(this->x, this->y),
                         glob = comp->get_position();
            states.transform.translate(local.x + glob.x, local.y + glob.y);  
            target.draw(*comp->get_shape(), states);
        }
    }
}

void AggregateCollection::move(float xarg, float yarg)
{
    this->x = xarg;
    this->y = yarg;
    for (Aggregate* component : this->aggregates)
    {
        if (component->is_collection())
        {
            component->move(xarg, yarg);
        }
    }
}

Aggregate* AggregateCollection::find(std::string name)
{
    for (Aggregate* aggr : this->aggregates)
    {
        if (aggr->get_name() == name)
        {
            return aggr;
        }
    }
    std::cout << "Not found" << std::endl;
    return nullptr;
}

Aggregate* AggregateCollection::find_all(std::string name)
{
    for (Aggregate* aggr : this->aggregates)
    {
        if (aggr->get_name() == name)
        {
            return aggr;
        }
        if (aggr->is_collection())
        {
            AggregateCollection* collection = dynamic_cast<AggregateCollection*>(aggr);
            return collection->find_all(name);
        }
    }
    std::cout << "Not found" << std::endl;
    return nullptr;
}

void AggregateCollection::move(float x, float y, std::string name)
{
    Aggregate* aggr = this->find(name);
    if (aggr == nullptr)
    {
        return;
    }

    aggr->move(x, y);
}

void AggregateCollection::remove(std::string name)
{
    for (int i = 0; i < this->aggregates.size(); ++i)
    {
        Aggregate* aggr = this->aggregates[i];
        if (aggr->get_name() == name)
        {
            this->aggregates.erase(this->aggregates.begin() + i);
            return;
        }
    }
}

void AggregateCollection::remove_inner(std::string name)
{
    for (int i = 0; i < this->aggregates.size(); ++i)
    {
        Aggregate* aggr = this->aggregates[i];
        if (aggr->get_name() == name)
        {
            this->aggregates.erase(this->aggregates.begin() + i);
            return;
        }
        if (aggr->is_collection())
        {
            AggregateCollection* collection = dynamic_cast<AggregateCollection*>(aggr);
            collection->remove_inner(name);
        }
    }
}