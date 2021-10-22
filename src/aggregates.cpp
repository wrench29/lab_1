#include <iostream>
#include <fstream>

#include <SFML/Graphics.hpp>

#include "aggregates.hpp"

////////////////////////
//
// Aggregate Component
//
////////////////////////

AggregateComponent::AggregateComponent(Shapes shape, Colors color, std::string name) : name(name), local_x(0.f), local_y(0.f) {
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

AggregateComponent::~AggregateComponent()
{
    delete this->drawable;
}

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

void AggregateComponent::set_color(Colors color)
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

AggregateComponent* AggregateComponent::clone(std::string name)  const
{
    AggregateComponent* component = new AggregateComponent(this->shape, this->color, name);
    return component;
}

////////////////////////
//
// Aggregate Collection
//
////////////////////////

AggregateCollection::~AggregateCollection()
{
    for (Aggregate* aggr : this->aggregates)
    {
        if (aggr->is_collection())
        {
            AggregateCollection* coll = dynamic_cast<AggregateCollection*>(aggr);
            delete coll;
        }
        else
        {
            AggregateComponent* comp = dynamic_cast<AggregateComponent*>(aggr);
            delete comp;
        }
    }
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

MementoList* Caretaker::__recursive_memento_list(AggregateCollection* coll)
{
    MementoList* mem_list = new MementoList(coll->get_name(), coll->get_position().x, coll->get_position().y);
    for (Aggregate* aggr : coll->aggregates)
    {
        if (aggr->is_collection())
        {
            AggregateCollection* aggr_coll = dynamic_cast<AggregateCollection*>(aggr);
            mem_list->vec.push_back(__recursive_memento_list(aggr_coll));
        }
        else
        {
            AggregateComponent* aggr_comp = dynamic_cast<AggregateComponent*>(aggr);
            MementoComponent* comp = new MementoComponent(aggr_comp->get_name(), 
                aggr_comp->get_shape_enum(), aggr_comp->get_color_enum(), 
                aggr_comp->get_coords().x, aggr_comp->get_coords().y);
            mem_list->vec.push_back(comp);
        }
    }
    return mem_list;
}
std::vector<Memento*> Caretaker::save_state(std::vector<Aggregate*> vec)
{
    std::vector<Memento*> new_vec;
    for (Aggregate* aggr : vec)
    {
        if (aggr->is_collection())
        {
            AggregateCollection* aggr_coll = dynamic_cast<AggregateCollection*>(aggr);
            new_vec.push_back(__recursive_memento_list(aggr_coll));
        }
        else
        {
            AggregateComponent* aggr_comp = dynamic_cast<AggregateComponent*>(aggr);
            MementoComponent* comp = new MementoComponent(aggr_comp->get_name(), 
                aggr_comp->get_shape_enum(), aggr_comp->get_color_enum(), 
                aggr_comp->get_coords().x, aggr_comp->get_coords().y);
            new_vec.push_back(comp);
        }
    }
    return new_vec;
}

AggregateCollection* Caretaker::__recursive_aggregate_list(MementoList* coll)
{
    AggregateCollection* aggr_coll = new AggregateCollection(coll->get_name());
    aggr_coll->move(coll->get_coords().x, coll->get_coords().y);
    for (Memento* mem : coll->vec)
    {
        if (mem->is_collection())
        {
            MementoList* mem_coll = dynamic_cast<MementoList*>(mem);
            aggr_coll->aggregates.push_back(__recursive_aggregate_list(mem_coll));
        }
        else
        {
            MementoComponent* mem_comp = dynamic_cast<MementoComponent*>(mem);
            AggregateComponent* comp = new AggregateComponent(mem_comp->get_shape(), mem_comp->get_color(), mem_comp->get_name());
            comp->move(mem_comp->get_coords().x, mem_comp->get_coords().y);
            aggr_coll->aggregates.push_back(comp);
        }
    }
    return aggr_coll;
}
std::vector<Aggregate*> Caretaker::restore_state(std::vector<Memento*> list)
{
    std::vector<Aggregate*> new_vec;
    for (Memento* memt : list)
    {
        if (memt->is_collection())
        {
            MementoList* mem_list = dynamic_cast<MementoList*>(memt);
            new_vec.push_back(__recursive_aggregate_list(mem_list));
        }
        else
        {
            MementoComponent* mem_comp = dynamic_cast<MementoComponent*>(memt);
            AggregateComponent* comp = new AggregateComponent(mem_comp->get_shape(), mem_comp->get_color(), mem_comp->get_name());
            comp->move(mem_comp->get_coords().x, mem_comp->get_coords().y);
            new_vec.push_back(comp);
        }
    }
    return new_vec;
}

void Caretaker::save_state_to_file(std::vector<Aggregate*> vec, std::string name)
{
    std::ofstream file(name);
    if (!file.is_open())
    {
        std::cout << "Error with opening file." << std::endl;
        return;
    }

    std::vector<Memento*> mem_vec = save_state(vec);

    for (Memento* mem : mem_vec)
    {
        if (mem->is_collection())
        {
            MementoList* mem_list = dynamic_cast<MementoList*>(mem);
            __recursive_save_to_file(file, mem_list);
        }
        else
        {
            MementoComponent* mem_comp = dynamic_cast<MementoComponent*>(mem);
            file << mem_comp->get_name() << " " << mem_comp->get_shape_str() << " " << mem_comp->get_color_str() << " " << mem_comp->get_coords().x << " " << mem_comp->get_coords().y << "\n";
        }
    }
    file.close();
}
std::vector<Aggregate*> Caretaker::restore_state_from_file(std::string name)
{
    std::ifstream file(name);
    if (!file.is_open())
    {
        std::cout << "Error with opening file." << std::endl;
        return std::vector<Aggregate*>();
    }

    std::vector<Memento*> mem_list;

    std::string line;
    while (std::getline(file, line))
    {
        std::vector<std::string> vec = split(line, " ");
        if (vec[0] == "begin")
        {
            MementoList* coll = new MementoList(vec[1], std::stof(vec[2]), std::stof(vec[3]));
            __recursive_restore_from_file(file, coll);
            mem_list.push_back(coll);
        }
        else
        {
            MementoComponent* el = new MementoComponent(vec[0], string_to_shape(vec[1]), string_to_color(vec[2]), std::stof(vec[3]), std::stof(vec[4]));
            mem_list.push_back(el);
        }
    }
    file.close();

    return restore_state(mem_list);
}
void Caretaker::__recursive_save_to_file(std::ofstream& file, MementoList* list)
{
    file << "begin " << list->get_name() << " " << list->get_coords().x << " " << list->get_coords().y << "\n"; // here's null pointer
    for (Memento* mem : list->vec)
    {
        if (mem->is_collection())
        {
            MementoList* mem_list = dynamic_cast<MementoList*>(mem);
            __recursive_save_to_file(file, mem_list);
        }
        else
        {
            MementoComponent* mem_comp = dynamic_cast<MementoComponent*>(mem);
            file << mem_comp->get_name() << " " << mem_comp->get_shape_str() << " " << mem_comp->get_color_str() << " " << mem_comp->get_coords().x << " " << mem_comp->get_coords().y << "\n";
        }
    }
    file << "end\n";
}
void Caretaker::__recursive_restore_from_file(std::ifstream &file, MementoList* list)
{
    std::string line;
    while (std::getline(file, line) && line != "end")
    {
        std::vector<std::string> vec = split(line, " ");
        if (vec[0] == "begin")
        {
            MementoList* coll = new MementoList(vec[1], std::stof(vec[2]), std::stof(vec[3]));
            __recursive_restore_from_file(file, coll);
            list->vec.push_back(coll);
        }
        else
        {
            MementoComponent* el = new MementoComponent(vec[0], string_to_shape(vec[1]), string_to_color(vec[2]), std::stof(vec[3]), std::stof(vec[4]));
            list->vec.push_back(el);
        }
    }
}
