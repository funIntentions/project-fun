//
// Created by dan on 29/06/15.
//

#ifndef PARTIALORDERPLANNER_BRIEFCASE_H
#define PARTIALORDERPLANNER_BRIEFCASE_H

#include "../PartialOrderPlan.h"

enum BriefcaseStates
{
    dictionaryAtOffice = 0, // 0
    paycheckAtOffice = 1, // 1
    dictionaryAtHome = 2, // 2
    paycheckAtHome = 3, // 3
    dictionaryInBriefcase = 4, // 4
    paycheckInBriefcase = 5, // 5
    briefcaseAtHome = 6, // 6
    briefcaseAtOffice = 7 // 7
};

struct BriefcaseDomain
{
public:

    std::vector<Operator> operators;
    Operator start, finish;

    BriefcaseDomain()
    {
        start.name = "start";
        finish.name = "finish";

        start.addedEffects = {briefcaseAtHome, dictionaryAtHome, paycheckInBriefcase};
        finish.preconditions = {dictionaryAtOffice, paycheckAtHome};

        Operator homeToOffice("HomeToOffice");
        homeToOffice.preconditions = {briefcaseAtHome};
        homeToOffice.addedEffects = {briefcaseAtOffice};
        homeToOffice.subtractedEffects = {briefcaseAtHome};

        Operator officeToHome("OfficeToHome");
        officeToHome.preconditions = {briefcaseAtOffice};
        officeToHome.addedEffects = {briefcaseAtHome};
        officeToHome.subtractedEffects = {briefcaseAtOffice};

        Operator takeOutPaycheckAtHome("takeOutPaycheckAtHome");
        takeOutPaycheckAtHome.preconditions = {paycheckInBriefcase, briefcaseAtHome};
        takeOutPaycheckAtHome.addedEffects = {paycheckAtHome};
        takeOutPaycheckAtHome.subtractedEffects = {paycheckInBriefcase};

        Operator takeOutPaycheckAtOffice("takeOutPaycheckAtOffice");
        takeOutPaycheckAtOffice.preconditions = {paycheckInBriefcase, briefcaseAtOffice};
        takeOutPaycheckAtOffice.addedEffects = {paycheckAtOffice};
        takeOutPaycheckAtOffice.subtractedEffects = {paycheckInBriefcase};

        Operator putPaycheckInAtHome("putPaycheckInAtHome");
        putPaycheckInAtHome.preconditions = {paycheckAtHome, briefcaseAtHome};
        putPaycheckInAtHome.addedEffects = {paycheckInBriefcase};
        putPaycheckInAtHome.subtractedEffects = {paycheckAtHome};

        Operator putPaycheckInAtOffice("putPaycheckInAtOffice");
        putPaycheckInAtOffice.preconditions = {paycheckAtOffice, briefcaseAtOffice};
        putPaycheckInAtOffice.addedEffects = {paycheckInBriefcase};
        putPaycheckInAtOffice.subtractedEffects = {paycheckAtOffice};

        Operator takeOutDictionaryAtHome("takeOutDictionaryAtHome");
        takeOutDictionaryAtHome.preconditions = {dictionaryInBriefcase, briefcaseAtHome};
        takeOutDictionaryAtHome.addedEffects = {dictionaryAtHome};
        takeOutDictionaryAtHome.subtractedEffects = {dictionaryInBriefcase};

        Operator takeOutDictionaryAtOffice("takeOutDictionaryAtOffice");
        takeOutDictionaryAtOffice.preconditions = {dictionaryInBriefcase, briefcaseAtOffice};
        takeOutDictionaryAtOffice.addedEffects = {dictionaryAtOffice};
        takeOutDictionaryAtOffice.subtractedEffects = {dictionaryInBriefcase};

        Operator putDictionaryInAtHome("putDictionaryInAtHome");
        putDictionaryInAtHome.preconditions = {dictionaryAtHome, briefcaseAtHome};
        putDictionaryInAtHome.addedEffects = {dictionaryInBriefcase};
        putDictionaryInAtHome.subtractedEffects = {dictionaryAtHome};

        Operator putDictionaryInAtOffice("putDictionaryInAtOffice");
        putDictionaryInAtOffice.preconditions = {dictionaryAtOffice, briefcaseAtOffice};
        putDictionaryInAtOffice.addedEffects = {dictionaryInBriefcase};
        putDictionaryInAtOffice.subtractedEffects = {dictionaryAtOffice};

        operators = {officeToHome,
                      homeToOffice,
                      takeOutPaycheckAtHome,
                      takeOutPaycheckAtOffice,
                      putPaycheckInAtHome,
                      putPaycheckInAtOffice,
                      takeOutDictionaryAtHome,
                      takeOutDictionaryAtOffice,
                      putDictionaryInAtHome,
                      putDictionaryInAtOffice};

    }

};

#endif //PARTIALORDERPLANNER_BRIEFCASE_H
