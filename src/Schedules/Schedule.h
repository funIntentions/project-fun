//
// Created by Dan on 2/6/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULE_H
#define PARTIALORDERPLANNER_SCHEDULE_H

#include <vector>
#include <string>

class ScheduleEntry;
class ActionInstance;

class Schedule {
private:
    std::string name;
    int id;
    std::vector<ScheduleEntry*> scheduleEntries;

public:
    Schedule(const std::string& name, int id) : name(name), id(id) {}
    ~Schedule() {}
    void addEntry(ScheduleEntry* entry) { scheduleEntries.push_back(entry); }
    int getEntryAtTime(double currentTime) const;

    int nextEntry(int entryIndex) const;
    ActionInstance* chooseNewAction(int entryIndex);
    double getEndTime(int entryIndex) const;

    const std::string& getName() const { return name; }
    int getId() const { return id; }
};


#endif //PARTIALORDERPLANNER_SCHEDULE_H
