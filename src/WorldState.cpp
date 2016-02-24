//
// Created by Dan on 2/23/2016.
//

#include "WorldState.h"

void WorldState::registerForAction(std::string action, OperatorCallbackFunction function)
{
    operatorCallbackFunctionMap.insert({action, function});
}
