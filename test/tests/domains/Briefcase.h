//
// Created by dan on 29/06/15.
//

#ifndef PARTIALORDERPLANNER_BRIEFCASE_H
#define PARTIALORDERPLANNER_BRIEFCASE_H

#include "PartialOrderPlan.h"

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
    std::vector<Operator> correctPlan;
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

        //std::vector<Operator> correctPlan1;
        correctPlan.push_back(finish);
        correctPlan.push_back(takeOutDictionaryAtOffice);
        correctPlan.push_back(homeToOffice);
        correctPlan.push_back(takeOutPaycheckAtHome);
        correctPlan.push_back(putDictionaryInAtHome);
        correctPlan.push_back(start);

        /*std::vector<Operator> correctPlan2;
        correctPlan2.push_back(start);
        correctPlan2.push_back(putDictionaryInAtHome);
        correctPlan2.push_back(takeOutPaycheckAtHome);
        correctPlan2.push_back(homeToOffice);
        correctPlan2.push_back(takeOutDictionaryAtOffice);
        correctPlan2.push_back(end);

        correctPlans.push_back(correctPlan1);
        correctPlans.push_back(correctPlan2);*/

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
