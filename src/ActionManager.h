//
// Created by Dan on 2/22/2016.
//

#ifndef PARTIALORDERPLANNER_ACTIONFACTORY_H
#define PARTIALORDERPLANNER_ACTIONFACTORY_H

#include "schedules/Action.h"

class ActionManager {
private:
    static std::unordered_map<Predicate, size_t> predicates;

    static size_t addPredicate(Predicate predicate);

public:
    static Operator buildOperator(const Action& templateTask, const std::vector<std::string>& entities); // TODO: replace string with entity index val
};


#endif //PARTIALORDERPLANNER_ACTIONFACTORY_H
