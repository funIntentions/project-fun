//
// Created by Dan on 2/6/2016.
//

#include "Action.h"

double Action::calculateDuration() const
{
    // TODO: calculateRange using randomness.
    return minDuration;
}

ActionInstance* Action::createActionInstance(Operator* op, int index) const
{
    return new ActionInstance(this, calculateDuration(), index, op);
}
