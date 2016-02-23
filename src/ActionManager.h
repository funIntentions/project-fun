//
// Created by Dan on 2/22/2016.
//

#ifndef PARTIALORDERPLANNER_ACTIONFACTORY_H
#define PARTIALORDERPLANNER_ACTIONFACTORY_H

#include "schedules/Action.h"
#include "PartialOrderPlan.h"
#include <unordered_map>

class ActionManager {
private:
    std::unordered_map<Predicate, size_t> predicates;
    size_t addPredicate(Predicate predicate);

public:
    Operator buildOperator(Action& templateTask, std::vector<unsigned>& entities);
};


#endif //PARTIALORDERPLANNER_ACTIONFACTORY_H
