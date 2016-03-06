//
// Created by dan on 27/06/15.
//

#ifndef PARTIALORDERPLANNER_SUSSMAN_H
#define PARTIALORDERPLANNER_SUSSMAN_H

#include "../PartialOrderPlan.h"

enum SussmanStates
{
    ConA, // 0
    AonC, // 1
    BonA, // 2
    BonC, // 3
    AonB, // 4
    ConB, // 5
    AOnTable, // 6
    BOnTable, // 7
    COnTable, // 8
    clearB, // 9
    clearC, // 10
    clearA, // 11
    holdingA, // 12
    holdingB, // 13
    holdingC, // 14
    handEmpty // 15
};


struct SussmanDomain
{
public:
    std::vector<Operator> operators;
    std::vector<Operator> correctPlan;
    Operator start, finish;

    SussmanDomain()
    {
        start.name = "start";
        finish.name = "finish";

        start.addedEffects = {ConA, handEmpty, AOnTable, BOnTable, clearB, clearC};
        finish.preconditions = {AonB, BonC};

        //start.addedEffects = {AonB, BonC, COnTable, clearA, handEmpty};
        //finish.preconditions = {AonC};

        Operator unstackCA("Unstack(C,A)");
        unstackCA.preconditions = {clearC, ConA, handEmpty};
        unstackCA.addedEffects = {holdingC, clearA};
        unstackCA.subtractedEffects = {clearC, ConA, handEmpty};

        Operator unstackAB("Unstack(A,B)");
        unstackAB.preconditions = {clearA, AonB, handEmpty};
        unstackAB.addedEffects = {holdingA, clearB};
        unstackAB.subtractedEffects = {clearA, AonB, handEmpty};

        Operator unstackBC("Unstack(B,C)");
        unstackBC.preconditions = {clearB, BonC, handEmpty};
        unstackBC.addedEffects = {holdingB, clearC};
        unstackBC.subtractedEffects = {clearB, BonC, handEmpty};

        Operator pickupA("Pickup(A)");
        pickupA.preconditions = {AOnTable, clearA, handEmpty};
        pickupA.addedEffects = {holdingA};
        pickupA.subtractedEffects = {clearA, AOnTable, handEmpty};

        Operator pickupB("Pickup(B)");
        pickupB.preconditions = {BOnTable, clearB, handEmpty};
        pickupB.addedEffects = {holdingB};
        pickupB.subtractedEffects = {BOnTable, clearB, handEmpty};

        Operator pickupC("Pickup(C)");
        pickupC.preconditions = {COnTable, clearC, handEmpty};
        pickupC.addedEffects = {holdingC};
        pickupC.subtractedEffects = {COnTable, clearC, handEmpty};

        Operator stackAB("Stack(A,B)");
        stackAB.preconditions = {holdingA, clearB};
        stackAB.addedEffects = {AonB, clearA, handEmpty};
        stackAB.subtractedEffects = {holdingA, clearB};

        Operator stackAC("Stack(A,C)");
        stackAC.preconditions = {holdingA, clearC};
        stackAC.addedEffects = {AonC, clearA, handEmpty};
        stackAC.subtractedEffects = {holdingA, clearC};

        Operator stackBC("Stack(B,C)");
        stackBC.preconditions = {holdingB, clearC};
        stackBC.addedEffects = {BonC, clearB, handEmpty};
        stackBC.subtractedEffects = {holdingB, clearC};

        Operator stackCA("Stack(C,A)");
        stackCA.preconditions = {holdingC, clearA};
        stackCA.addedEffects = {ConA, clearC, handEmpty};
        stackCA.subtractedEffects = {holdingC, clearA};

        Operator stackCB("Stack(C,B)");
        stackCB.preconditions = {holdingC, clearB};
        stackCB.addedEffects = {ConB, clearC, handEmpty};
        stackCB.subtractedEffects = {holdingC, clearB};

        Operator putdownC("Putdown(C)");
        putdownC.preconditions = {holdingC};
        putdownC.addedEffects = {COnTable, clearC, handEmpty};
        putdownC.subtractedEffects = {holdingC};

        Operator putdownA("Putdown(A)");
        putdownA.preconditions = {holdingA};
        putdownA.addedEffects = {AOnTable, clearA, handEmpty};
        putdownA.subtractedEffects = {holdingA};

        Operator putdownB("Putdown(B)");
        putdownB.preconditions = {holdingB};
        putdownB.addedEffects = {BOnTable, clearB, handEmpty};
        putdownB.subtractedEffects = {holdingB};

        correctPlan.push_back(finish);
        correctPlan.push_back(stackAB);
        correctPlan.push_back(pickupA);
        correctPlan.push_back(stackBC);
        correctPlan.push_back(pickupB);
        correctPlan.push_back(putdownC);
        correctPlan.push_back(unstackCA);
        correctPlan.push_back(start);

        operators = {unstackCA, unstackAB, unstackBC, pickupA, pickupB, pickupC, stackAB, stackBC, stackCA, stackCB, stackAC, putdownC, putdownA, putdownB};
    }
};

#endif //PARTIALORDERPLANNER_SUSSMAN_H
