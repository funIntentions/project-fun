//
// Created by dan on 14/07/15.
//

#ifndef PARTIALORDERPLANNER_ENTITYMANAGER_H
#define PARTIALORDERPLANNER_ENTITYMANAGER_H

#include <vector>
#include <deque>

const unsigned MINIMUM_FREE_INDICES = 1024;

const unsigned ENTITY_INDEX_BITS = 22;
const unsigned ENTITY_INDEX_MASK = (1 << ENTITY_INDEX_BITS) - 1;

const unsigned ENTITY_GENERATION_BITS = 8;
const unsigned ENTITY_GENERATION_MASK = (1 << ENTITY_GENERATION_BITS) - 1;

struct Entity
{
    unsigned id;

    unsigned index() const {return id & ENTITY_INDEX_MASK;}
    unsigned generation() const {return (id >> ENTITY_INDEX_BITS) & ENTITY_GENERATION_MASK;}
};

class EntityManager
{
private:
    std::vector<unsigned char> _generation;
    std::deque<unsigned> _freeIndices;

    Entity MakeEntity(unsigned idx, unsigned char gen);

public:

    Entity Create();
    bool Alive(Entity e) const;
    void Destroy(Entity e);

};


#endif //PARTIALORDERPLANNER_ENTITYMANAGER_H
