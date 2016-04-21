//
// Created by Dan on 3/6/2016.
//

#ifndef PARTIALORDERPLANNER_POSITIONCOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_POSITIONCOMPONENTMANAGER_H

#include <ActionManager.h>
#include <memory>
#include "ComponentManager.h"
#include "LocationComponentManager.h"
#include "OpinionComponentManager.h"

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
    std::shared_ptr<OpinionComponentManager> _characterComponentManager;

public:

    PositionComponentManager(std::shared_ptr<LocationComponentManager> locationComponentManager, std::shared_ptr<OpinionComponentManager> characterComponentManager) : ComponentManager(), _locationComponentManger(locationComponentManager), _characterComponentManager(characterComponentManager)
    {
        _data.size = 0;
    }

    Entity getLocation(Entity entity)
    {
        Instance instance = lookup(entity);
        return _data.location[instance.i];
    }

    std::vector<int> getEntityState(Entity entity, ActionManager& actionManager)
    {
        Instance instance = lookup(entity);
        std::vector<unsigned> atParams = {_data.location[instance.i].id, entity.id};
        int atState = actionManager.buildState("at", atParams); // This will crash it, assert fails
        std::vector<int> state = {atState};
        return state;
    }

    void changeLocation(Entity entity, Entity location)
    {
        Instance inst = lookup(entity);
        _locationComponentManger->removeLocalEntity(_data.location[inst.i], entity);
        std::cout << "old location: " << _locationComponentManger->getLocationName(_data.location[inst.i]) << std::endl;
        _locationComponentManger->addLocalEntity(location, entity);
        std::cout << "new location: " << _locationComponentManger->getLocationName(location) << std::endl << std::endl;
        _data.location[inst.i] = location;

        updateKnowledge(entity, location);
    }

    void updateKnowledge(Entity entity, Entity location)
    {
        std::vector<Entity> localEntities = _locationComponentManger->getLocalEntities(location);
        for (Entity local : localEntities)
        {
            _characterComponentManager->addKnowledge(local, {entity});
        }

        _characterComponentManager->addKnowledge(entity, localEntities);
    }

    void spawnComponent(Entity entity, Entity location)
    {
        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);
        _data.location.push_back(location);

        _locationComponentManger->addLocalEntity(location, entity);
        updateKnowledge(entity, location);

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
