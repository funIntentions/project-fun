//
// Created by Dan on 1/24/2016.
//

#include "util/Extra.h"
#include "gtest/gtest.h"
#include "tests/Briefcase.h"

#include "PartialOrderPlanner.h"

class PartialOrderPlannerFixture : public ::testing::Test {
protected:
    virtual void TearDown() {}

    virtual void SetUp() {}
public:
    PartialOrderPlanner partialOrderPlanner;

    PartialOrderPlannerFixture() : Test() {

    }

    virtual ~PartialOrderPlannerFixture() {

    }
};

TEST_F(PartialOrderPlannerFixture, briefcase_domain)
{
    BriefcaseDomain briefcaseDomain;

    std::vector<PartialOrderPlan> plans = partialOrderPlanner.findPartialOrderPlan(briefcaseDomain.start, briefcaseDomain.finish, briefcaseDomain.operators);

    ASSERT_GE(plans.size(), 0) << "number of plans found: " << plans.size() << std::endl;

    std::vector<long> totalOrderPlan = topologicalSort(plans[0], plans[0].start);
    int correct = 0;

    for (int step = 0; step != totalOrderPlan.size(); ++step)
    {
        std::unordered_map<long, Operator>::iterator op = plans[0].steps.find(totalOrderPlan[step]);
        ASSERT_TRUE(op != plans[0].steps.end());

        Operator stepOperator = op->second;
        Operator correctStep = briefcaseDomain.correctPlan[step];
        if (stepOperator.name == correctStep.name)
            ++correct;
    }

    EXPECT_EQ(correct, plans[0].steps.size());
}