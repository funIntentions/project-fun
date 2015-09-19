//
// Created by dan on 14/07/15.
//

#include <assert.h>
#include "EntityManager.h"


Entity EntityManager::makeEntity(unsigned idx, unsigned char gen)
{
    Entity newEntity;
    newEntity.id = (idx | gen);
    return newEntity;
}

Entity EntityManager::create()
{
    unsigned idx;

    if (_freeIndices.size() > MINIMUM_FREE_INDICES)
    {
        idx = _freeIndices.front();
        _freeIndices.pop_front();
    }
    else
    {
        _generation.push_back(0);
        idx = _generation.size() - 1;
        assert(idx < (1 << ENTITY_INDEX_BITS));
    }

    return makeEntity(idx, _generation[idx]);
}

bool EntityManager::alive(Entity e) const
{
    return _generation[e.index()] == e.generation();
}

void EntityManager::destroy(Entity e)
{
    const unsigned idx = e.index();
    ++_generation[idx];
    _freeIndices.push_back(idx);
}