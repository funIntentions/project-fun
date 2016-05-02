//
// Created by Dan on 2/7/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULEINSTANCE_H
#define PARTIALORDERPLANNER_SCHEDULEINSTANCE_H

#include <components/ScheduleComponentManager.h>

class ScheduleData;
class Action;

class Schedule {
private:
    ScheduleData* schedule;
    int entryIndex;
    double entryEndTime;
    unsigned actionIndex;
public:
    Schedule(ScheduleData* schedule);

    void chooseEntryForTime(double currentTime);
    void startNextScheduleEntry();
    bool timeIsUp(double lastTime, double currentTime);
    Action* chooseNewAction();

    int getEntryAtTime(double currentTime) const;
    double getEndTime(int entryIndex) const;
    int nextEntry(int entryIndex) const;
    int currentEntry() const {return entryIndex;}
    int getId() const;
    std::string getName() const;
    unsigned getActionIndex()
    {
        return actionIndex;
    }
    void setActionIndex(unsigned index)
    {
        actionIndex = index;
    }
};


#endif //PARTIALORDERPLANNER_SCHEDULEINSTANCE_H
