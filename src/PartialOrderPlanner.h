//
// Created by dan on 27/06/15.
//

#ifndef PARTIALORDERPLANNER_PARTIALORDERPLANNER_H
#define PARTIALORDERPLANNER_PARTIALORDERPLANNER_H


#include "PartialOrderPlan.h"

class PartialOrderPlanner
{
private:
    std::vector<PartialOrderPlan> finishedPlans;
    std::priority_queue<PartialOrderPlan, std::vector<PartialOrderPlan>, ComparePlans> partialPlans;
    std::vector<Operator> operators;

    void addOpenGoals(PartialOrderPlan& plan, const Operator& newOperator);

    void addTemporalLink(PartialOrderPlan& plan, long op, long target, bool isBefore);

    // Checks to see if there are any operators existing that could threaten this link
    void findThreatsToCausalLink(PartialOrderPlan& plan, long newOperator, Goal causalLink);

    // Checks if newOperator is a threat to any existing links (needed for SE) - not addition I guess? :P
    void findThreatsCausedByOperator(PartialOrderPlan& plan, const Operator& newOperator);

    void findAllThreats(PartialOrderPlan& plan);

    void resolveThreats(PartialOrderPlan& plan);

    void promote(PartialOrderPlan& plan, const Threat& threat);

    void demote(PartialOrderPlan& plan, const Threat& threat);

    bool isCyclicPlan(const PartialOrderPlan& plan);

    PartialOrderPlan makeInitialPlan(Operator& initialState, Operator& endGoal);

    Goal selectOpenGoal(PartialOrderPlan& plan);

    bool step1IsBeforeStep2(const PartialOrderPlan& plan, long step1, long step2);

    bool step1IsAfterStep2(const PartialOrderPlan& plan, long step1, long step2);

    bool alreadyUsed(const PartialOrderPlan& plan, const Operator& op); // TODO: find a way so that I don't have to limit the number of potencial ops

    // Simple establishment/use existing operator to satisfy goal
    void doSimpleEstablishment(PartialOrderPlan& partialPlan, Operator& chosen, Goal goal);

    // add a new operator to satisfy a goal
    void doOperatorAddition(PartialOrderPlan& plan,
                            Operator& newOperator,
                            Goal goal);

    void chooseOperator(PartialOrderPlan& plan, Goal goal);

public:
    PartialOrderPlanner(std::vector<Operator> domainOperators) : operators(domainOperators)
    {

    }

    std::vector<PartialOrderPlan> findPartialOrderPlan(Operator initialState, Operator endGoal);
};


#endif //PARTIALORDERPLANNER_PARTIALORDERPLANNER_H
