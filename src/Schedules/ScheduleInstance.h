//
// Created by Dan on 2/7/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULEINSTANCE_H
#define PARTIALORDERPLANNER_SCHEDULEINSTANCE_H

#include <WorldState.h>
#include <components/ScheduleComponentManager.h>

class Schedule;
class ActionInstance;

class ScheduleInstance {
private:
    Schedule* schedule;
    int entryIndex;
    double entryEndTime;
    unsigned actionIndex;
public:
    ScheduleInstance(Schedule* schedule);

    void setupEntries(std::unordered_map<std::string, OperatorCallbackFunction>& operatorCallbacks, Entity entity);

    void chooseEntryForTime(double currentTime, WorldState& worldState);
    void startNextScheduleEntry(WorldState& worldState);
    bool timeIsUp(double lastTime, double currentTime);
    ActionInstance* chooseNewAction();

    int getEntryAtTime(double currentTime) const;
    double getEndTime(int entryIndex) const;
    int nextEntry(int entryIndex) const;
    int getId() const;
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
