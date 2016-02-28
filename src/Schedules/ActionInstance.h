//
// Created by Dan on 2/7/2016.
//

#ifndef PARTIALORDERPLANNER_ACTIONINSTANCE_H
#define PARTIALORDERPLANNER_ACTIONINSTANCE_H

#include <string>
#include <vector>

class Action;
class Operator;

class ActionInstance {
private:
    const Action* action;
    double duration;
public:

    ActionInstance(const Action* action, double duration, Operator* op);

    bool perform(double deltaTime);

    const std::string& getActionName() const;
    int getActionId() const;
    double getDuration() const
    {
        return duration;
    }

    Operator* actionOperator;
};


#endif //PARTIALORDERPLANNER_ACTIONINSTANCE_H
