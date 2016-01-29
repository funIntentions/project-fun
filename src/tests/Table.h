//
// Created by dan on 29/06/15.
//

#ifndef PARTIALORDERPLANNER_SETTINGTABLE_H
#define PARTIALORDERPLANNER_SETTINGTABLE_H

#include "../PartialOrderPlan.h"

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

        operators = {layTableCloth, putOutGlasses, putOutPlates, putOutSilverware};
    }
};

#endif //PARTIALORDERPLANNER_SETTINGTABLE_H
