//
// Created by dan on 29/06/15.
//

#ifndef PARTIALORDERPLANNER_SETTINGTABLE_H
#define PARTIALORDERPLANNER_SETTINGTABLE_H

#include "PartialOrderPlan.h"

enum TableStates
{
    tableCleared,
    onTableCloth,
    outGlasses,
    outPlates,
    outSilverware
};

struct TableDomain
{
    std::vector<Operator> operators;
    std::vector<Operator> correctPlan;
    Operator start, finish;

    TableDomain()
    {
        start.name = "start";
        finish.name = "finish";

        start.addedEffects = {tableCleared};
        finish.preconditions = {onTableCloth, outGlasses, outPlates, outSilverware};

        Operator layTableCloth("layTableCloth");
        layTableCloth.preconditions = {tableCleared};
        layTableCloth.addedEffects = {onTableCloth};
        layTableCloth.subtractedEffects = {tableCleared};
        Operator putOutGlasses("putOutGlasses");
        putOutGlasses.addedEffects = {outGlasses};
        putOutGlasses.subtractedEffects = {tableCleared};
        Operator putOutPlates("putOutPlates");
        putOutPlates.addedEffects = {outPlates};
        putOutPlates.subtractedEffects = {tableCleared};
        Operator putOutSilverware("putOutSilverware");
        putOutSilverware.addedEffects = {outSilverware};
        putOutSilverware.subtractedEffects = {tableCleared};

        correctPlan.push_back(finish);
        correctPlan.push_back(putOutGlasses);
        correctPlan.push_back(putOutPlates);
        correctPlan.push_back(putOutSilverware);
        correctPlan.push_back(layTableCloth);
        correctPlan.push_back(start);

        operators = {layTableCloth, putOutGlasses, putOutPlates, putOutSilverware};
    }
};

#endif //PARTIALORDERPLANNER_SETTINGTABLE_H
