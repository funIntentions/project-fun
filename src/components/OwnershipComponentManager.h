//
// Created by Dan on 4/10/2016.
//

#ifndef PARTIALORDERPLANNER_OWNERSHIPCOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_OWNERSHIPCOMPONENTMANAGER_H


#include <EntityManager.h>
#include <ActionManager.h>
#include <ComponentManager.h>
#include <unordered_set>
#include <set>

class OwnershipComponentManager : public ComponentManager {
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<Entity> owner;
        std::vector<bool> owned;
    };

    InstanceData _data;
    std::shared_ptr<ActionManager> _actionManager;
    std::unordered_map<unsigned, std::set<Entity>> ownership;

public:

    OwnershipComponentManager(std::shared_ptr<ActionManager> actionManager) : _actionManager(actionManager)
    {
            _data.size = 0;
    }

    bool isOwned(Entity belonging)
    {
        Instance belongingInstance = lookup(belonging);
        if (belongingInstance.i == -1)
            return false;

        return _data.owned[belongingInstance.i];
    }

    bool isOwnedBy(Entity entity, Entity belonging)
    {
        Instance belongingInstance = lookup(belonging);
        if (belongingInstance.i == -1)
            return false;

        return _data.owner[belongingInstance.i].id == entity.id;
    }

    std::vector<Entity> getBelongings(Entity entity)
    {
        auto ownershipItr = ownership.find(entity.id);
    if (ownershipItr != ownership.end()) return std::vector<Entity>(ownershipItr->second.begin(), ownershipItr->second.end());
        return {};
    }

    void giveOwnership(Entity entity, Entity belonging)
    {
        Instance instance = lookup(belonging);
        if (instance.i != -1)
        {
            if (_data.owned[instance.i])
            {
                auto belongingsItr = ownership.find(_data.owner[instance.i].id);
                if (belongingsItr != ownership.end())
                    belongingsItr->second.erase(belonging);
            }

            ownership[entity.id].insert(belonging);

            _data.owner[instance.i] = entity;
            _data.owned[instance.i] = true;
        }
    }

    void spawnComponent(Entity entity)
    {
        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);
        _data.owner.push_back({});
        _data.owned.push_back(false);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.owner[i] = _data.owner[last];
        _data.owned[i] = _data.owned[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.owner.pop_back();
        _data.owned.pop_back();

        --_data.size;
    }
};


#endif //PARTIALORDERPLANNER_OWNERSHIPCOMPONENTMANAGER_H
