//
// Created by Dan on 2/6/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULEENTRY_H
#define PARTIALORDERPLANNER_SCHEDULEENTRY_H

#include <string>
#include <vector>
#include <set>
#include <PartialOrderPlanner.h>
#include <WorldState.h>
#include <ScheduleComponentManager.h>

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
    virtual ScheduleEntry* setupEntry(std::unordered_map<std::string, OperatorCallbackFunction>& operatorCallbacks, Entity entity) = 0;
    virtual void startEntry(WorldState& worldState) = 0;
    virtual void endEntry() = 0;
    virtual ActionInstance* chooseNewAction(WorldState& worldState) = 0;
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
    virtual ScheduleEntry* setupEntry(std::unordered_map<std::string, OperatorCallbackFunction>& operatorCallbacks, Entity entity);
    virtual void startEntry(WorldState& worldState) {};
    virtual void endEntry() {};
    virtual ActionInstance* chooseNewAction(WorldState& worldState);
    virtual ScheduleEntry* clone(const std::string& name, const int& id, double startTime);
};


class PlannerScheduleEntry : public ScheduleEntry
{
private:
    std::unordered_map<std::string, Action*> actions;
    std::vector<Operator> operators;
    std::vector<Operator> plan;
    std::set<int> goals;
    PartialOrderPlanner planner;
    int actionIndex;
    Operator idle;
public:
    PlannerScheduleEntry(const std::string& name, int id, double startTime)
            : ScheduleEntry(name, id, startTime), actionIndex(-1)
    {
        idle.name = "idle";
    }

    virtual void addAction(Action* action);
    virtual ScheduleEntry* setupEntry(std::unordered_map<std::string, OperatorCallbackFunction>& operatorCallbacks, Entity entity);
    virtual void startEntry(WorldState& worldState);
    virtual void endEntry();
    virtual ActionInstance* chooseNewAction(WorldState& worldState);
    virtual ScheduleEntry* clone(const std::string& name, const int& id, double startTime);
};

#endif //PARTIALORDERPLANNER_SCHEDULEENTRY_H
