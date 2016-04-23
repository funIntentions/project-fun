//
// Created by Dan on 4/22/2016.
//

#include "StateComponentManager.h"

StateComponentManager::StateComponentManager()
{
    _data.size = 0;
}

State::Health StateComponentManager::getHealth(Entity entity)
{
    Instance instance = lookup(entity);
    return _data.health[instance.i];
}

void StateComponentManager::setHealth(Entity entity, State::Health health)
{
    Instance instance = lookup(entity);
    _data.health[instance.i] = health;
}

void StateComponentManager::spawnComponent(Entity entity, State::Health health)
{
    _map.emplace(entity.index(), _data.size);
    _data.entity.push_back(entity);
    _data.health.push_back(health);

    ++_data.size;
}

void StateComponentManager::destroy(unsigned i)
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
