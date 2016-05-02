//
// Created by Dan on 2/7/2016.
//

#include "ActionInstance.h"
#include "Action.h"
#include "planner/PartialOrderPlan.h"


ActionInstance::ActionInstance(const Action* action, double duration)
                                : action(action), duration(duration)
{

}

bool ActionInstance::perform(double deltaTime)
{
    duration -= deltaTime;
    return (duration <= 0);
}

std::vector<std::string> ActionInstance::getParameters() const
{
    return action->parameters;
}

std::vector<int> ActionInstance::getPreconditions()
{
    return action->actionOperator->preconditions;
}

std::vector<int> ActionInstance::getActionEffects()
{
    return action->actionOperator->addedEffects;
}

int ActionInstance::getActionId() const
{
    return action->getId();
}

const std::string& ActionInstance::getActionName() const
{
    return action->getName();
}
