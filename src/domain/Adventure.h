//
// Created by Dan on 4/2/2016.
//

#ifndef PARTIALORDERPLANNER_ADVENTURE_H
#define PARTIALORDERPLANNER_ADVENTURE_H
#include <vector>
#include <set>
#include "PartialOrderPlan.h"

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

struct AdventureDomain {
public:
    std::vector<Operator> operators;
    Operator start, finish;
    std::set<std::string> npcActions;
    Operator* helpMarshland;
    Operator* helpAshplane;
    Operator* enslaveMarshland;
    Operator* enslaveAshplane;
    Operator* darkvoidToAshplane;
    Operator* darkvoidToMarshland;
    Operator* ashplaneToDarkvoid;
    Operator* ashplaneToMarshland;
    Operator* marshlandToDarkvoid;
    Operator* marshlandToAshplane;

    AdventureDomain()
    {
        start.name = "start";
        finish.name = "finish";

        start.addedEffects = {krulgHasPebbles, inDarkvoid, notInAshplane, notInMarshland, marshlandNotEnslaved, marshlandNotHelped, ashplaneNotEnslaved, ashplaneNotHelped, krulgNotArrested, krulgNotHero, krulgNotWanted, noFalsifiedEvidence, wizardDoesntHavePebbles};
        finish.preconditions = {wizardHasPebbles};

        helpMarshland = new Operator("Help Marshland");
        helpMarshland->preconditions = {marshlandNotHelped, marshlandNotEnslaved, inMarshland};
        helpMarshland->addedEffects = {marshlandHelped};
        helpMarshland->subtractedEffects = {marshlandNotHelped};

        helpAshplane = new Operator("Help Ashplane");
        helpAshplane->preconditions = {ashplaneNotHelped, ashplaneNotEnslaved, inAshplane};
        helpAshplane->addedEffects = {ashplaneHelped};
        helpAshplane->subtractedEffects = {ashplaneNotHelped};

        enslaveMarshland = new Operator("Enslave Marshland");
        enslaveMarshland->preconditions = {marshlandNotHelped, marshlandNotEnslaved, inMarshland};
        enslaveMarshland->addedEffects = {marshlandEnslaved, krulgWanted};
        enslaveMarshland->subtractedEffects = {marshlandNotEnslaved, krulgNotWanted};

        enslaveAshplane = new Operator("Enslave Ashplane");
        enslaveAshplane->preconditions = {ashplaneNotHelped, ashplaneNotEnslaved, inAshplane};
        enslaveAshplane->addedEffects = {ashplaneEnslaved, krulgWanted};
        enslaveAshplane->subtractedEffects = {ashplaneNotEnslaved, krulgNotWanted};

        darkvoidToAshplane = new Operator("Travel to Ashplane from Darkvoid");
        darkvoidToAshplane->preconditions = {notInAshplane, inDarkvoid};
        darkvoidToAshplane->addedEffects = {inAshplane, notInDarkvoid};
        darkvoidToAshplane->subtractedEffects = {inDarkvoid, notInAshplane};

        darkvoidToMarshland = new Operator("Travel to Marshland from Darkvoid");
        darkvoidToMarshland->preconditions = {notInMarshland, inDarkvoid};
        darkvoidToMarshland->addedEffects = {inMarshland, notInDarkvoid};
        darkvoidToMarshland->subtractedEffects = {inDarkvoid, notInMarshland};

        ashplaneToMarshland = new Operator("Travel to Marshland from Ashplane");
        ashplaneToMarshland->preconditions = {notInMarshland, inAshplane};
        ashplaneToMarshland->addedEffects = {inMarshland, notInAshplane};
        ashplaneToMarshland->subtractedEffects = {inAshplane, notInMarshland};

        ashplaneToDarkvoid = new Operator("Travel to Darkvoid from Ashplane");
        ashplaneToDarkvoid->preconditions = {notInDarkvoid, inAshplane};
        ashplaneToDarkvoid->addedEffects = {inDarkvoid, notInAshplane};
        ashplaneToDarkvoid->subtractedEffects = {inAshplane, notInDarkvoid};

        marshlandToAshplane = new Operator("Travel to Ashplane from Marshland");
        marshlandToAshplane->preconditions = {notInAshplane, inMarshland};
        marshlandToAshplane->addedEffects = {inAshplane, notInMarshland};
        marshlandToAshplane->subtractedEffects = {inMarshland, notInAshplane};

        marshlandToDarkvoid = new Operator("Travel to Darkvoid from Marshland");
        marshlandToDarkvoid->preconditions = {notInDarkvoid, inMarshland};
        marshlandToDarkvoid->addedEffects = {inDarkvoid, notInMarshland};
        marshlandToDarkvoid->subtractedEffects = {inMarshland, notInDarkvoid};

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

        npcActions = {falsifyEvidence.name, arrestKrulg.name, wizardStealsPebbles.name};

        operators = {*helpAshplane, *helpMarshland, *enslaveAshplane, *enslaveMarshland, *darkvoidToAshplane,
                     *darkvoidToMarshland, *marshlandToAshplane, *marshlandToDarkvoid, *ashplaneToMarshland, *ashplaneToDarkvoid, falsifyEvidence, arrestKrulg, wizardStealsPebbles};
    }

};

#endif //PARTIALORDERPLANNER_ADVENTURE_H