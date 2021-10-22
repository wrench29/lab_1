#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <string>

#include "utils.hpp"


class Aggregate : public sf::Transform, public sf::Drawable
{
public:
    virtual void move(float x, float y) = 0;
    virtual std::string get_name() const = 0;
    virtual bool is_collection() const = 0;
    virtual sf::Vector2f get_position() const = 0;
};

class AggregateComponent : public Aggregate
{
public:
    AggregateComponent(Shapes shape, Colors color, std::string name);
    ~AggregateComponent();

    virtual void move(float x, float y);
    void set_color(Colors color);

    AggregateComponent* clone(std::string name) const;
    virtual std::string get_name() const { return this->name; };
    sf::Shape* get_shape() const { return this->drawable; };
    Shapes get_shape_enum() const { return this->shape; };
    Colors get_color_enum() const { return this->color; };
    sf::Vector2f get_coords() const { return sf::Vector2f(local_x, local_y); };
    virtual bool is_collection() const { return false; };
    virtual sf::Vector2f get_position() const { return sf::Vector2f(this->local_x, this->local_y); };
private:
    Shapes shape;
    Colors color;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    float local_x, local_y;
    bool initialized;
    const std::string name;
    sf::Shape* drawable;
};

class AggregateCollection : public Aggregate
{
public:
    AggregateCollection(std::string name) : name(name), x(0.f), y(0.f) {};
    ~AggregateCollection();

    void add(Aggregate* component);
    virtual void move(float x, float y);
    void remove(std::string name);
    void remove_inner(std::string name);

    virtual sf::Vector2f get_position() const { return sf::Vector2f(this->x, this->y); };
    virtual std::string get_name() const { return this->name; };
    int size() { return this->aggregates.size(); };
    virtual bool is_collection() const { return true; };
    
private:
    Aggregate* find(std::string name);
    Aggregate* find_all(std::string name);
    float x, y;
    const std::string name;
    std::vector<Aggregate*> aggregates;
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
friend class Caretaker;
};

class Memento
{
public:
    virtual std::string get_name() const = 0;
    virtual sf::Vector2f get_coords() const = 0;
    virtual bool is_collection() const = 0;
};

class MementoComponent : public Memento
{
public:
    MementoComponent(std::string name, Shapes shape, Colors color, float x, float y) :
        name(name), shape(shape), color(color), local_x(x), local_y(y) {};
    virtual std::string get_name() const { return this->name; };
    Shapes get_shape() const { return this->shape; };
    Colors get_color() const { return this->color; };
    virtual sf::Vector2f get_coords() const { return sf::Vector2f(this->local_x, this->local_y); };
    std::string get_shape_str() const { return shape_to_string(this->shape); };
    std::string get_color_str() const { return color_to_string(this->color); };
    virtual bool is_collection() const { return false; };
private:
    const std::string name;
    const Shapes shape;
    const Colors color;
    const float local_x, local_y;
};
class MementoList : public Memento
{
public:
    virtual std::string get_name() const { return this->name; };
    virtual sf::Vector2f get_coords() const { return sf::Vector2f(x, y); };
    virtual bool is_collection() const { return true; };
private:
    MementoList(std::string name, float x, float y) : name(name), x(x), y(y) {};
    std::string name;
    float x, y;
    std::vector<Memento*> vec;
friend class Caretaker;
};

class Caretaker
{
private:
    Caretaker() = delete;
    static MementoList* __recursive_memento_list(AggregateCollection* coll);
    static AggregateCollection* __recursive_aggregate_list(MementoList* coll);
    static std::vector<Memento*> save_state(std::vector<Aggregate*> vec);
    static std::vector<Aggregate*> restore_state(std::vector<Memento*> list);
    static void __recursive_save_to_file(std::ofstream& file, MementoList* list);
    static void __recursive_restore_from_file(std::ifstream &file, MementoList* list);
public:
    Caretaker(Caretaker& other) = delete;
    void operator=(const Caretaker&) = delete;
    static void save_state_to_file(std::vector<Aggregate*> vec, std::string name);
    static std::vector<Aggregate*> restore_state_from_file(std::string name);
};