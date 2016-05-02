//
// Created by Dan on 2/6/2016.
//

#include <stdlib.h>
#include "ActionData.h"

double ActionData::calculateDuration() const
{
    double random = (double)rand() / RAND_MAX;
    return (minDuration + random * (maxDuration - minDuration));
}

Action* ActionData::createActionInstance() const
{
    return new Action(this, calculateDuration());
}
