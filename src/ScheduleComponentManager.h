//
// Created by Dan on 2/13/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H


#include "ComponentManager.h"

class ActionInstance;
class ScheduleInstance;
class Schedule;
class ScheduleEntry;
class Action;

class ScheduleComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<ActionInstance*> currentAction;
        std::vector<ScheduleInstance*> currentSchedule;
    };

    InstanceData _data;

    std::unordered_map<int, Schedule*> schedules;
    std::unordered_map<int, ScheduleEntry*> scheduleEntryTemplates;
    std::unordered_map<int, Action*> actions;
    std::unordered_map<std::string, int> actionNameToIdMap;
    std::unordered_map<std::string, int> entryNameToIdMap;
    std::unordered_map<std::string, int> scheduleNameToIdMap;
    std::vector<Predicate> tempWorldState;
    const std::string SIMPLE_SCHEDULE_ENTRY = "simple";

public:

    ScheduleComponentManager();

    void readActions();

    void readSchedules();

    ScheduleEntry* scheduleEntryFactory(const std::string& name, const std::string& type, double startTime);

    ~ScheduleComponentManager();

    void runSchedules(double lastTime, double currentTime, double deltaTime);

    void spawnComponent(Entity entity, std::string scheduleName, double currentTime);

    void destroy(unsigned i);
};


#endif //PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H
