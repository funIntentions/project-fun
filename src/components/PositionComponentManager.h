//
// Created by Dan on 3/6/2016.
//

#ifndef PARTIALORDERPLANNER_POSITIONCOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_POSITIONCOMPONENTMANAGER_H

#include "ComponentManager.h"

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

public:

    PositionComponentManager() : ComponentManager()
    {
        _data.size = 0;
    }

    Entity getLocationForEntity(Entity entity)
    {
        return _data.location[entity.id];
    }

    /*std::vector<int> getEntityState(Entity entity, ActionManager& actionManager)
    {
        return {actionManager.buildState("at", {_data.location[entity.id].id, entity.id})};
    }*/

    void changeEntitiesLocation(Entity entity, Entity location)
    {
        _data.location[entity.id] = location;
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
