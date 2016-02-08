//
// Created by Dan on 2/7/2016.
//

#ifndef PARTIALORDERPLANNER_ACTIONINSTANCE_H
#define PARTIALORDERPLANNER_ACTIONINSTANCE_H

#include <string>

class Action;

class ActionInstance {
private:
    const Action* action;
    double duration;
    int index;
public:

    ActionInstance(const Action* action, double duration, int index);

    bool perform(double deltaTime);

    const std::string& getActionName() const;
    int getActionId() const;
    double getDuration() const
    {
        return duration;
    }
    int getIndex() const
    {
        return index;
    }
};


#endif //PARTIALORDERPLANNER_ACTIONINSTANCE_H
