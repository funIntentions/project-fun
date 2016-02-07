//
// Created by Dan on 2/6/2016.
//

#include "Schedule.h"
#include "ScheduleEntry.h"


int Schedule::nextEntry(int entryIndex) const
{
    return (entryIndex + 1) % scheduleEntries.size();
}

ActionInstance* Schedule::chooseNewAction(int entryIndex)
{
    return scheduleEntries[entryIndex]->chooseNewAction();
}

double Schedule::getEndTime(int entryIndex) const
{
    return scheduleEntries[nextEntry(entryIndex)]->getStartTime();
}
