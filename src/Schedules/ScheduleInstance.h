//
// Created by Dan on 2/7/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULEINSTANCE_H
#define PARTIALORDERPLANNER_SCHEDULEINSTANCE_H

#include <WorldState.h>

class Schedule;
class ActionInstance;

class ScheduleInstance {
private:
    Schedule* schedule;
    int entryIndex;
    double entryEndTime;
public:
    ScheduleInstance(Schedule* schedule);

    void chooseEntryForTime(double currentTime);
    void startNextScheduleEntry();
    bool timeIsUp(double lastTime, double currentTime);
    ActionInstance* chooseNewAction(WorldState& worldState);

    int getId() const;
};


#endif //PARTIALORDERPLANNER_SCHEDULEINSTANCE_H
