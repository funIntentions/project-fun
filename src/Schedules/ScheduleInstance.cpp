//
// Created by Dan on 2/7/2016.
//

#include "ScheduleInstance.h"
#include "Schedule.h"
#include <iostream>

ScheduleInstance::ScheduleInstance(Schedule* s)
{
    schedule = s;
}

void ScheduleInstance::chooseEntryForTime(double currentTime)
{
    entryIndex = schedule->getEntryAtTime(currentTime);
    entryEndTime = schedule->getEndTime(entryIndex);
}

void ScheduleInstance::startNextScheduleEntry()
{
    entryIndex = schedule->nextEntry(entryIndex);
    entryEndTime = schedule->getEndTime(entryIndex);
}

bool ScheduleInstance::timeIsUp(double lastTime, double currentTime)
{
    return (entryEndTime > lastTime && entryEndTime <= currentTime); // TODO: when time wraps, 24 to 0, if the end time is 0 this wont trigger and the behaviour will never end.
}

ActionInstance* ScheduleInstance::chooseNewAction()
{
    return schedule->chooseNewAction(entryIndex);
}

int ScheduleInstance::getId() const { return schedule->getId(); }
