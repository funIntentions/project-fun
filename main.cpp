#include <iostream>
#include <vector>
#include "PartialOrderPlanner.h"
#include "tests/Sussman.h"
#include "tests/Briefcase.h"
#include "tests/FruitBowl.h"
#include "tests/Table.h"
#include "helper/extras.h"



int main()
{
    // TODO: Building new operators should be made much quicker/easier than this
    // TODO: check how long the algorithm takes

    SussmanDomain sussmanDomain;
    BriefcaseDomain briefcaseDomain;
    FruitBowlDomain fruitBowlDomain;
    TableDomain tableDomain;
    PartialOrderPlanner partialOrderPlanner(sussmanDomain.operators);
    vector<PartialOrderPlan> plans = partialOrderPlanner.findPartialOrderPlan(sussmanDomain.start, sussmanDomain.finish);

    // Display plan's Causal links
    if (plans.size() > 0)
    {
        printPlanInformation(plans[0]);
    }

    return 0;
}


