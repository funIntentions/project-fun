//
// Created by dan on 14/07/15.
//

#ifndef PARTIALORDERPLANNER_COMPONENTMANAGER_H
#define PARTIALORDERPLANNER_COMPONENTMANAGER_H

#include <unordered_map>
#include "EntityManager.h"

struct Instance {int i;};

class ComponentManager
{
protected:

    std::unordered_map<unsigned, unsigned> _map;

    Instance makeInstance(int i)
    {
        Instance inst = {i};
        return inst;
    }

public:

    ComponentManager() {}

    Instance lookup(Entity e)
    {
        std::unordered_map<unsigned,unsigned>::iterator it;
        if ((it = _map.find(e.index())) != _map.end())
            return makeInstance(it->second);
        return {-1};
    }

    virtual void destroy(unsigned i) = 0;
};


#endif //PARTIALORDERPLANNER_COMPONENTMANAGER_H
