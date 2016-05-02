//
// Created by Dan on 2/7/2016.
//

#include "Action.h"
#include "ActionData.h"
#include "planner/PartialOrderPlan.h"


Action::Action(const ActionData* action, double duration)
                                : action(action), duration(duration)
{

}

bool Action::perform(double deltaTime)
{
    duration -= deltaTime;
    return (duration <= 0);
}

std::vector<std::string> Action::getParameters() const
{
    return action->parameters;
}

std::vector<int> Action::getPreconditions()
{
    return action->actionOperator->preconditions;
}

std::vector<int> Action::getActionEffects()
{
    return action->actionOperator->addedEffects;
}

int Action::getActionId() const
{
    return action->getId();
}

const std::string& Action::getActionName() const
{
    return action->getName();
}
