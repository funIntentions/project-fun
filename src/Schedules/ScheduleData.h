//
// Created by Dan on 2/6/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULE_H
#define PARTIALORDERPLANNER_SCHEDULE_H

#include <vector>
#include <string>
#include <planner/PartialOrderPlanner.h>

class ScheduleEntry;
class Action;

class ScheduleData {
private:
    std::string name;
    int id;

public:
    std::vector<ScheduleEntry*> entries;

    ScheduleData(const std::string& name, int id) : name(name), id(id) {}
    ~ScheduleData() {}
    void addEntry(ScheduleEntry* entry) { entries.push_back(entry); }
    unsigned getNumEntries()
    {
        return entries.size();
    }

    const std::string& getName() const { return name; }
    int getId() const { return id; }
};


#endif //PARTIALORDERPLANNER_SCHEDULE_H
