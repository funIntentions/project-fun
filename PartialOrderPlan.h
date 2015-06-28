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

static const long NONE = -1;

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
};

struct Goal
{
    int condition;
    long step; // the step that the condition is for
    Goal(long owner, int precondition) : condition(precondition), step(owner) {}
};

struct TemporalLink
{
    TemporalLink(long target, bool beforeTarget) :
            targetOperator(target),
            isBefore(beforeTarget) {}

    //Operator targetOperator;
    long targetOperator;
    bool isBefore; // if step1 < step2 (if step1 must happen before step2)
};

struct CausalLink
{
    CausalLink(long target, int satisfiedCondition) :
            targetOperator(target),
            condition(satisfiedCondition) {}

    //Operator targetOperator;
    long targetOperator;
    int condition;
};

struct Threat
{
    long vulnerableOperator;
    CausalLink vulnerableLink;
    long operatorsThreat;

    Threat (long vulnerableOp, CausalLink causalLink, long threat)
            : vulnerableOperator(vulnerableOp), vulnerableLink(causalLink), operatorsThreat(threat) {}
};

struct PartialOrderPlan
{
    std::unordered_map<long, Operator> steps; // TODO: check if unordered_map is the right map for the job.
    std::vector< std::vector<TemporalLink> > ordering;
    std::vector< std::vector<CausalLink> > links;
    std::vector<Threat> threats;
    std::queue<Goal> open;
    long start;
    long end;

};


#endif //PARTIALORDERPLANNER_PARTIALORDERPLAN_H
