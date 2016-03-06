//
// Created by Dan on 2/13/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H

#include <functional>
#include <memory>
#include "ComponentManager.h"
#include "PartialOrderPlan.h"
#include "CharacterComponentManagers.h"
#include "WorldState.h"

class ActionInstance;
class ScheduleInstance;
class Schedule;
class ScheduleEntry;
class Action;

typedef std::function<std::vector<Operator>(Action, Entity)> OperatorCallbackFunction;

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

    std::unordered_map<std::string, OperatorCallbackFunction> operatorCallbackFunctionMap;

    WorldState worldState; // TODO: Temp
    double time;

    const std::string SIMPLE_SCHEDULE_ENTRY = "simple";
    const std::string PLANNER_SCHEDULE_ENTRY = "planner";

public:

    ScheduleComponentManager();

    void readActions();

    void readSchedules();

    ScheduleEntry* scheduleEntryFactory(const std::string& name, const std::string& type, double startTime);

    ~ScheduleComponentManager();

    void registerForAction(std::string action, OperatorCallbackFunction function);

    void runSchedules(double deltaTime, ActionManager& actionManager); // TODO: decouple action manager

    void updateWorldState(std::vector<int> addedEffects, ActionManager& actionManager);

    void spawnComponent(Entity entity, std::string scheduleName, double currentTime);

    void destroy(unsigned i);
};


#endif //PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H
