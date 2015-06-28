#include <iostream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <unordered_map>
#include <list>
#include <queue>
#include <string>
#include "PartialOrderPlanner.h"
#include "tests/Sussman.h"

using namespace std;

//static const long NONE = -1;

enum Goals
{
    tableCleared,
    onTableCloth,
    outGlasses,
    outPlates,
    outSilverware
};

enum BriefcaseGoals
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

enum FruitBowlGoals
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

vector<long> topologicalSort(PartialOrderPlan plan, long vert)
{
    vector<long> totalOrderPlan;
    long numVertices = plan.ordering.size();
    long* parent = new long[numVertices];

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        parent[i] = -1;
    }

    list<long> stack;

    parent[vert] = -1;
    stack.push_back(vert);

    vector<TemporalLink>::iterator itr;

    while(!stack.empty())
    {
        vert = stack.back();
        cout << vert << ": ";

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (parent[itr->targetOperator] < 0 && itr->isBefore)
            {
                cout << "isBefore (" << itr->isBefore << ") -> " << itr->targetOperator << "| ";
                parent[itr->targetOperator] = vert;
                stack.push_back(itr->targetOperator);
                break;
            }
        }

        if (itr == plan.ordering[vert].end())
        {
            totalOrderPlan.push_back(vert);
            stack.pop_back();
        }

        cout << "\n";
    }

    return totalOrderPlan;
}

void BFS(PartialOrderPlan plan, long vert)
{
    long numVertices = plan.steps.size();
    bool* visited = new bool[numVertices];
    for (unsigned int i = 0; i < numVertices; ++i)
    {
        visited[i] = false;
    }

    list<long> queue;

    visited[vert] = true;
    queue.push_back(vert);

    vector<TemporalLink>::iterator itr;

    while(!queue.empty())
    {
        vert = queue.front();
        cout << vert << ": ";
        queue.pop_front();

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (!visited[itr->targetOperator])
            {
                cout << "isBefore (" << itr->isBefore << ") -> " << itr->targetOperator << "| ";
                visited[itr->targetOperator] = true;
                queue.push_back(itr->targetOperator);
            }
        }

        cout << "\n";
    }
}

void DFSVisit(PartialOrderPlan plan, long vert)
{
    long numVertices = plan.ordering.size();
    long* parent = new long[numVertices];

    for (unsigned int i = 0; i < numVertices; ++i)
    {
        parent[i] = -1;
    }

    list<long> stack;

    parent[vert] = -1;
    stack.push_back(vert);

    vector<TemporalLink>::iterator itr;

    while(!stack.empty())
    {
        vert = stack.back();
        cout << vert << ": ";

        for (itr = plan.ordering[vert].begin(); itr != plan.ordering[vert].end(); ++itr)
        {
            if (parent[itr->targetOperator] > -1)
            {
                cout << "isBefore (" << itr->isBefore << ") -> " << itr->targetOperator << "| ";
                parent[itr->targetOperator] = vert;
                stack.push_back(itr->targetOperator);
                break;
            }
        }

        if (itr == plan.ordering[vert].end())
        {
            stack.pop_back();
        }

        cout << "\n";
    }
}

int main()
{
    // TODO: Building new operators should be made much quicker/easier than this
    // TODO: check how long the algorithm takes
    
    // TODO: store this "setting the table" example somewhere
    //Operator start("start");
    //Operator finish("finish");

    /*start.addedEffects = {tableCleared};
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

    vector<Operator> operators = {layTableCloth, putOutGlasses, putOutPlates, putOutSilverware};

    vector<Plan> plans = pop(start, finish, operators);*/

    // TODO: Briefcase domain
    /*start.addedEffects = {briefcaseAtHome, dictionaryAtHome, paycheckInBriefcase};
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

    vector<Operator> operators = {officeToHome,
                                  homeToOffice,
                                  takeOutPaycheckAtHome,
                                  takeOutPaycheckAtOffice,
                                  putPaycheckInAtHome,
                                  putPaycheckInAtOffice,
                                  takeOutDictionaryAtHome,
                                  takeOutDictionaryAtOffice,
                                  putDictionaryInAtHome,
                                  putDictionaryInAtOffice};*/


    // TODO: Fruit Bowl Domain 1

    /*start.addedEffects = {holdingBanana, orangeOnTable, orangeNotEaten, bananaNotEaten, hungry};
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
    eatBanana.subtractedEffects = {bananaNotEaten, hungry};


    vector <Operator> operators = {pickupOrangeFromTable, putOrangeInBowl, putBananaInBowl, eatBanana};*/

    // TODO: Fruit Bowl Domain 2

    /*start.addedEffects = {holdingPear, orangeOnTable, pearOnTable, pearNotEaten, orangeNotEaten, bananaNotEaten, hungry};
    finish.preconditions = {pearInBowl, notHungry};

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

    vector <Operator> operators = {putPearInBowl, eatPear};*/

    SussmanDomain sussmanDomain;
    PartialOrderPlanner partialOrderPlanner(sussmanDomain.operators);
    vector<PartialOrderPlan> plans = partialOrderPlanner.pop(sussmanDomain.start, sussmanDomain.finish);

    cout << endl;
    cout << "Number of potencial plans: " << plans.size() << endl;
    cout << endl;


    // Display plan's Causal links
    if (plans.size() > 0)
    {
        PartialOrderPlan plan = plans[0];

        cout << endl;
        for (auto l = plan.steps.begin(); l != plan.steps.end(); ++l)
        {
            cout << l->second.name << endl;
        }
        cout << endl;

        int id = 0;
        for (auto itr = plan.links.begin(); itr != plan.links.end(); ++itr)
        {
            vector<CausalLink> links = *itr;

            for (auto j = links.begin(); j != links.end(); ++j)
            {
                cout << "Final: Causal Link From: " << plan.steps.at(id).name << " To: " << plan.steps.at(j->targetOperator).name << " For Condition: " << j->condition << endl;
            }
            ++id;
        }

        cout << endl;
        // Display plan's ordering
        id = 0;
        for (auto i = plan.ordering.begin(); i != plan.ordering.end(); ++i)
        {
            vector<TemporalLink> order = *i;

            for (auto t = order.begin(); t != order.end(); ++t)
            {
                cout << "Temporal Link For " << plan.steps.at(id).name << " To: " << plan.steps.at(t->targetOperator).name << " Is Before? " << t->isBefore << endl;
            }
            ++id;
        }

        cout << endl << "Building totoal order plan" << endl;
        vector<long> totalOrderPlan = topologicalSort(plans[0], plans[0].start);//getTotalOrderPlan(plans[0]);

        cout << endl;
        cout << "Total Order Plan (One of them)" << endl;
        cout << endl;

        for (auto step = totalOrderPlan.begin(); step != totalOrderPlan.end(); ++step)
        {
            unordered_map<long, Operator>::const_iterator op = plans[0].steps.find(*step);
            if (op != plans[0].steps.end())
                cout << op->second.name << endl;
            else
                cout << "Oops: something is wrong" << endl;
        }

        /*vector<vector<long>> totalOrderPlans = generateTotalOrderPlans(plans[0]);
        cout << endl;
        cout << "Total Order Plan (One of them)" << endl;
        cout << endl;

        for (auto totalOrderPlan = totalOrderPlans.begin(); totalOrderPlan != totalOrderPlans.end(); ++totalOrderPlan)
        {
            cout << endl;
            cout << "++ Possible Plan ++" << endl;
            cout << endl;

            for (auto step = totalOrderPlan->begin(); step != totalOrderPlan->end(); ++step)
            {
                unordered_map<long, Operator>::const_iterator op = plans[0].steps.find(*step);
                if (op != plans[0].steps.end())
                    cout << op->second.name << endl;
                else
                    cout << "Oops: something is wrong" << endl;
            }

            cout << "=================" << endl;
        }*/
    }

    return 0;
}


