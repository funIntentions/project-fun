//
// Created by Dan on 2/6/2016.
//

#include "util/Extra.h"
#include "ScheduleEntry.h"
#include "Schedule.h"

// Simple ScheduleData Entry

void SimpleScheduleEntry::addAction(ActionData* a)
{
    action = a;
}

Action* SimpleScheduleEntry::chooseNewAction(Schedule*)
{
    return action->createActionInstance();
}

ScheduleEntry* SimpleScheduleEntry::clone(const std::string& name, const int& id, double startTime)
{
    SimpleScheduleEntry* simpleScheduleEntry = new SimpleScheduleEntry(name, id, startTime);
    simpleScheduleEntry->action = action;
    return simpleScheduleEntry;
}

// Sequence ScheduleData Entry

void SequenceScheduleEntry::addAction(ActionData* action)
{
    actions.push_back(action);
}

Action* SequenceScheduleEntry::chooseNewAction(Schedule* scheduleInstance)
{
    unsigned actionIndex = scheduleInstance->getActionIndex();
    if (actionIndex >= actions.size())
        actionIndex = actions.size() - 1;

    scheduleInstance->setActionIndex(actionIndex + 1);

    ActionData* action = actions[actionIndex];

    return action->createActionInstance();
}

ScheduleEntry* SequenceScheduleEntry::clone(const std::string& name, const int& id, double startTime)
{
    SequenceScheduleEntry* entry = new SequenceScheduleEntry(name, id, startTime);

    entry->actions.reserve(actions.size());

    for (auto action : actions)
    {
        entry->actions.push_back(action);
    }

    return entry;
}
