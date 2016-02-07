//
// Created by Dan on 2/7/2016.
//

#include "ScheduleInstance.h"
#include "Schedule.h"

ScheduleInstance::ScheduleInstance(Schedule* s)
{
    schedule = s;
}

void ScheduleInstance::startNextScheduleEntry()
{
    entryIndex = schedule->nextEntry(entryIndex);
    entryEndTime = schedule->getEndTime(entryIndex);
}

bool ScheduleInstance::timeIsUp(double lastTime, double currentTime)
{
    return (entryEndTime < lastTime && entryEndTime >= currentTime);
}

ActionInstance* ScheduleInstance::chooseNewAction()
{
    return schedule->chooseNewAction(entryIndex);
}

int ScheduleInstance::getId() const { return schedule->getId(); }
