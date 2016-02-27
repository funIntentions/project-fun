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
    std::unordered_map<size_t, Predicate> idToPredicateMap;
    size_t addPredicate(Predicate predicate);

public:
    Operator buildOperator(Action& templateTask, std::vector<unsigned>& entities);

    Predicate getPredicate(int id);

    size_t buildState(std::string type, std::vector<unsigned> params);
};


#endif //PARTIALORDERPLANNER_ACTIONFACTORY_H
