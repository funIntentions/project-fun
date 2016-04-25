//
// Created by Dan on 3/26/2016.
//

#ifndef PARTIALORDERPLANNER_ATTRIBUTECOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_ATTRIBUTECOMPONENTMANAGER_H

#include "ComponentManager.h"
#include "ActionManager.h"
#include "Attribute.h"
#include <memory>
#include <string>
#include <numeric>
#include <assert.h>

class AttributeComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<std::vector<float>> attributes;
    };

    InstanceData _data;

public:

    AttributeComponentManager() : ComponentManager()
    {
        _data.size = 0;
    }

    void spawnComponent(Entity entity)
    {
        _map.emplace(entity.index(), _data.size);

        _data.entity.push_back(entity);
        _data.attributes.push_back(std::vector<float>(Attribute::NumberOfAttributes));

        ++_data.size;
    }

    // Sorts in ascending order such that operators with the smallest difference between attributes (closely align) are first.
    static bool sortOperatorsByPreference(const std::pair<float, int>& lhs, const std::pair<float, int>& rhs)
    {
        return lhs.first < rhs.first;
    }

    float calculatePreference(const std::vector<float>& lhs, const std::vector<float>& rhs)
    {
        assert(lhs.size() == rhs.size());

        float sum = 0.0f;
        for (unsigned i = 0; i < lhs.size(); ++i)
        {
            float difference = lhs[i] - rhs[i];
            sum += difference * difference;
        }

        return sum;
    }

    std::vector<Operator> sortActionOperations(const std::unordered_map<int, Action*>& actions, Entity entity)
    {
        Instance instance = lookup(entity);
        std::vector<float> attributes = _data.attributes[instance.i];
        std::vector<std::pair<float, int>> operatorPreferences;

        // calculate this entity's preference (product) for each action
        for (auto actionPair : actions)
        {
            Action* action = actionPair.second;
            int actionId = actionPair.first;

            float preference = calculatePreference(action->attributes, attributes);
            action->actionOperator->score = preference;
            operatorPreferences.push_back({preference, actionId});
        }

        // sort the preference operator pair so that the most preferred operators are first (ascending order)
        std::sort(operatorPreferences.begin(), operatorPreferences.end(), sortOperatorsByPreference);

        // return sorted the operators in the form that is needed by the planner
        std::vector<Operator> orderedOperators;
        for (auto operatorPair : operatorPreferences)
        {
            auto actionOperatorItr = actions.find(operatorPair.second);
            Operator actionOperator = *actionOperatorItr->second->actionOperator;
            orderedOperators.push_back(actionOperator);
        }

        return orderedOperators;
    }

    float getAttribute(Entity entity, Attribute attribute)
    {
        Instance instance = lookup(entity);
        return _data.attributes[instance.i][attribute];
    }

    void setAttribute(Entity entity, Attribute attribute, float value)
    {
        Instance instance = lookup(entity);
        if (instance.i >= 0)
            _data.attributes[instance.i][attribute] = value;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.attributes[i] = _data.attributes[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.attributes.pop_back();

        --_data.size;
    }
};


#endif //PARTIALORDERPLANNER_ATTRIBUTECOMPONENTMANAGER_H
