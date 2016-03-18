//
// Created by Dan on 2/6/2016.
//

#include "util/Extra.h"
#include "ScheduleEntry.h"

// Simple Schedule Entry

void SimpleScheduleEntry::addAction(Action* a)
{
    action = a;
}

ActionInstance* SimpleScheduleEntry::chooseNewAction(WorldState& worldState)
{
    return action->createActionInstance(&op);
}

ScheduleEntry* SimpleScheduleEntry::setupEntry(std::unordered_map<std::string, OperatorCallbackFunction>& operatorCallbacks, Entity entity)
{
    SimpleScheduleEntry* entry = new SimpleScheduleEntry(name, id, startTime);
    entry->op.name = action->getName(); // TODO: Figure out how non-planner schedules will work

    return entry;
}

ScheduleEntry* SimpleScheduleEntry::clone(const std::string& name, const int& id, double startTime)
{
    SimpleScheduleEntry* simpleScheduleEntry = new SimpleScheduleEntry(name, id, startTime);
    simpleScheduleEntry->action = action;
    return simpleScheduleEntry;
}

// Planner Schedule Entry

void PlannerScheduleEntry::addAction(Action* action)
{
    actions.insert({action->getName(), action});
}

ScheduleEntry* PlannerScheduleEntry::setupEntry(std::unordered_map<std::string, OperatorCallbackFunction>& operatorCallbacks, Entity entity)
{
    PlannerScheduleEntry* entry = (PlannerScheduleEntry*) this->clone(name, id, startTime);

    for (auto actionItr : actions)
    {
        auto opItr = operatorCallbacks.find(actionItr.second->getName());
        if (opItr != operatorCallbacks.end())
        {
            std::vector<Operator> newOperators = opItr->second(*actionItr.second, entity);

            for (auto op : newOperators)
            {
                for (int effect : op.addedEffects)
                    entry->goals.insert(effect);

                entry->operators.push_back(op);
            }
        }
    }

    return entry;
}

void PlannerScheduleEntry::startEntry(WorldState &worldState)
{
    int randGoal = rand() % goals.size();
    int selectedGoal = *std::next(goals.begin(), randGoal);
    std::cout << "Selected Goal: " << selectedGoal << std::endl;

    Operator start;
    start.addedEffects = worldState.state;
    Operator end;
    end.preconditions.push_back(selectedGoal); // 6 == final location
    std::vector<PartialOrderPlan> plans = planner.findPartialOrderPlan(start, end, operators);

    if (plans.size() > 0)
    {
        std::vector<unsigned> totalOrderPlan = topologicalSort(plans[0], plans[0].start);

        for (auto step = totalOrderPlan.begin() + 1; step != totalOrderPlan.end() - 1; ++step)
        {
            std::unordered_map<unsigned, Operator>::iterator op = plans[0].steps.find(*step);
            if (op != plans[0].steps.end())
                plan.insert(plan.begin(), op->second);
        }
    }

    // add an idle action for cases when a plan cannot be made or when the plan is completed early
    plan.push_back(idle);
}

void PlannerScheduleEntry::endEntry()
{
    actionIndex = -1;
    plan.clear();
}

ActionInstance* PlannerScheduleEntry::chooseNewAction(WorldState& worldState)
{
    ++actionIndex;
    int planSize = plan.size();

    if (actionIndex >= planSize)
        actionIndex = planSize - 1; // TODO: start new entry once this one is finished.

    Operator* op = &plan[actionIndex];

    auto actionItr = actions.find(op->name);
    Action* action = actionItr->second; // TODO: handle cases when the matching action is not found.. should never happen?

    return action->createActionInstance(op);
}

ScheduleEntry* PlannerScheduleEntry::clone(const std::string& name, const int& id, double startTime)
{
    PlannerScheduleEntry* entry = new PlannerScheduleEntry(name, id, startTime);

    entry->actions.reserve(actions.size());

    for (auto action : actions)
    {
        entry->actions.insert(action);
    }

    return entry;
}
