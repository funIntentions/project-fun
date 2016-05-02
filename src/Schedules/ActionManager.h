//
// Created by Dan on 2/22/2016.
//

#ifndef PARTIALORDERPLANNER_ACTIONFACTORY_H
#define PARTIALORDERPLANNER_ACTIONFACTORY_H

#include "schedules/Action.h"
#include "planner/PartialOrderPlan.h"
#include <unordered_map>

class ActionManager {
private:
    std::unordered_map<Predicate, size_t> predicates;
    std::unordered_map<PredicateTemplate, size_t> predicateTemplates;
    std::unordered_map<size_t, Predicate> idToPredicateMap;
    std::unordered_map<size_t, PredicateTemplate> idToPredicateTemplateMap;

    size_t addPredicate(Predicate predicate);

public:
    Operator buildOperator(Action& templateTask, std::vector<unsigned>& entities);

    Predicate getPredicate(int id);

    PredicateTemplate getPredicateTemplate(int id);

    int getPredicateId(PredicateTemplate);

    size_t addPredicateTemplate(PredicateTemplate predicate);

    size_t buildState(std::string type, std::vector<unsigned> params);
};


#endif //PARTIALORDERPLANNER_ACTIONFACTORY_H
