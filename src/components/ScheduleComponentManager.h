//
// Created by Dan on 2/13/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H

#include <functional>
#include <memory>
#include <PartialOrderPlanner.h>
#include "ComponentManager.h"
#include "PartialOrderPlan.h"
#include "LocationComponentManager.h"
#include "WorldState.h"
#include "PositionComponentManager.h"
#include "CharacterComponentManager.h"

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
        std::vector<WorldState> state;
        std::vector<std::vector<ActionInstance*>> queuedActions;
        std::vector<ScheduleInstance*> currentSchedule;
    };

    InstanceData _data;

    std::unordered_map<int, Schedule*> schedules;
    std::unordered_map<int, ScheduleEntry*> scheduleEntryTemplates;
    std::unordered_map<int, Action*> actions;
    std::vector<std::shared_ptr<Operator>> operators;
    std::unordered_map<std::string, int> actionNameToIdMap;
    std::unordered_map<std::string, int> entryNameToIdMap;
    std::unordered_map<std::string, int> scheduleNameToIdMap;

    std::unordered_map<std::string, OperatorCallbackFunction> operatorCallbackFunctionMap;
    std::shared_ptr<ActionManager> _actionManager;
    std::shared_ptr<CharacterComponentManager> _characterComponentManager;
    std::shared_ptr<PositionComponentManager> _positionComponentManager;
    PartialOrderPlanner planner;

    double time;

    const std::string SIMPLE_SCHEDULE_ENTRY = "simple";
    const std::string SEQUENCE_SCHEDULE_ENTRY = "sequence";

public:
    ScheduleComponentManager(std::shared_ptr<ActionManager> actionManager, std::shared_ptr<CharacterComponentManager> characterComponentManager, std::shared_ptr<PositionComponentManager> positionComponentManager);

    void readActions(std::shared_ptr<ActionManager> actionManager);

    void readSchedules();

    ScheduleEntry* scheduleEntryFactory(const std::string& name, const std::string& type, double startTime);

    ~ScheduleComponentManager();

    void registerForAction(std::string action, OperatorCallbackFunction function);

    void usePlanner(Entity entity, std::vector<int> preconditions);

    void updateState(Entity entity, std::vector<int> effects);

    std::vector<int> getState(Entity entity);

    bool preconditionsMet(Entity entity, std::vector<int> preconditions);

    std::vector<int> runSchedules(double deltaTime);

    void spawnComponent(Entity entity, std::string scheduleName, double currentTime);

    void destroy(unsigned i);
};


#endif //PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H
