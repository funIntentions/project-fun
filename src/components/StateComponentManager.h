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
    enum Activity
    {
        None,
        Fishing,
        Hooked,
        Apprehended,
        Confronted,
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
        std::vector<State::Activity> state;
    };

    InstanceData _data;

public:

    StateComponentManager();

    State::Health getHealth(Entity entity);

    State::Activity getState(Entity entity);

    void setHealth(Entity entity, State::Health);

    void setState(Entity entity, State::Activity);

    void spawnComponent(Entity entity, State::Health health, State::Activity state);

    void destroy(unsigned i);
};

#endif //PARTIALORDERPLANNER_STATECOMPONENTMANAGER_H
