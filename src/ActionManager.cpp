//
// Created by Dan on 2/22/2016.
//

#include "ActionManager.h"

size_t ActionManager::addPredicate(Predicate predicate)
{
    size_t id = predicates.size();
    auto itr = predicates.find(predicate);
    if (itr != predicates.end())
        id = itr->second;
    else
        predicates.insert({predicate, id});
    return id;
}

Operator ActionManager::buildOperator(const Action& templateTask, const std::vector<std::string>& entities)
{
    Operator anOperator;
    anOperator.name = templateTask.name;

    assert(templateTask.parameters.size() == entities.size());

    std::unordered_map<std::string, std::string> paramMapping;
    for (int i = 0; i < templateTask.parameters.size(); ++i)
    {
        paramMapping.insert({templateTask.parameters[i], entities[i]});
    }

    for (Predicate precondition : templateTask.positivePreconditions)
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

    for (Predicate effect : templateTask.addedEffects)
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

    for (Predicate effect : templateTask.subtractedEffects)
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

    return anOperator;
}

