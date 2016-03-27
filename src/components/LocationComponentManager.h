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
        std::vector<std::string> name;
        std::vector<std::vector<Entity>> localEntities;

    };

    InstanceData _data;

public:

    LocationComponentManager() : ComponentManager()
    {
        _data.size = 0;
    }

    void spawnComponent(Entity entity, std::string name, std::vector<Entity> locals)
    {

        _map.emplace(entity.index(), _data.size);

        _data.entity.push_back(entity);
        _data.name.push_back(name);
        _data.localEntities.push_back(locals);

        ++_data.size;
    }

    void addLocalEntity(Entity location, Entity local)
    {
        Instance instance = lookup(location);
        if (instance.i >= 0)
        {
            _data.localEntities[instance.i].push_back(local);
        }
    }

    void removeLocalEntity(Entity location, Entity local)
    {
        Instance instance = lookup(location);
        if (instance.i >= 0)
        {
            for (auto itr = _data.localEntities[instance.i].begin(); itr != _data.localEntities[instance.i].end(); ++itr)
            {
                if (itr->id == local.id)
                {
                    _data.localEntities[instance.i].erase(itr);
                    return;
                }
            }
        }
    }

    std::vector<Entity> getLocalEntities(Entity location)
    {
        Instance instance = lookup(location);
        return _data.localEntities[instance.i];
    }

    std::string getLocationName(Entity location)
    {
        Instance instance = lookup(location);
        return _data.name[instance.i];
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.name[i] = _data.name[last];
        _data.localEntities[i] = _data.localEntities[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.name.pop_back();
        _data.localEntities.pop_back();

        --_data.size;
    }
};


#endif //PARTIALORDERPLANNER_LOCATIONCOMPONENTMANAGERS_H
