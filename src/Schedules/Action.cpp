//
// Created by Dan on 2/6/2016.
//

#include <stdlib.h>
#include "Action.h"

double Action::calculateDuration() const
{
    double random = (double)rand() / RAND_MAX;
    return (minDuration + random * (maxDuration - minDuration));
}

ActionInstance* Action::createActionInstance() const
{
    return new ActionInstance(this, calculateDuration());
}
