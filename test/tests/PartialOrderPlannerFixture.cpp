//
// Created by Dan on 1/24/2016.
//

#include "util/Extra.h"
#include "gtest/gtest.h"
#include "domains/Briefcase.h"
#include "domains/FruitBowl.h"
#include "domains/Sussman.h"
#include "domains/Table.h"

#include "planner/PartialOrderPlanner.h"

class PartialOrderPlannerFixture : public ::testing::Test {
protected:
    virtual void TearDown() {}

    virtual void SetUp() {}
public:
    PartialOrderPlanner partialOrderPlanner;

    PartialOrderPlannerFixture() : Test() {

    }

    bool plansMatch(std::vector<Operator> planA, std::vector<Operator> planB)
    {
        if (planA.size() != planB.size())
            return false;

        int correct = 0;

        for (int step = 0; step < planA.size(); ++step)
        {
            Operator a = planA[step];
            Operator b = planB[step];

            if (a.name == b.name)
                ++correct;
        }

        return (correct == planA.size());
    }

    void testDomain(Operator start, Operator end, std::vector<Operator> operators, std::vector<Operator> correctPlan)
    {
        std::vector<PartialOrderPlan> plans = partialOrderPlanner.findPartialOrderPlan(start, end, operators);

        ASSERT_GE(plans.size(), 0) << "No plans found."<< std::endl;

        std::vector<unsigned> totalOrderPlan = topologicalSort(plans[0], plans[0].start);
        std::vector<Operator> plan;

        for (int step = 0; step != totalOrderPlan.size(); ++step)
        {
            std::unordered_map<unsigned, Operator>::iterator op = plans[0].steps.find(totalOrderPlan[step]);
            ASSERT_NE(op, plans[0].steps.end());
            plan.push_back(op->second);
        }

        EXPECT_TRUE(plansMatch(correctPlan, plan));
    }
    virtual ~PartialOrderPlannerFixture() {

    }
};

TEST_F(PartialOrderPlannerFixture, briefcase_domain)
{
    BriefcaseDomain briefcaseDomain;
    testDomain(briefcaseDomain.start, briefcaseDomain.finish, briefcaseDomain.operators, briefcaseDomain.correctPlan);
}

TEST_F(PartialOrderPlannerFixture, fruitbowl_domain)
{
    FruitBowlDomain fruitBowlDomain;
    testDomain(fruitBowlDomain.start, fruitBowlDomain.finish, fruitBowlDomain.operators, fruitBowlDomain.correctPlan);
}

TEST_F(PartialOrderPlannerFixture, sussman_domain)
{
    SussmanDomain sussmanDomain;
    testDomain(sussmanDomain.start, sussmanDomain.finish, sussmanDomain.operators, sussmanDomain.correctPlan);
}

TEST_F(PartialOrderPlannerFixture, table_domain)
{
    TableDomain tableDomain;
    testDomain(tableDomain.start, tableDomain.finish, tableDomain.operators, tableDomain.correctPlan);
}
