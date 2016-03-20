//
// Created by Dan on 2/22/2016.
//

#include <assert.h>
#include "ActionManager.h"

size_t ActionManager::addPredicate(Predicate predicate)
{
    size_t id = predicates.size();
    auto itr = predicates.find(predicate);
    if (itr != predicates.end())
        id = itr->second;
    else
    {
        predicates.insert({predicate, id});
        idToPredicateMap.insert({id, predicate});
    }

    return id;
}

int ActionManager::getPredicateId(PredicateTemplate predicateTemplate)
{
    auto itr = predicateTemplates.find(predicateTemplate);
    if (itr != predicateTemplates.end())
        return itr->second;

    return -1;
}

size_t ActionManager::addPredicateTemplate(PredicateTemplate predicate)
{
    size_t id = predicateTemplates.size();
    auto itr = predicateTemplates.find(predicate);
    if (itr != predicateTemplates.end())
        id = itr->second;
    else
    {
        predicateTemplates.insert({predicate, id});
        idToPredicateTemplateMap.insert({id, predicate});
    }

    return id;
}

Predicate ActionManager::getPredicate(int id)
{
    auto itr = idToPredicateMap.find(id);
    return itr->second;
}


PredicateTemplate ActionManager::getPredicateTemplate(int id)
{
    auto itr = idToPredicateTemplateMap.find(id);
    return itr->second;
}

Operator ActionManager::buildOperator(Action& templateTask, std::vector<unsigned>& entities)
{
    Operator anOperator;
    /*anOperator.name = templateTask.getName();

    assert(templateTask.parameters.size() == entities.size());

    std::unordered_map<std::string, unsigned> paramMapping;
    for (int i = 0; i < templateTask.parameters.size(); ++i)
    {
        paramMapping.insert({templateTask.parameters[i], entities[i]});
    }

    for (PredicateTemplate precondition : templateTask.preconditions)
    {
        Predicate newPrecondition;
        newPrecondition.type = precondition.type;

        for (std::string param : precondition.params)
        {
            auto itr = paramMapping.find(param);
            if (itr != paramMapping.end())
            {
                newPrecondition.params.push_back(itr->second);
            }
        }

        anOperator.preconditions.push_back(addPredicate(newPrecondition));
    }

    for (PredicateTemplate effect : templateTask.addedEffects)
    {
        Predicate newEffect;
        newEffect.type = effect.type;

        for (std::string param : effect.params)
        {
            auto itr = paramMapping.find(param);
            if (itr != paramMapping.end())
            {
                newEffect.params.push_back(itr->second);
            }
        }

        anOperator.addedEffects.push_back(addPredicate(newEffect));
    }

    for (PredicateTemplate effect : templateTask.subtractedEffects)
    {
        Predicate newEffect;
        newEffect.type = effect.type;

        for (std::string param : effect.params)
        {
            auto itr = paramMapping.find(param);
            if (itr != paramMapping.end())
            {
                newEffect.params.push_back(itr->second);
            }
        }

        anOperator.subtractedEffects.push_back(addPredicate(newEffect));
    }
*/
    return anOperator;
}

size_t ActionManager::buildState(std::string type, std::vector<unsigned> params)
{
    Predicate goal;
    goal.type = type;
    goal.params = params;

    auto itr = predicates.find(goal);
    assert(itr != predicates.end()); // Otherwise goal is not obtainable (although there is other ways for it to be unattainable: no addedEffect predicate) TODO: bullet proof this if possible, or at least make it safe by handling edge cases

    return itr->second;
}
