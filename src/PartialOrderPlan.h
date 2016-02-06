//
// Created by dan on 27/06/15.
//

#ifndef PARTIALORDERPLANNER_PARTIALORDERPLAN_H
#define PARTIALORDERPLANNER_PARTIALORDERPLAN_H

#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <list>
#include <queue>
#include <string>

// TODO: rename Operator to step
struct Operator
{
    std::string name;
    long id;
    std::vector<int> preconditions;
    std::vector<int> addedEffects;
    std::vector<int> subtractedEffects;

    Operator(std::string op = "") : name(op)
    {
        id = -1;
    }

    bool operator==(const Operator& other) const
    {
        return (name.compare(other.name) == 0)
                && preconditions == other.preconditions
                && addedEffects == other.addedEffects
                && subtractedEffects == other.subtractedEffects;
    }
};

namespace std {
    template <>
    struct hash<Operator>
    {
        std::size_t operator()(const Operator& anOperator) const
        {
            std::size_t seed = 0;
            for(auto& pre : anOperator.preconditions) {
                seed ^= std::hash<int>()(pre) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            for(auto& effect : anOperator.addedEffects) {
                seed ^= std::hash<int>()(effect) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            for(auto& effect : anOperator.subtractedEffects) {
                seed ^= std::hash<int>()(effect) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            return (std::hash<std::string>()(anOperator.name)) ^ seed;
        }
    };
}

struct Goal
{
    int condition;
    long step; // the step that the condition is for

    Goal(long owner, int precondition) : condition(precondition), step(owner) {}
};

struct TemporalLink
{

    long targetOperator;
    bool isBefore; // if step1 < step2 (if step1 must happen before step2)

    TemporalLink(long target, bool beforeTarget) :
            targetOperator(target),
            isBefore(beforeTarget) {}

};

struct Threat
{
    long vulnerableOperator;
    Goal vulnerableLink;
    long operatorsThreat;

    Threat (long vulnerableOp, Goal causalLink, long threat)
            : vulnerableOperator(vulnerableOp), vulnerableLink(causalLink), operatorsThreat(threat) {}
};

struct PartialOrderPlan
{
    std::unordered_map<long, Operator> steps; // TODO: check if unordered_map is the right map for the job.
    std::unordered_map<Operator, int> stepsDone;
    std::vector< std::vector<TemporalLink> > ordering;
    std::vector< std::vector<Goal> > links;
    std::vector<Threat> threats;
    std::queue<Goal> open;
    long start;
    long end;

    PartialOrderPlan()
    {

    }
};

struct ComparePlans
{
    bool operator()(PartialOrderPlan const &plan1, PartialOrderPlan const &plan2)
    {
        return (plan1.steps.size() > plan2.steps.size());
    }
};


#endif //PARTIALORDERPLANNER_PARTIALORDERPLAN_H
