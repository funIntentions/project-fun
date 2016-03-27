//
// Created by Dan on 3/26/2016.
//

#ifndef PARTIALORDERPLANNER_ATTRIBUTECOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_ATTRIBUTECOMPONENTMANAGER_H


#include "ComponentManager.h"
#include "ActionManager.h"
#include <memory>
#include <string>

class AttributeComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<float> health;
    };

    InstanceData _data;

public:

    AttributeComponentManager() : ComponentManager()
    {
        _data.size = 0;
    }

    void spawnComponent(Entity entity, float health)
    {
        _map.emplace(entity.index(), _data.size);

        _data.entity.push_back(entity);
        _data.health.push_back(health);

        ++_data.size;
    }

    float getHealth(Entity entity)
    {
        Instance instance = lookup(entity);
        return _data.health[instance.i];
    }

    void setHealth(Entity entity, float value)
    {
        Instance instance = lookup(entity);
        if (instance.i >= 0)
            _data.health[instance.i] = value;
    }

    // TODO: Remove awful fuzzy state conversion
    std::string getHealthState(Entity entity)
    {
        Instance instance = lookup(entity);
        if  (_data.health[instance.i] <= 0)
            return "Dead";
        else
            return "Alive";
    }

    // TODO: Remove awful fuzzy state conversion
    void setHealthState(Entity entity, std::string state)
    {
        Instance instance = lookup(entity);
        if (instance.i >= 0)
        {
            if (state == "Alive")
                _data.health[instance.i] = 100;
            else
                _data.health[instance.i] = 0;
        }
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.health[i] = _data.health[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.health.pop_back();

        --_data.size;
    }
};


#endif //PARTIALORDERPLANNER_ATTRIBUTECOMPONENTMANAGER_H
