//
// Created by Dan on 2/7/2016.
//

#include "ScheduleInstance.h"
#include "Schedule.h"
#include "ScheduleEntry.h"

ScheduleInstance::ScheduleInstance(Schedule* s) : entryIndex(-1), entryEndTime(0), actionIndex(0)
{
    schedule = s;
}

void ScheduleInstance::chooseEntryForTime(double currentTime)
{
    entryIndex = getEntryAtTime(currentTime);
    entryEndTime = getEndTime(entryIndex);
}

bool ScheduleInstance::timeIsUp(double lastTime, double currentTime)
{
    return ((entryEndTime > lastTime || lastTime > currentTime) && entryEndTime <= currentTime); // TODO: if the start time of a entry is 24 what happens?
}

ActionInstance* ScheduleInstance::chooseNewAction()
{
    return schedule->entries[entryIndex]->chooseNewAction(this);
}

int ScheduleInstance::getId() const { return schedule->getId(); }

std::string ScheduleInstance::getName() const { return schedule->getName(); }

void ScheduleInstance::startNextScheduleEntry()
{
    entryIndex = nextEntry(entryIndex);
    entryEndTime = getEndTime(entryIndex);
    actionIndex = 0;
}

int ScheduleInstance::nextEntry(int entryIndex) const
{
    assert(schedule->entries.size() > 0); // Will break if there are no entries in a schedule.
    return (entryIndex + 1) % schedule->entries.size();
}

int ScheduleInstance::getEntryAtTime(double currentTime) const
{
    if (currentTime == 0)
        currentTime = 24.0;

    if (schedule->entries.size() == 1)
        return 0;

    for (unsigned int i = 0; i < schedule->entries.size(); ++i)
    {
        double startTime = schedule->entries[i]->getStartTime();
        double endTime = getEndTime(i);

        if (endTime >= startTime)
        {
            if (currentTime >= startTime && currentTime < endTime) return i;
        }
        else
        {
            if (currentTime >= startTime || currentTime) return i;
        }
    }

    return 0;
}

double ScheduleInstance::getEndTime(int entryIndex) const
{
    return schedule->entries[nextEntry(entryIndex)]->getStartTime();
}
