//
// Created by Dan on 4/10/2016.
//

#ifndef PARTIALORDERPLANNER_OWNERSHIPCOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_OWNERSHIPCOMPONENTMANAGER_H


#include <EntityManager.h>
#include <ActionManager.h>
#include <ComponentManager.h>
#include <unordered_set>

class OwnershipComponentManager : public ComponentManager {
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<std::unordered_set<unsigned>> belongings;
    };

    InstanceData _data;
    std::shared_ptr<ActionManager> _actionManager;

public:

    OwnershipComponentManager(std::shared_ptr<ActionManager> actionManager) : _actionManager(actionManager)
    {
            _data.size = 0;
    }

    bool hasOwnership(Entity entity, Entity belonging)
    {
        Instance instance = lookup(entity);
        if (instance.i != -1)
        {
            auto itr = _data.belongings[instance.i].find(belonging.id);

            return itr != _data.belongings[instance.i].end();
        }
        return false;
    }

    std::vector<unsigned> getBelongings(Entity entity)
    {
        Instance instance = lookup(entity);
        if (instance.i != -1)
        {
            std::vector<unsigned> state(_data.belongings[instance.i].begin(), _data.belongings[instance.i].end());
            return state;
        }

        return {};
    }

    void giveOwnership(Entity entity, Entity belonging)
    {
        Instance instance = lookup(entity);
        if (instance.i != -1)
        {
            auto itr = _data.belongings[instance.i].find(belonging.id);
            if (itr == _data.belongings[instance.i].end())
                _data.belongings[instance.i].insert(belonging.id);
        }
    }

    void endOwnership(Entity entity, Entity belonging)
    {
        Instance instance = lookup(entity);
        if (instance.i != -1)
        {
            auto itr = _data.belongings[instance.i].find(belonging.id);
            if (itr != _data.belongings[instance.i].end())
                _data.belongings[instance.i].erase(itr);
        }
    }

    void spawnComponent(Entity entity)
    {
        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();

        --_data.size;
    }
};


#endif //PARTIALORDERPLANNER_OWNERSHIPCOMPONENTMANAGER_H
