//
// Created by dan on 27/06/15.
//

#include "PartialOrderPlanner.h"

std::vector<PartialOrderPlan> PartialOrderPlanner::findPartialOrderPlan(Operator initialState, Operator endGoal)
{
        PartialOrderPlan plan = makeInitialPlan(initialState, endGoal);
        partialPlans.push(plan);

        while (!partialPlans.empty())
        {
            plan = partialPlans.top();
            partialPlans.pop();

            if (plan.threats.empty())
            {
                if (plan.open.empty())
                {
                    finishedPlans.push_back(plan); // no threats and no open goals left - success!
                    break;
                }
                else
                {
                    Goal goal = selectOpenGoal(plan);
                    chooseOperator(plan, goal);
                }
            }
            else
            {
                resolveThreats(plan);
            }
        }

        return finishedPlans;
    }

void PartialOrderPlanner::addOpenGoals(PartialOrderPlan& plan, const Operator& newOperator)
{
    for (auto itr = newOperator.preconditions.begin() ;itr != newOperator.preconditions.end();++itr)
    {
        plan.open.push(Goal(newOperator.id, *itr));
    }
}

void PartialOrderPlanner::addTemporalLink(PartialOrderPlan& plan, long op, long target, bool isBefore)
{
        TemporalLink temporalLinkBefore(target, isBefore);
        plan.ordering[op].push_back(temporalLinkBefore);

        TemporalLink temporalLinkAfterNew(op, !isBefore);
        plan.ordering[target].push_back(temporalLinkAfterNew);
    }

// Checks to see if there are any operators existing that could threaten this link
void PartialOrderPlanner::findThreatsToCausalLink(PartialOrderPlan& plan, long newOperator, Goal causalLink)
{
        std::vector<Operator> potentialThreats;

        for (auto stepItr = plan.steps.begin(); stepItr != plan.steps.end(); ++stepItr)
        {
            for (auto condItr = stepItr->second.subtractedEffects.begin(); condItr != stepItr->second.subtractedEffects.end(); ++condItr)
            {
                if (*condItr == causalLink.condition)
                {
                    potentialThreats.push_back(stepItr->second);
                }
            }
        }

        for (auto potential = potentialThreats.begin(); potential != potentialThreats.end(); ++potential)
        {
            if (!step1IsBeforeStep2(plan, potential->id, newOperator) && !step1IsAfterStep2(plan, potential->id, causalLink.step)) // is within the danger interval
            {
                Threat newThreat(newOperator, causalLink, potential->id);
                plan.threats.push_back(newThreat);
            }
        }
    }

// Checks if newOperator is a threat to any existing links (needed for SE) - not addition I guess? :P
void PartialOrderPlanner::findThreatsCausedByOperator(PartialOrderPlan& plan, const Operator& newOperator)
{
        std::vector<Threat> potentialThreats;

        for (auto subItr = newOperator.subtractedEffects.begin(); subItr != newOperator.subtractedEffects.end(); ++subItr)
        {
            for (long step = 0; step < plan.links.size(); ++step)
            {
                std::vector<Goal> stepsLinks = plan.links[step];
                for (auto linkItr = stepsLinks.begin(); linkItr != stepsLinks.end(); ++linkItr)
                {
                    if (linkItr->condition == (*subItr))
                    {
                        Threat newThreat(step, *linkItr, newOperator.id);
                        potentialThreats.push_back(newThreat);
                    }
                }
            }
        }

        for (auto potential = potentialThreats.begin(); potential != potentialThreats.end(); ++potential)
        {
            if (!step1IsBeforeStep2(plan, newOperator.id, potential->vulnerableOperator) && !step1IsAfterStep2(plan, newOperator.id, potential->vulnerableLink.step)) // is within the danger interval
            {
                plan.threats.push_back(*potential);
            }
        }
    }

void PartialOrderPlanner::findAllThreats(PartialOrderPlan& plan)
{
        /*std::vector<Threat> potentialThreats;

        for (auto operatorItr = plan.steps.begin(); operatorItr != plan.steps.end(); ++operatorItr)
        {
            Operator step = operatorItr->second;

            for (auto effectsItr = step.subtractedEffects.begin(); effectsItr != step.subtractedEffects.end(); ++effectsItr)
            {

            }
        }*/
}

void PartialOrderPlanner::resolveThreats(PartialOrderPlan& plan)
{
        if (!plan.threats.empty())
        {
            Threat threat(plan.threats.back());
            plan.threats.pop_back();

            PartialOrderPlan promotedPartialPlan(plan);
            promote(promotedPartialPlan, threat);

            if (!isCyclicPlan(promotedPartialPlan))
                partialPlans.push(promotedPartialPlan);

            PartialOrderPlan demotedPartialPlan(plan);
            demote(demotedPartialPlan, threat);

            if (!isCyclicPlan(demotedPartialPlan))
                partialPlans.push(demotedPartialPlan);
        }
    }

void PartialOrderPlanner::promote(PartialOrderPlan& plan, const Threat& threat)
{
        addTemporalLink(plan, threat.operatorsThreat, threat.vulnerableLink.step, false);
    }

void PartialOrderPlanner::demote(PartialOrderPlan& plan, const Threat& threat)
{
        addTemporalLink(plan, threat.operatorsThreat, threat.vulnerableOperator, true);
    }

bool PartialOrderPlanner::isCyclicPlan(const PartialOrderPlan& plan)
{
        long numVertices = plan.ordering.size();
        long* parent = new long[numVertices];

        for (unsigned int i = 0; i < numVertices; ++i)
        {
            parent[i] = -1;
        }

        std::list<long> stack;

        parent[plan.start] = -1;
        stack.push_back(plan.start);

        std::vector<TemporalLink>::const_iterator itr;

        while(!stack.empty())
        {
            long step = stack.back();

            for (itr = plan.ordering[step].begin(); itr != plan.ordering[step].end(); ++itr)
            {
                if (itr->isBefore)
                {
                    if (parent[itr->targetOperator] < 0)
                    {
                        parent[itr->targetOperator] = step;
                        stack.push_back(itr->targetOperator);
                        break;
                    }
                    else
                    {
                        for (auto ancestor = stack.begin(); ancestor != stack.end(); ++ancestor)
                        {
                            if (*ancestor == itr->targetOperator)
                            {
                                return true; // has cycle
                            }
                        }
                    }
                }
            }

            if (itr == plan.ordering[step].end())
            {
                stack.pop_back();
            }
        }

        return false;
    }

PartialOrderPlan PartialOrderPlanner::makeInitialPlan(Operator& initialState, Operator& endGoal)
{
        PartialOrderPlan plan;

        initialState.id = plan.ordering.size();
        plan.links.push_back({});
        plan.ordering.push_back({});

        endGoal.id = plan.ordering.size();
        plan.links.push_back({});
        plan.ordering.push_back({});

        addTemporalLink(plan, initialState.id, endGoal.id, true);

        plan.start = initialState.id;
        plan.end = endGoal.id;

        plan.steps[initialState.id] = initialState;
        plan.steps[endGoal.id] = endGoal;

        addOpenGoals(plan, endGoal);

        return plan;
    }

Goal PartialOrderPlanner::selectOpenGoal(PartialOrderPlan& plan)
{
        Goal goal = plan.open.front();
        plan.open.pop();

        return goal;
    }

bool PartialOrderPlanner::step1IsBeforeStep2(const PartialOrderPlan& plan, long step1, long step2)
{
        long numVertices = plan.steps.size();
        bool* visited = new bool[numVertices];

        for (unsigned int i = 0; i < numVertices; ++i)
        {
            visited[i] = false;
        }

        std::list<long> queue;

        visited[step1] = true;
        queue.push_back(step1);

        std::vector<TemporalLink>::const_iterator itr;

        while(!queue.empty())
        {
            long step = queue.front();
            queue.pop_front();

            if (step == step2) return true;

            for (itr = plan.ordering[step].begin(); itr != plan.ordering[step].end(); ++itr)
            {
                if (!visited[itr->targetOperator])
                {
                    if (itr->isBefore)
                    {
                        visited[itr->targetOperator] = true;
                        queue.push_back(itr->targetOperator);
                    }
                }
            }
        }

        return false;
}

bool PartialOrderPlanner::step1IsAfterStep2(const PartialOrderPlan& plan, long step1, long step2)
{
        long numVertices = plan.steps.size();
        bool* visited = new bool[numVertices];

        for (unsigned int i = 0; i < numVertices; ++i)
        {
            visited[i] = false;
        }

        std::list<long> queue;

        visited[step1] = true;
        queue.push_back(step1);

        std::vector<TemporalLink>::const_iterator itr;

        while(!queue.empty())
        {
            long step = queue.front();
            queue.pop_front();

            if (step == step2) return true;

            for (itr = plan.ordering[step].begin(); itr != plan.ordering[step].end(); ++itr)
            {
                if (!visited[itr->targetOperator])
                {
                    if (!itr->isBefore)
                    {
                        visited[itr->targetOperator] = true;
                        queue.push_back(itr->targetOperator);
                    }
                }
            }
        }

        return false;
    }

bool PartialOrderPlanner::alreadyUsed(const PartialOrderPlan& plan, const Operator& op) // TODO: find a way so that I don't have to limit the number of potencial ops (& the string compare)
{
        for (auto itr = plan.steps.begin(); itr != plan.steps.end(); ++itr)
        {
            if (itr->second.name.compare(op.name) == 0)
            {
                return true;
            }
        }

        return false;
    }

// Simple establishment/use existing operator to satisfy goal
void PartialOrderPlanner::doSimpleEstablishment(PartialOrderPlan& partialPlan, Operator& chosen, Goal goal)
{
        if (!(goal.step == partialPlan.end || goal.step == partialPlan.start || chosen.id == partialPlan.end || chosen.id == partialPlan.start)) // TODO: have a better way to ensure temporal links are properly added and there aren't ugly checks like this...
            addTemporalLink(partialPlan, goal.step, chosen.id, false);
        partialPlan.links[chosen.id].push_back(goal);
    }

// add a new operator to satisfy a goal
void PartialOrderPlanner::doOperatorAddition(PartialOrderPlan& plan,
                      Operator& newOperator,
                      Goal goal)
{
        newOperator.id = plan.ordering.size();

        plan.ordering.push_back({});
        addTemporalLink(plan, newOperator.id, plan.start, false);
        addTemporalLink(plan, newOperator.id, plan.end, true);

        if (goal.step != plan.end)
            addTemporalLink(plan, newOperator.id, goal.step, true);

        plan.links.push_back({goal});

        addOpenGoals(plan, newOperator);

        plan.steps[newOperator.id] = newOperator;
    }

void PartialOrderPlanner::chooseOperator(PartialOrderPlan& plan, Goal goal)
{
        std::vector<Operator> chosen;

        for (auto stepItr = plan.steps.begin(); stepItr != plan.steps.end(); ++stepItr)
        {
            for (auto condItr = stepItr->second.addedEffects.begin(); condItr != stepItr->second.addedEffects.end(); ++condItr)
            {
                if (*condItr == goal.condition)
                {
                    chosen.push_back(stepItr->second);
                }
            }
        }

        for (auto chosenItr = chosen.begin(); chosenItr != chosen.end(); ++chosenItr)
        {
            // check to see if the chosen operator comes before the one in question
            if (!step1IsAfterStep2(plan, chosenItr->id, goal.step))
            {
                PartialOrderPlan partialPlan(plan);
                doSimpleEstablishment(partialPlan, *chosenItr, goal);
                findThreatsToCausalLink(partialPlan, chosenItr->id, goal); // TODO: make this nicer... eliminate the repetition
                findThreatsCausedByOperator(partialPlan, partialPlan.steps[goal.step]);
                partialPlans.push(partialPlan);
            }
        }

        for (Operator option : operators)
        {

            if (alreadyUsed(plan, option))
            {
                continue;
            }

            for (auto itr = option.addedEffects.begin(); itr != option.addedEffects.end(); ++itr)
            {
                if (*itr == goal.condition)
                {
                    PartialOrderPlan partialPlan(plan);
                    doOperatorAddition(partialPlan, option, goal);
                    findThreatsToCausalLink(partialPlan, option.id, goal); // TODO: here's that repetition that I mentioned up there ^
                    findThreatsCausedByOperator(partialPlan, partialPlan.steps[goal.step]); // TODO: do I need this here? I know I do for SE but A as well?
                    partialPlans.push(partialPlan);
                }
            }
        }
    }

