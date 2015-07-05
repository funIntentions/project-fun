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
    std::priority_queue<PartialOrderPlan, std::vector<PartialOrderPlan>, ComparePlans> partialPlans;
    //std::queue<PartialOrderPlan> partialPlans;
    std::vector<Operator> operators;

    PartialOrderPlanner(std::vector<Operator> domainOperators) : operators(domainOperators)
    {

    }

    std::vector<PartialOrderPlan> pop(Operator initialState, Operator endGoal);

    void addOpenGoals(PartialOrderPlan& plan, Operator newOperator);

    void addTemporalLink(PartialOrderPlan& plan, long op, long target, bool isBefore);

    // Checks to see if there are any operators existing that could threaten this link
    void findThreatsToCausalLink(PartialOrderPlan& plan, long newOperator, Goal causalLink);

    // Checks if newOperator is a threat to any existing links (needed for SE) - not addition I guess? :P
    void findThreatsCausedByOperator(PartialOrderPlan& plan, Operator newOperator);

    void findAllThreats(PartialOrderPlan& plan);

    void resolveThreats(PartialOrderPlan& plan);

    void promote(PartialOrderPlan& plan, Threat threat);

    void demote(PartialOrderPlan& plan, const Threat& threat);

    bool isCyclicPlan(PartialOrderPlan plan, long vert);

    PartialOrderPlan makeInitialPlan(Operator& initialState, Operator& endGoal);

    Goal selectOpenGoal(PartialOrderPlan& plan);

    bool BFSVertComesBeforeVert2(PartialOrderPlan plan, long vert, long vert2);

    bool BFSVertComesAfterVert2(PartialOrderPlan plan, long vert, long vert2);

    bool alreadyUsed(PartialOrderPlan plan, Operator op); // TODO: find a way so that I don't have to limit the number of potencial ops

    // Simple establishment/use existing operator to satisfy goal
    void doSimpleEstablishment(PartialOrderPlan& partialPlan, Operator& chosen, Goal goal);

    // add a new operator to satisfy a goal
    void doOperatorAddition(PartialOrderPlan& plan,
                          Operator& newOperator,
                          Goal goal);

    void chooseOperator(PartialOrderPlan& plan, Goal goal);

};


#endif //PARTIALORDERPLANNER_PARTIALORDERPLANNER_H
