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

ScheduleEntry* SimpleScheduleEntry::clone(const std::string& name, const int& id, double startTime)
{
    SimpleScheduleEntry* simpleScheduleEntry = new SimpleScheduleEntry(name, id, startTime);
    simpleScheduleEntry->action = action;
    return simpleScheduleEntry;
}
