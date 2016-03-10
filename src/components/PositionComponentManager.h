//
// Created by Dan on 3/6/2016.
//

#ifndef PARTIALORDERPLANNER_POSITIONCOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_POSITIONCOMPONENTMANAGER_H

#include <ActionManager.h>
#include <memory>
#include "ComponentManager.h"
#include "LocationComponentManager.h"

class PositionComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<Entity> location;
    };

    InstanceData _data;
    std::shared_ptr<LocationComponentManager> _locationComponentManger;

public:

    PositionComponentManager(std::shared_ptr<LocationComponentManager> locationComponentManager) : ComponentManager(), _locationComponentManger(locationComponentManager)
    {
        _data.size = 0;
    }

    Entity getLocationForEntity(Entity entity)
    {
        return _data.location[entity.id];
    }

    std::vector<int> getEntityState(Entity entity, ActionManager& actionManager)
    {
        Instance instance = lookup(entity);
        std::vector<unsigned> atParams = {_data.location[instance.i].id, entity.id};
        int atState = actionManager.buildState("at", atParams);
        std::vector<int> state = {atState};
        return state;
    }

    void changeEntitiesLocation(Entity entity, Entity location)
    {
        Instance inst = lookup(entity);
        std::cout << "old location: " << _locationComponentManger->getNameOfPlace(_data.location[inst.i]) << std::endl;
        std::cout << "new location: " << _locationComponentManger->getNameOfPlace(location) << std::endl << std::endl;
        _data.location[inst.i] = location;
    }

    void spawnComponent(Entity entity, Entity location)
    {
        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);
        _data.location.push_back(location);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.location[i] = _data.location[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.location.pop_back();

        --_data.size;
    }
};



#endif //PARTIALORDERPLANNER_POSITIONCOMPONENTMANAGER_H
