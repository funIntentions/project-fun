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

void ScheduleInstance::setupEntries(std::unordered_map<std::string, OperatorCallbackFunction>& operatorCallbacks, Entity entity)
{
    // TODO: Remove
    /*for (ScheduleEntry* entryTemplate : schedule->entryTemplates)
    {
        entries.push_back(entryTemplate->setupEntry(operatorCallbacks, entity));
    }*/
}

void ScheduleInstance::chooseEntryForTime(double currentTime, WorldState& worldState)
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

void ScheduleInstance::startNextScheduleEntry(WorldState& worldState)
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
