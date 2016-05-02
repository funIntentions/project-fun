//
// Created by Dan on 2/7/2016.
//

#include "Schedule.h"
#include "ScheduleData.h"
#include "ScheduleEntry.h"

Schedule::Schedule(ScheduleData* s) : entryIndex(-1), entryEndTime(0), actionIndex(0)
{
    schedule = s;
}

void Schedule::chooseEntryForTime(double currentTime)
{
    entryIndex = getEntryAtTime(currentTime);
    entryEndTime = getEndTime(entryIndex);
}

bool Schedule::timeIsUp(double lastTime, double currentTime)
{
    return ((entryEndTime > lastTime || lastTime > currentTime) && entryEndTime <= currentTime); // TODO: if the start time of a entry is 24 what happens?
}

Action* Schedule::chooseNewAction()
{
    return schedule->entries[entryIndex]->chooseNewAction(this);
}

int Schedule::getId() const { return schedule->getId(); }

std::string Schedule::getName() const { return schedule->getName(); }

void Schedule::startNextScheduleEntry()
{
    entryIndex = nextEntry(entryIndex);
    entryEndTime = getEndTime(entryIndex);
    actionIndex = 0;
}

int Schedule::nextEntry(int entryIndex) const
{
    assert(schedule->entries.size() > 0); // Will break if there are no entries in a schedule.
    return (entryIndex + 1) % schedule->entries.size();
}

int Schedule::getEntryAtTime(double currentTime) const
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

double Schedule::getEndTime(int entryIndex) const
{
    return schedule->entries[nextEntry(entryIndex)]->getStartTime();
}
