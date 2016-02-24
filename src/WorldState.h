//
// Created by Dan on 2/23/2016.
//

#ifndef PARTIALORDERPLANNER_WORLDSTATE_H
#define PARTIALORDERPLANNER_WORLDSTATE_H

#include "EntityManager.h"
#include "schedules/Predicate.h"
#include "PartialOrderPlan.h"

struct WorldState {
    Entity entity;
    std::vector<int> goals;
    std::vector<Operator> operators;
};

#endif //PARTIALORDERPLANNER_WORLDSTATE_H
