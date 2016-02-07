//
// Created by Dan on 2/6/2016.
//

#include "ScheduleEntry.h"
#include "Action.h"

// Simple Schedule Entry

virtual void SimpleScheduleEntry::addAction(Action* a)
{
    action = a;
}

virtual ActionInstance* SimpleScheduleEntry::chooseNewAction()
{
    return action->createActionInstance();
}
