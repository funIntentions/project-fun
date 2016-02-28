//
// Created by Dan on 2/7/2016.
//

#include "ActionInstance.h"
#include "Action.h"


ActionInstance::ActionInstance(const Action* action, double duration, Operator* op)
                                : action(action), duration(duration), actionOperator(op)
{

}

bool ActionInstance::perform(double deltaTime)
{
    duration -= deltaTime;
    return (duration <= 0);
}

int ActionInstance::getActionId() const
{
    return action->getId();
}

const std::string& ActionInstance::getActionName() const
{
    return action->getName();
}
