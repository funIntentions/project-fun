//
// Created by Dan on 4/2/2016.
//

#include <gtest/gtest.h>
#include <EntityManager.h>
#include <components/AttributeComponentManager.h>
#include <PartialOrderPlanner.h>
#include <util/Extra.h>

class AttributeComponentFixture : public ::testing::Test {
protected:
    virtual void TearDown() {}

    virtual void SetUp()
    {
        character = entityManager.create("Character");
        attributeComponentManager.spawnComponent(character);
    }
public:
    AttributeComponentManager attributeComponentManager;
    EntityManager entityManager;
    PartialOrderPlanner partialOrderPlanner;

    Entity character;

    AttributeComponentFixture() : Test(), attributeComponentManager()
    {}

    ~AttributeComponentFixture() {}

    std::vector<Operator> usePlanner(Operator start, Operator end, std::vector<Operator> ops, Entity entity)
    {
        std::vector<Operator> plan;
        std::vector<PartialOrderPlan> plans = partialOrderPlanner.findPartialOrderPlan(start, end, ops);

        if (plans.size() > 0)
        {
            std::vector<unsigned> totalOrderPlan = topologicalSort(plans[0], plans[0].start);

            for (auto step = totalOrderPlan.begin() + 1; step != totalOrderPlan.end() - 1; ++step)
            {
                std::unordered_map<unsigned, Operator>::iterator op = plans[0].steps.find(*step);
                if (op != plans[0].steps.end())
                {
                    plan.push_back(op->second);
                }
            }
        }

        return plan;
    }
};

TEST_F(AttributeComponentFixture, high_attribute_ranking_actions)
{
    Action steal("Steal", 0, 1, 1);
    steal.attributes[Attribute::CHARM] = 0.0f;
    steal.actionOperator = std::shared_ptr<Operator>(new Operator(steal.getName()));
    Action buy("Buy", 2, 1, 1);
    buy.attributes[Attribute::CHARM] = 0.8f;
    buy.actionOperator = std::shared_ptr<Operator>(new Operator(buy.getName()));
    Action borrow("Borrow(Never Return)", 1, 1, 1);
    borrow.attributes[Attribute::CHARM] = 0.4f;
    borrow.actionOperator = std::shared_ptr<Operator>(new Operator(borrow.getName()));

    std::unordered_map<int, Action*> actions;
    actions[steal.getId()] = &steal;
    actions[borrow.getId()] = &borrow;
    actions[buy.getId()] = &buy;

    attributeComponentManager.setAttribute(character, Attribute::CHARM, 1.0);
    std::vector<Operator> operators = attributeComponentManager.sortActionOperations(actions, character);

    ASSERT_EQ(operators.size(), 3);

    ASSERT_EQ(operators[0].name, buy.getName());
    ASSERT_EQ(operators[1].name, borrow.getName());
    ASSERT_EQ(operators[2].name, steal.getName());
}

TEST_F(AttributeComponentFixture, low_attribute_ranking_actions)
{
    Action steal("Steal", 0, 1, 1);
    steal.attributes[Attribute::CHARM] = 0.0f;
    steal.actionOperator = std::shared_ptr<Operator>(new Operator(steal.getName()));
    Action buy("Buy", 2, 1, 1);
    buy.attributes[Attribute::CHARM] = 0.8f;
    buy.actionOperator = std::shared_ptr<Operator>(new Operator(buy.getName()));
    Action borrow("Borrow(Never Return)", 1, 1, 1);
    borrow.attributes[Attribute::CHARM] = 0.4f;
    borrow.actionOperator = std::shared_ptr<Operator>(new Operator(borrow.getName()));

    std::unordered_map<int, Action*> actions;
    actions[steal.getId()] = &steal;
    actions[borrow.getId()] = &borrow;
    actions[buy.getId()] = &buy;

    attributeComponentManager.setAttribute(character, Attribute::CHARM, 0.0);
    std::vector<Operator> operators = attributeComponentManager.sortActionOperations(actions, character);

    ASSERT_EQ(operators.size(), 3);

    ASSERT_EQ(operators[2].name, buy.getName());
    ASSERT_EQ(operators[1].name, borrow.getName());
    ASSERT_EQ(operators[0].name, steal.getName());
}

TEST_F(AttributeComponentFixture, chosen_plan_actions_correspond_to_attributes)
{
    int fishing = 0;
    int hasFishingBait = 1;
    int usingFishingBait = 2;

    Action catchFish("CatchFish", 0, 1, 1);
    catchFish.attributes[Attribute::INTELLIGENCE] = 0.4f;
    catchFish.actionOperator = std::shared_ptr<Operator>(new Operator(catchFish.getName()));
    catchFish.actionOperator->addedEffects = {fishing};
    catchFish.actionOperator->preconditions = {usingFishingBait};

    Action baitHook("BaitHook", 2, 1, 1);
    baitHook.attributes[Attribute::INTELLIGENCE] = 0.8f;
    baitHook.actionOperator = std::shared_ptr<Operator>(new Operator(baitHook.getName()));
    baitHook.actionOperator->addedEffects = {usingFishingBait};
    baitHook.actionOperator->preconditions = {hasFishingBait};

    Action catchNothing("CatchNothing", 1, 1, 1);
    catchNothing.attributes[Attribute::INTELLIGENCE] = 0.0f;
    catchNothing.actionOperator = std::shared_ptr<Operator>(new Operator(catchNothing.getName()));
    catchNothing.actionOperator->addedEffects = {fishing};

    Operator end("Goal");
    end.preconditions = {fishing};

    Operator start("Start");
    start.addedEffects = {hasFishingBait};

    std::unordered_map<int, Action*> actions;
    actions[catchFish.getId()] = &catchFish;
    actions[baitHook.getId()] = &baitHook;
    actions[catchNothing.getId()] = &catchNothing;

    attributeComponentManager.setAttribute(character, Attribute::INTELLIGENCE, 0.0);
    std::vector<Operator> operators = attributeComponentManager.sortActionOperations(actions, character);

    std::vector<Operator> plan = usePlanner(start,end, operators, character);

    ASSERT_EQ(plan.size(), 1);
    ASSERT_EQ(plan[0].name, catchNothing.getName());

    attributeComponentManager.setAttribute(character, Attribute::INTELLIGENCE, 1.0);
    operators = attributeComponentManager.sortActionOperations(actions, character);

    plan = usePlanner(start,end, operators, character);
    ASSERT_EQ(plan.size(), 2);
    ASSERT_EQ(plan[0].name, catchFish.getName());
    ASSERT_EQ(plan[1].name, baitHook.getName());
}

TEST_F(AttributeComponentFixture, chosen_plan_when_scores_match_is_shortest)
{
    int fishing = 0;
    int hasFishingBait = 1;
    int usingFishingBait = 2;

    Action catchFish("CatchFish", 0, 1, 1);
    catchFish.attributes[Attribute::INTELLIGENCE] = 0.0f;
    catchFish.actionOperator = std::shared_ptr<Operator>(new Operator(catchFish.getName()));
    catchFish.actionOperator->addedEffects = {fishing};
    catchFish.actionOperator->preconditions = {usingFishingBait};

    Action baitHook("BaitHook", 2, 1, 1);
    baitHook.attributes[Attribute::INTELLIGENCE] = 0.0f;
    baitHook.actionOperator = std::shared_ptr<Operator>(new Operator(baitHook.getName()));
    baitHook.actionOperator->addedEffects = {usingFishingBait};
    baitHook.actionOperator->preconditions = {hasFishingBait};

    Action catchNothing("CatchNothing", 1, 1, 1);
    catchNothing.attributes[Attribute::INTELLIGENCE] = 0.0f;
    catchNothing.actionOperator = std::shared_ptr<Operator>(new Operator(catchNothing.getName()));
    catchNothing.actionOperator->addedEffects = {fishing};

    Operator end("Goal");
    end.preconditions = {fishing};

    Operator start("Start");
    start.addedEffects = {hasFishingBait};

    std::unordered_map<int, Action*> actions;
    actions[catchFish.getId()] = &catchFish;
    actions[baitHook.getId()] = &baitHook;
    actions[catchNothing.getId()] = &catchNothing;

    attributeComponentManager.setAttribute(character, Attribute::INTELLIGENCE, 0.0);
    std::vector<Operator> operators = attributeComponentManager.sortActionOperations(actions, character);

    std::vector<Operator> plan = usePlanner(start,end, operators, character);

    ASSERT_EQ(plan.size(), 1);
    ASSERT_EQ(plan[0].name, catchNothing.getName());

    attributeComponentManager.setAttribute(character, Attribute::INTELLIGENCE, 1.0);
    operators = attributeComponentManager.sortActionOperations(actions, character);

    plan = usePlanner(start,end, operators, character);

    ASSERT_EQ(plan.size(), 1);
    ASSERT_EQ(plan[0].name, catchNothing.getName());
}
