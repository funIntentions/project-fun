//
// Created by Dan on 2/13/2016.
//

#ifndef PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H

#include <functional>
#include <memory>
#include <PartialOrderPlanner.h>
#include <demo/StoryLogger.h>
#include "ComponentManager.h"
#include "PartialOrderPlan.h"
#include "LocationComponentManager.h"
#include "PositionComponentManager.h"
#include "OpinionComponentManager.h"
#include "AttributeComponentManager.h"
#include "OwnershipComponentManager.h"

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
    std::shared_ptr<TypeComponentManager> _typeComponentManager;
    std::shared_ptr<OpinionComponentManager> _opinionComponentManager;
    std::shared_ptr<PositionComponentManager> _positionComponentManager;
    std::shared_ptr<OwnershipComponentManager> _ownershipComponentManager;
    std::shared_ptr<AttributeComponentManager> _attributeComponentManager;
    PartialOrderPlanner planner;

    double time;
    double speed;

    const std::string SIMPLE_SCHEDULE_ENTRY = "simple";
    const std::string SEQUENCE_SCHEDULE_ENTRY = "sequence";

public:
    ScheduleComponentManager(std::shared_ptr<ActionManager> actionManager,
                             std::shared_ptr<TypeComponentManager> typeComponentManager,
                             std::shared_ptr<OpinionComponentManager> opinionComponentManager,
                             std::shared_ptr<PositionComponentManager> positionComponentManager,
                             std::shared_ptr<OwnershipComponentManager> ownershipComponentManager,
                             std::shared_ptr<AttributeComponentManager> attributeComponentManager);

    void readActions(std::shared_ptr<ActionManager> actionManager);

    void readSchedules();

    ScheduleEntry* scheduleEntryFactory(const std::string& name, const std::string& type, double startTime);

    ~ScheduleComponentManager();

    void registerForAction(std::string action, OperatorCallbackFunction function);

    void usePlanner(Entity entity, std::vector<int> preconditions);

    void updateState(ActionInstance* action, StoryLogger& storyLogger);

    std::vector<int> getState(Entity entity);

    void mapParameters(Entity entity, ActionInstance* action);

    bool preconditionsMet(ActionInstance* action);

    void runSchedules(double deltaTime, StoryLogger& storyLogger);

    void spawnComponent(Entity entity, std::string scheduleName, double currentTime, StoryLogger& storyLogger);

    double getTimeOfDay() {return time;}

    void destroy(unsigned i);
};


#endif //PARTIALORDERPLANNER_SCHEDULECOMPONENTMANAGER_H
