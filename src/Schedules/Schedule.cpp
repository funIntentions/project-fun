//
// Created by Dan on 2/6/2016.
//

#include "Schedule.h"
#include "ScheduleEntry.h"


int Schedule::nextEntry(int entryIndex) const
{
    return (entryIndex + 1) % scheduleEntries.size(); //TODO: will break when size == 0?
}

int Schedule::getEntryAtTime(double currentTime) const
{
    if (scheduleEntries.size() == 1)
        return 0;

    for (unsigned int i = 0; i < scheduleEntries.size(); ++i)
    {
        double startTime = scheduleEntries[i]->getStartTime();
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

ActionInstance* Schedule::chooseNewAction(int entryIndex)
{
    return scheduleEntries[entryIndex]->chooseNewAction();
}

double Schedule::getEndTime(int entryIndex) const
{
    return scheduleEntries[nextEntry(entryIndex)]->getStartTime();
}
