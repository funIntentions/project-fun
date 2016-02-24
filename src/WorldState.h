//
// Created by Dan on 2/23/2016.
//

#ifndef PARTIALORDERPLANNER_WORLDSTATE_H
#define PARTIALORDERPLANNER_WORLDSTATE_H


#include <schedules/Action.h>
#include "PartialOrderPlan.h"
#include "EntityManager.h"

typedef std::function<std::vector<Operator>(Action, Entity)> OperatorCallbackFunction;

// TODO: name to something more appropriate "task domain" or abilities or something
class WorldState {
public:
    std::unordered_map<std::string, OperatorCallbackFunction> operatorCallbackFunctionMap;
    //std::vector<Operator> operators;
    //std::vector<Predicate> goals;
    std::vector<Action> actions;

    void registerForAction(std::string action, OperatorCallbackFunction function);
};


#endif //PARTIALORDERPLANNER_WORLDSTATE_H
