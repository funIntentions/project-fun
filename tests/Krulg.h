//
// Created by Dan on 9/19/2015.
//

#ifndef PARTIALORDERPLANNER_KRULG_H
#define PARTIALORDERPLANNER_KRULG_H

#include <vector>
#include "../PartialOrderPlan.h"

enum AdventureStates
{
    krulgWanted = 0,
    krulgHero = 1,
    krulgArrested = 2,
    krulgHasPebbles = 3,
    inDarkvoid = 4,
    inMarshland = 5,
    inAshplane = 6,
    marshlandHelped = 7,
    marshlandEnslaved = 8,
    ashplaneHelped = 9,
    ashplaneEnslaved = 10,
    falsifiedEvidense = 11,
    wizardHasPebbles = 12,
    wizardDoesntHavePebbles = 13,
    krulgDoesntHavePebbles = 14,
    notInMarshland = 15,
    notInAshplane = 16,
    marshlandNotHelped = 17,
    marshlandNotEnslaved = 18,
    ashplaneNotHelped = 19,
    ashplaneNotEnslaved = 20,
    noFalsifiedEvidence = 21,
    krulgNotArrested = 22,
    krulgNotWanted = 23,
    krulgNotHero = 24,
    notInDarkvoid = 25
};

struct Krulg {
public:
    std::vector<Operator> operators;
    Operator start, finish;

    Krulg()
    {
        start.name = "start";
        finish.name = "finish";

        start.addedEffects = {krulgHasPebbles, inDarkvoid, notInAshplane, notInDarkvoid, marshlandNotEnslaved, marshlandNotHelped, ashplaneNotEnslaved, ashplaneNotHelped, krulgNotArrested, krulgNotHero, krulgNotWanted, noFalsifiedEvidence, wizardDoesntHavePebbles};
        finish.preconditions = {wizardHasPebbles};

        Operator helpMarshland("Help Marshland");
        helpMarshland.preconditions = {marshlandNotHelped, marshlandNotEnslaved, inMarshland};
        helpMarshland.addedEffects = {marshlandHelped};
        helpMarshland.subtractedEffects = {marshlandNotHelped};

        Operator helpAshplane("Help Ashplane");
        helpAshplane.preconditions = {ashplaneNotHelped, ashplaneNotEnslaved, inAshplane};
        helpAshplane.addedEffects = {ashplaneHelped};
        helpAshplane.subtractedEffects = {ashplaneNotHelped};

        Operator enslaveMarshland("Enslave Marshland");
        enslaveMarshland.preconditions = {marshlandNotHelped, marshlandNotEnslaved, inMarshland};
        enslaveMarshland.addedEffects = {marshlandEnslaved, krulgWanted};
        enslaveMarshland.subtractedEffects = {marshlandNotEnslaved, krulgNotWanted};

        Operator enslaveAshplane("Enslave Ashplane");
        enslaveAshplane.preconditions = {ashplaneNotHelped, ashplaneNotEnslaved, inAshplane};
        enslaveAshplane.addedEffects = {ashplaneEnslaved, krulgWanted};
        enslaveAshplane.subtractedEffects = {ashplaneNotEnslaved, krulgNotWanted};

        Operator darkVoidToAshplane("Travel to Ashplane from Darkvoid");
        darkVoidToAshplane.preconditions = {notInAshplane, inDarkvoid};
        darkVoidToAshplane.addedEffects = {inAshplane, notInDarkvoid};
        darkVoidToAshplane.subtractedEffects = {inDarkvoid, notInAshplane};

        Operator darkVoidToMarshland("Travel to Marshland from Darkvoid");
        darkVoidToMarshland.preconditions = {notInMarshland, inDarkvoid};
        darkVoidToMarshland.addedEffects = {inMarshland, notInDarkvoid};
        darkVoidToMarshland.subtractedEffects = {inDarkvoid, notInMarshland};

        Operator ashplaceToMarshland("Travel to Marshland from Ashplane");
        ashplaceToMarshland.preconditions = {notInMarshland, inAshplane};
        ashplaceToMarshland.addedEffects = {inMarshland, notInAshplane};
        ashplaceToMarshland.subtractedEffects = {inAshplane, notInMarshland};

        Operator ashplaneToDarkvoid("Travel to Darkvoid from Ashplane");
        ashplaneToDarkvoid.preconditions = {notInDarkvoid, inAshplane};
        ashplaneToDarkvoid.addedEffects = {inDarkvoid, notInAshplane};
        ashplaneToDarkvoid.subtractedEffects = {inAshplane, notInDarkvoid};

        Operator marshlandToAshplane("Travel to Ashplane from Marshland");
        marshlandToAshplane.preconditions = {notInAshplane, inMarshland};
        marshlandToAshplane.addedEffects = {inAshplane, notInMarshland};
        marshlandToAshplane.subtractedEffects = {inMarshland, notInAshplane};

        Operator marshlandToDarkvoid("Travel to Darkvoid from Marshland");
        marshlandToDarkvoid.preconditions = {notInDarkvoid, inMarshland};
        marshlandToDarkvoid.addedEffects = {inDarkvoid, notInMarshland};
        marshlandToDarkvoid.subtractedEffects = {inMarshland, notInDarkvoid};

        Operator falsifyEvidence("Falsify Evidence");
        falsifyEvidence.preconditions = {noFalsifiedEvidence, marshlandHelped, ashplaneHelped, marshlandNotEnslaved, ashplaneNotEnslaved, krulgNotWanted};
        falsifyEvidence.addedEffects = {falsifiedEvidense, krulgWanted};
        falsifyEvidence.subtractedEffects = {krulgNotWanted, noFalsifiedEvidence};

        Operator arrestKrulg("Arrest Krulg");
        arrestKrulg.preconditions = {krulgNotArrested, krulgWanted};
        arrestKrulg.addedEffects = {krulgArrested};
        arrestKrulg.subtractedEffects = {krulgNotArrested, krulgWanted};

        Operator wizardStealsPebbles("Wizard Steals Pebbles");
        wizardStealsPebbles.preconditions = {krulgArrested, krulgHasPebbles, wizardDoesntHavePebbles};
        wizardStealsPebbles.addedEffects = {krulgDoesntHavePebbles, wizardHasPebbles};
        wizardStealsPebbles.subtractedEffects = {wizardDoesntHavePebbles, krulgHasPebbles};

        operators = {helpAshplane, helpMarshland, enslaveAshplane, enslaveMarshland, darkVoidToAshplane, darkVoidToMarshland, marshlandToAshplane, marshlandToDarkvoid, ashplaceToMarshland, ashplaneToDarkvoid, falsifyEvidence, arrestKrulg, wizardStealsPebbles};
    }

};


#endif //PARTIALORDERPLANNER_KRULG_H
