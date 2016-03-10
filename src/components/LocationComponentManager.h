//
// Created by Dan on 2/22/2016.
//

#ifndef PARTIALORDERPLANNER_LOCATIONCOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_LOCATIONCOMPONENTMANAGER_H

#include "ComponentManager.h"
#include "ActionManager.h"
#include <memory>
#include <string>

class LocationComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<std::string> location;
        std::vector<std::vector<Entity>> locations;

    };

    InstanceData _data;

public:

    LocationComponentManager() : ComponentManager()
    {
        _data.size = 0;
    }

    std::vector<Operator> determineActionsOfEntity(Action travelTemplate, Entity traveller, std::shared_ptr<ActionManager> actionManager)
    {
        std::vector<Operator> ops;

        for (int i = 0; i < _data.size; ++i)
        {
            for (Entity location : _data.locations[i])
            {
                std::vector<unsigned> params = {_data.entity[i].id, location.id, traveller.id};
                ops.push_back(actionManager->buildOperator(travelTemplate, params)); //Params: Entity fromLocation, Entity toLocation, Entity traveller
            }
        }

        return ops;
    }

    std::string getNameOfPlace(Entity entity)
    {
        Instance inst = lookup(entity);
        if (inst.i >= 0)
        {
            return _data.location[inst.i];
        }

        return "not found";
    }

    void spawnComponent(Entity entity, std::string location, std::vector<Entity> locations)
    {

        _map.emplace(entity.index(), _data.size);

        _data.entity.push_back(entity);
        _data.location.push_back(location);
        _data.locations.push_back(locations);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.location[i] = _data.location[last];
        _data.locations[i] = _data.locations[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.location.pop_back();
        _data.locations.pop_back();

        --_data.size;
    }
};


#endif //PARTIALORDERPLANNER_LOCATIONCOMPONENTMANAGERS_H
