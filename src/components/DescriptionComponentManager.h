//
// Created by Dan on 4/2/2016.
//

#ifndef PARTIALORDERPLANNER_DESCRIPTIONCOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_DESCRIPTIONCOMPONENTMANAGER_H

#include <iostream>
#include <EntityManager.h>
#include <ComponentManager.h>

class DescriptionComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<std::string> name;
        std::vector<std::string> description;
    };

    InstanceData _data;

public:

    DescriptionComponentManager() : ComponentManager()
    {
        _data.size = 0;
    }

    void examineEntityWithName(std::string name)
    {
        std::string description = "Mysterious... It seems I can't examine this... thing?";

        for (int index = 0; index < _data.size; ++index)
        {
            if (_data.name[index] == name)
            {
                description = _data.description[index];
            }
        }

        std::cout << description << std::endl;
    }

    void examineEntity(Entity entity)
    {
        // TODO: check for entity in component manager
        std::cout << _data.description[entity.id] << std::endl;
    }

    void spawnComponent(Entity entity, std::string name, std::string description)
    {
        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);
        _data.name.push_back(name);
        _data.description.push_back(description);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.name[i] = _data.name[last];
        _data.description[i] = _data.description[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.name.pop_back();
        _data.description.pop_back();

        --_data.size;
    }
};


#endif //PARTIALORDERPLANNER_DESCRIPTIONCOMPONENTMANAGER_H