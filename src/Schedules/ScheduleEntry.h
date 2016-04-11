//
// Created by Dan on 2/6/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULEENTRY_H
#define PARTIALORDERPLANNER_SCHEDULEENTRY_H

#include <string>
#include <vector>
#include <set>
#include <PartialOrderPlanner.h>
#include <components/ScheduleComponentManager.h>

class Action;
class ActionInstance;

class ScheduleEntry {
protected:
    std::string name;
    int id;
    double startTime;
public:
    ScheduleEntry(const std::string& name, int id, double startTime)
        : name(name), id(id), startTime(startTime) {}
    virtual ~ScheduleEntry() {}

    virtual void addAction(Action* action) = 0;
    virtual ActionInstance* chooseNewAction(ScheduleInstance* scheduleInstance) = 0;
    virtual ScheduleEntry* clone(const std::string& name, const int& id, double startTime) = 0;

    const std::string& getName() const { return name; }
    int getId() const { return id; }
    double getStartTime() const { return startTime; }
};

class SimpleScheduleEntry : public ScheduleEntry
{
private:
    Action* action;
    Operator op;
public:
    SimpleScheduleEntry(const std::string& name, int id, double startTime)
            : ScheduleEntry(name, id, startTime), action(nullptr) {}

    virtual void addAction(Action* action);
    virtual ActionInstance* chooseNewAction(ScheduleInstance* scheduleInstance);
    virtual ScheduleEntry* clone(const std::string& name, const int& id, double startTime);
};

class SequenceScheduleEntry : public ScheduleEntry
{
private:
    std::vector<Action*> actions;
public:
    SequenceScheduleEntry(const std::string& name, int id, double startTime)
            : ScheduleEntry(name, id, startTime)
    {}

    virtual void addAction(Action* action);
    virtual ActionInstance* chooseNewAction(ScheduleInstance* scheduleInstance);
    virtual ScheduleEntry* clone(const std::string& name, const int& id, double startTime);

};

#endif //PARTIALORDERPLANNER_SCHEDULEENTRY_H
