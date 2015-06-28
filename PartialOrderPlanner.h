//
// Created by dan on 27/06/15.
//

#ifndef PARTIALORDERPLANNER_PARTIALORDERPLANNER_H
#define PARTIALORDERPLANNER_PARTIALORDERPLANNER_H


#include "PartialOrderPlan.h"

class PartialOrderPlanner
{
public:
    std::vector<PartialOrderPlan> finishedPlans;
    std::queue<PartialOrderPlan> partialPlans;
    std::vector<Operator> operators;

    PartialOrderPlanner(std::vector<Operator> domainOperators) : operators(domainOperators)
    {

    }

    std::vector<PartialOrderPlan> pop(Operator initialState, Operator endGoal)
    {
        PartialOrderPlan plan = makeInitialPlan(initialState, endGoal);
        partialPlans.push(plan);

        while (!partialPlans.empty())
        {
            plan = partialPlans.front();
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
                resolveThreats(partialPlans, plan);
            }

        }

        return finishedPlans;
    }

    void addGoals(PartialOrderPlan& plan, Operator newOperator)
    {
        std::vector<int>::iterator itr = newOperator.preconditions.begin();

        for (;itr != newOperator.preconditions.end();++itr)
        {
            plan.open.push(Goal(newOperator.id, *itr));
        }
    }

    void addTemporalLink(PartialOrderPlan& plan, long op, long target, bool isBefore)
    {
        TemporalLink temporalLinkBefore(target, isBefore);
        plan.ordering[op].push_back(temporalLinkBefore);

        TemporalLink temporalLinkAfterNew(op, !isBefore);
        plan.ordering[target].push_back(temporalLinkAfterNew);
    }

    // Checks to see if there are any operators existing that could threaten this link
    void findThreatsToCausalLink(PartialOrderPlan& plan, long newOperator, Goal causalLink)
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
            if (!BFSVertComesBeforeVert2(plan, potential->id, newOperator) && !BFSVertComesAfterVert2(plan, potential->id, causalLink.step)) // is within the danger interval
            {
                Threat* newThreat = new Threat(newOperator, causalLink, potential->id);
                plan.threats.push_back(*newThreat);
            }
        }
    }

    // Checks if newOperator is a threat to any existing links (needed for SE) - not addition I guess? :P
    void findThreatsCausedByOperator(PartialOrderPlan& plan, Operator newOperator)
    {
        std::vector<Threat> potentialThreats;

        for (auto subItr = newOperator.subtractedEffects.begin(); subItr != newOperator.subtractedEffects.end(); ++subItr)
        {
            for (long step = 0; step < plan.links.size(); ++step)
            {
                std::vector<Goal> stepsLinks = plan.links[0];
                for (auto linkItr = stepsLinks.begin(); linkItr != stepsLinks.end(); ++linkItr)
                {
                    if (linkItr->condition == (*subItr))
                    {
                        Threat* newThreat = new Threat(step, *linkItr, newOperator.id);
                        potentialThreats.push_back(*newThreat);
                    }
                }
            }
        }

        for (auto potential = potentialThreats.begin(); potential != potentialThreats.end(); ++potential)
        {
            if (!BFSVertComesBeforeVert2(plan, newOperator.id, potential->vulnerableOperator) && !BFSVertComesAfterVert2(plan, newOperator.id, potential->vulnerableLink.step)) // is within the danger interval
            {
                plan.threats.push_back(*potential);
            }
        }
    }

    void resolveThreats(std::queue<PartialOrderPlan>& partialPlans, PartialOrderPlan& plan)
    {
        if (!plan.threats.empty())
        {
            Threat threat(plan.threats.back());
            plan.threats.pop_back();

            PartialOrderPlan* newPartialPlan = new PartialOrderPlan(plan);
            promote(*newPartialPlan, threat);

            if (isConsistent(*newPartialPlan, threat))
                partialPlans.push(*newPartialPlan);

            newPartialPlan = new PartialOrderPlan(plan);
            demote(*newPartialPlan, threat);

            if (isConsistent(*newPartialPlan, threat))
                partialPlans.push(*newPartialPlan);
        }
    }

    void promote(PartialOrderPlan& plan, Threat threat)
    {
        addTemporalLink(plan, threat.operatorsThreat, threat.vulnerableLink.step, false);
    }

    void demote(PartialOrderPlan& plan, const Threat& threat)
    {
        addTemporalLink(plan, threat.operatorsThreat, threat.vulnerableOperator, true);
    }

    bool isConsistent(PartialOrderPlan& plan, const Threat& threat)
    {
        std::vector<TemporalLink> temporalLinks = plan.ordering[threat.operatorsThreat];

        for (auto link = temporalLinks.begin(); link != temporalLinks.end(); ++link)
        {
            std::vector<TemporalLink> targetsTemporalLinks = plan.ordering[link->targetOperator];
            for (auto targetLink = targetsTemporalLinks.begin(); targetLink != targetsTemporalLinks.end(); ++targetLink)
            {
                if (targetLink->targetOperator == threat.operatorsThreat && link->isBefore == targetLink->isBefore)
                {
                    return false;
                }
            }
        }

        return true;
    }

    PartialOrderPlan makeInitialPlan(Operator& initialState, Operator& endGoal)
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

        addGoals(plan, endGoal);

        return plan;
    }

    Goal selectOpenGoal(PartialOrderPlan& plan)
    {
        Goal goal = plan.open.front();
        plan.open.pop();

        return goal;
    }

    bool BFSVertComesBeforeVert2(PartialOrderPlan plan, long vert, long vert2)
    {
        long numVertices = plan.steps.size();
        bool* visited = new bool[numVertices];

        for (unsigned int i = 0; i < numVertices; ++i)
        {
            visited[i] = false;
        }

        std::list<long> queue;

        visited[vert] = true;
        queue.push_back(vert);

        std::vector<TemporalLink>::iterator itr;

        while(!queue.empty())
        {
            vert = queue.front();
            queue.pop_front();

            if (vert == vert2) return true;

            for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
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

    bool BFSVertComesAfterVert2(PartialOrderPlan plan, long vert, long vert2)
    {
        long numVertices = plan.steps.size();
        bool* visited = new bool[numVertices];

        for (unsigned int i = 0; i < numVertices; ++i)
        {
            visited[i] = false;
        }

        std::list<long> queue;

        visited[vert] = true;
        queue.push_back(vert);

        std::vector<TemporalLink>::iterator itr;

        while(!queue.empty())
        {
            vert = queue.front();
            queue.pop_front();

            if (vert == vert2) return true;

            for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
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

    bool alreadyUsed(PartialOrderPlan plan, Operator op) // TODO: find a way so that I don't have to limit the number of potencial ops
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
    void doSimpleEstablishment(PartialOrderPlan& partialPlan, Operator& chosen, Goal goal)
    {
        if (!(goal.step == partialPlan.end || goal.step == partialPlan.start || chosen.id == partialPlan.end || chosen.id == partialPlan.start)) // TODO: have a better way to ensure temporal links are properly added and there aren't ugly checks like this...
            addTemporalLink(partialPlan, goal.step, chosen.id, false);
        partialPlan.links[chosen.id].push_back(goal);
    }

    // add a new operator to satisfy a goal
    void doOperatorAddition(PartialOrderPlan& plan,
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

        addGoals(plan, newOperator);

        plan.steps[newOperator.id] = newOperator;
    }

    void chooseOperator(PartialOrderPlan& plan, Goal goal)
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
            if (!BFSVertComesAfterVert2(plan, chosenItr->id, goal.step))
            {
                PartialOrderPlan* partialPlan = new PartialOrderPlan(plan);
                doSimpleEstablishment(*partialPlan, *chosenItr, goal);
                findThreatsToCausalLink(*partialPlan, chosenItr->id, goal); // TODO: make this nicer... eliminate the repetition
                findThreatsCausedByOperator(*partialPlan, partialPlan->steps[goal.step]);
                partialPlans.push(*partialPlan);
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
                    PartialOrderPlan* partialPlan = new PartialOrderPlan(plan);
                    doOperatorAddition(*partialPlan, option, goal);
                    findThreatsToCausalLink(*partialPlan, option.id, goal); // TODO: here's that repetition that I mentioned up there ^
                    findThreatsCausedByOperator(*partialPlan, partialPlan->steps[goal.step]); // TODO: do I need this here? I know I do for SE but A as well?
                    partialPlans.push(*partialPlan);
                }
            }
        }
    }

};


#endif //PARTIALORDERPLANNER_PARTIALORDERPLANNER_H
