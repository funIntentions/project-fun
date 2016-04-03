//
// Created by Dan on 4/2/2016.
//

#include <gtest/gtest.h>
#include <EntityManager.h>
#include <components/AttributeComponentManager.h>

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
    Entity character;

    AttributeComponentFixture() : Test(), attributeComponentManager()
    {}

    ~AttributeComponentFixture() {}
};

TEST_F(AttributeComponentFixture, high_attribute_ranking_actions)
{
    Action steal("Steal", 0, 1, 1);
    steal.attributes[Attribute::Morality] = 0.0f;
    steal.actionOperator = std::shared_ptr<Operator>(new Operator(steal.getName()));
    Action buy("Buy", 2, 1, 1);
    buy.attributes[Attribute::Morality] = 0.8f;
    buy.actionOperator = std::shared_ptr<Operator>(new Operator(buy.getName()));
    Action borrow("Borrow(Never Return)", 1, 1, 1);
    borrow.attributes[Attribute::Morality] = 0.4f;
    borrow.actionOperator = std::shared_ptr<Operator>(new Operator(borrow.getName()));

    std::unordered_map<int, Action*> actions;
    actions[steal.getId()] = &steal;
    actions[borrow.getId()] = &borrow;
    actions[buy.getId()] = &buy;

    attributeComponentManager.setAttribute(character, Attribute::Morality, 1.0);
    std::vector<Operator> operators = attributeComponentManager.sortActionOperations(actions, character);

    ASSERT_EQ(operators.size(), 3);

    ASSERT_EQ(operators[0].name, buy.getName());
    ASSERT_EQ(operators[1].name, borrow.getName());
    ASSERT_EQ(operators[2].name, steal.getName());
}

TEST_F(AttributeComponentFixture, low_attribute_ranking_actions)
{
    Action steal("Steal", 0, 1, 1);
    steal.attributes[Attribute::Morality] = 0.0f;
    steal.actionOperator = std::shared_ptr<Operator>(new Operator(steal.getName()));
    Action buy("Buy", 2, 1, 1);
    buy.attributes[Attribute::Morality] = 0.8f;
    buy.actionOperator = std::shared_ptr<Operator>(new Operator(buy.getName()));
    Action borrow("Borrow(Never Return)", 1, 1, 1);
    borrow.attributes[Attribute::Morality] = 0.4f;
    borrow.actionOperator = std::shared_ptr<Operator>(new Operator(borrow.getName()));

    std::unordered_map<int, Action*> actions;
    actions[steal.getId()] = &steal;
    actions[borrow.getId()] = &borrow;
    actions[buy.getId()] = &buy;

    attributeComponentManager.setAttribute(character, Attribute::Morality, 0.0);
    std::vector<Operator> operators = attributeComponentManager.sortActionOperations(actions, character);

    ASSERT_EQ(operators.size(), 3);

    ASSERT_EQ(operators[2].name, buy.getName());
    ASSERT_EQ(operators[1].name, borrow.getName());
    ASSERT_EQ(operators[0].name, steal.getName());
}
