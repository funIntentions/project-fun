//
// Created by Dan on 2/7/2016.
//

#include <assert.h>
#include "ScheduleInstance.h"
#include "Schedule.h"
#include "ScheduleEntry.h"

ScheduleInstance::ScheduleInstance(Schedule* s) : entryIndex(-1), entryEndTime(0)
{
    schedule = s;
}

void ScheduleInstance::setupEntries(std::unordered_map<std::string, OperatorCallbackFunction>& operatorCallbacks, Entity entity)
{
    for (ScheduleEntry* entryTemplate : schedule->entryTemplates)
    {
        entries.push_back(entryTemplate->setupEntry(operatorCallbacks, entity));
    }
}

void ScheduleInstance::chooseEntryForTime(double currentTime, WorldState& worldState)
{
    if (entryIndex > 0)
        entries[entryIndex]->endEntry();

    entryIndex = getEntryAtTime(currentTime);
    entryEndTime = getEndTime(entryIndex);

    entries[entryIndex]->startEntry(worldState);
}

bool ScheduleInstance::timeIsUp(double lastTime, double currentTime)
{
    return ((entryEndTime > lastTime || lastTime > currentTime) && entryEndTime <= currentTime); // TODO: if the start time of a entry is 24 what happens?
}

ActionInstance* ScheduleInstance::chooseNewAction(WorldState& worldState)
{
    return entries[entryIndex]->chooseNewAction(worldState);
}

int ScheduleInstance::getId() const { return schedule->getId(); }

void ScheduleInstance::startNextScheduleEntry(WorldState& worldState)
{
    entries[entryIndex]->endEntry();

    entryIndex = nextEntry(entryIndex);
    entryEndTime = getEndTime(entryIndex);

    entries[entryIndex]->startEntry(worldState);
}

int ScheduleInstance::nextEntry(int entryIndex) const
{
    assert(entries.size() > 0); // Will break if there are no entries in a schedule.
    return (entryIndex + 1) % entries.size();
}

int ScheduleInstance::getEntryAtTime(double currentTime) const
{
    if (entries.size() == 1)
        return 0;

    for (unsigned int i = 0; i < entries.size(); ++i)
    {
        double startTime = entries[i]->getStartTime();
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
    return entries[nextEntry(entryIndex)]->getStartTime();
}
