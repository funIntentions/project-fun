//
// Created by dan on 29/06/15.
//

#ifndef PARTIALORDERPLANNER_FRUITBOWL_H
#define PARTIALORDERPLANNER_FRUITBOWL_H

#include "planner/PartialOrderPlan.h"

enum FruitBowlStates
{
    holdingBanana, // 0
    holdingOrange, // 1
    holdingPear, // 2
    pearOnTable, // 3
    pearInBowl, // 4
    bananaOnTable, // 5
    bananaInBowl, // 6
    orangeOnTable, // 7
    orangeInBowl, // 8
    handsEmpty, // 9
    hungry, // 10
    notHungry, // 11
    orangeNotEaten, // 12
    orangeEaten, // 13
    bananaNotEaten, // 14
    bananaEaten, // 15
    pearNotEaten, // 16
    pearEaten // 17
};

struct FruitBowlDomain
{
    std::vector<Operator> operators;
    std::vector<Operator> correctPlan;
    Operator start, finish;

    FruitBowlDomain()
    {
        start.name = "start";
        finish.name = "finish";

        start.addedEffects = {holdingBanana, orangeOnTable, orangeNotEaten, bananaNotEaten, hungry};
        finish.preconditions = {orangeInBowl, notHungry};

        Operator pickupOrangeFromTable("PickupOrangeFromTable");
        pickupOrangeFromTable.preconditions = {handsEmpty, orangeNotEaten, orangeOnTable};
        pickupOrangeFromTable.addedEffects = {holdingOrange};
        pickupOrangeFromTable.subtractedEffects = {handsEmpty, orangeOnTable};

        Operator pickupBananaFromTable("PickupBananaFromTable");
        pickupBananaFromTable.preconditions = {handsEmpty, bananaNotEaten, bananaOnTable};
        pickupBananaFromTable.addedEffects = {holdingBanana};
        pickupBananaFromTable.subtractedEffects = {handsEmpty, bananaOnTable};

        Operator pickupPearFromTable("PickupPearFromTable");
        pickupPearFromTable.preconditions = {handsEmpty, pearNotEaten, pearOnTable};
        pickupPearFromTable.addedEffects = {holdingPear};
        pickupPearFromTable.subtractedEffects = {handsEmpty, pearOnTable};

        Operator putPearInBowl("PutPearInBowl");
        putPearInBowl.preconditions = {holdingPear, pearNotEaten};
        putPearInBowl.addedEffects = {pearInBowl, handsEmpty};
        putPearInBowl.subtractedEffects = {holdingPear};

        Operator putOrangeInBowl("PutOrangeInBowl");
        putOrangeInBowl.preconditions = {holdingOrange, orangeNotEaten};
        putOrangeInBowl.addedEffects = {orangeInBowl, handsEmpty};
        putOrangeInBowl.subtractedEffects = {holdingOrange};

        Operator putBananaInBowl("PutBananaInBowl");
        putBananaInBowl.preconditions = {holdingBanana, bananaNotEaten};
        putBananaInBowl.addedEffects = {bananaInBowl, handsEmpty};
        putBananaInBowl.subtractedEffects = {holdingBanana};

        Operator eatBanana("EatBanana");
        eatBanana.preconditions = {hungry, holdingBanana, bananaNotEaten};
        eatBanana.addedEffects = {bananaEaten, notHungry, handsEmpty};
        eatBanana.subtractedEffects = {bananaNotEaten, hungry, holdingBanana};

        Operator eatOrange("EatOrange");
        eatOrange.preconditions = {hungry, holdingOrange, orangeNotEaten};
        eatOrange.addedEffects = {notHungry, orangeEaten, handsEmpty};
        eatOrange.subtractedEffects = {orangeNotEaten, hungry, holdingOrange};

        Operator eatPear("EatPear");
        eatPear.preconditions = {hungry, holdingPear, pearNotEaten};
        eatPear.addedEffects = {notHungry, pearEaten, handsEmpty};
        eatPear.subtractedEffects = {pearNotEaten, holdingPear, hungry};

        correctPlan.push_back(finish);
        correctPlan.push_back(putOrangeInBowl);
        correctPlan.push_back(pickupOrangeFromTable);
        correctPlan.push_back(eatBanana);
        correctPlan.push_back(start);

        operators = {pickupOrangeFromTable, putOrangeInBowl, putBananaInBowl, eatBanana};
    }
};

#endif //PARTIALORDERPLANNER_FRUITBOWL_H
