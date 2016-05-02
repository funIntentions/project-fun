//
// Created by Dan on 2/7/2016.
//

#ifndef PARTIALORDERPLANNER_ACTIONINSTANCE_H
#define PARTIALORDERPLANNER_ACTIONINSTANCE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <framework/EntityManager.h>

class Action;
class Operator;

class ActionInstance {
private:
    const Action* action;
    double duration;
public:
    std::unordered_map<std::string, Entity> mappedParameters;

    ActionInstance(const Action* action, double duration);

    bool perform(double deltaTime);

    std::vector<std::string> getParameters() const;
    std::vector<int> getPreconditions();
    std::vector<int> getActionEffects();
    const std::string& getActionName() const;
    int getActionId() const;
    double getDuration() const
    {
        return duration;
    }
};


#endif //PARTIALORDERPLANNER_ACTIONINSTANCE_H
