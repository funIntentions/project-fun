//
// Created by Dan on 4/22/2016.
//

#ifndef PARTIALORDERPLANNER_STATECOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_STATECOMPONENTMANAGER_H

#include <EntityManager.h>
#include <ComponentManager.h>
#include <ActionManager.h>

struct State
{
    enum Health
    {
        Alive,
        Dead
    };
};

class StateComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<State::Health> health;
    };

    InstanceData _data;

public:

    StateComponentManager();

    State::Health getHealth(Entity entity);

    void setHealth(Entity entity, State::Health);

    void spawnComponent(Entity entity, State::Health health);

    void destroy(unsigned i);
};

#endif //PARTIALORDERPLANNER_STATECOMPONENTMANAGER_H
