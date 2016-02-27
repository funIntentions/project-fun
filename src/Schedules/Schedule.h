//
// Created by Dan on 2/6/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULE_H
#define PARTIALORDERPLANNER_SCHEDULE_H

#include <vector>
#include <string>
#include <PartialOrderPlanner.h>
#include <WorldState.h>

class ScheduleEntry;
class ActionInstance;

class Schedule {
private:
    std::string name;
    int id;

public:
    std::vector<ScheduleEntry*> entryTemplates;
    Schedule(const std::string& name, int id) : name(name), id(id) {}
    ~Schedule() {}
    void addEntry(ScheduleEntry* entry) { entryTemplates.push_back(entry); }
    /*int getEntryAtTime(double currentTime) const;

    int nextEntry(int entryIndex) const;
    ActionInstance* chooseNewAction(int entryIndex, WorldState& worldState);
    double getEndTime(int entryIndex) const;*/

    const std::string& getName() const { return name; }
    int getId() const { return id; }
};


#endif //PARTIALORDERPLANNER_SCHEDULE_H
