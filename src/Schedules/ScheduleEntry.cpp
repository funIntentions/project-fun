//
// Created by Dan on 2/6/2016.
//

#include "ScheduleEntry.h"
#include "Action.h"

// Simple Schedule Entry

void SimpleScheduleEntry::addAction(Action* a)
{
    action = a;
}

ActionInstance* SimpleScheduleEntry::chooseNewAction()
{
    return action->createActionInstance();
}
