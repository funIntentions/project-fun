//
// Created by Dan on 2/7/2016.
//

#include "ActionInstance.h"
#include "Action.h"


ActionInstance::ActionInstance(const Action* action, double duration, int index)
                                : action(action), duration(duration), index(index)
{

}

bool ActionInstance::perform(double deltaTime)
{
    duration -= deltaTime;
    return (duration <= 0);
}

const int ActionInstance::getActionId() const {
    return action->getId();
}
