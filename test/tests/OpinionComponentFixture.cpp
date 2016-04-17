//
// Created by Dan on 3/27/2016.
//

#include <gtest/gtest.h>
#include <components/CharacterComponentManager.h>

class OpinionComponentFixture : public ::testing::Test {
protected:
    virtual void TearDown() {}

    virtual void SetUp()
    {
        Group humanGroup;
        humanGroup.name = "Human";
        Association humanAssociations;
        humanAssociations.category = "Food";
        humanAssociations.types = {"Fish"};
        humanGroup.associations.insert({humanAssociations.category, humanAssociations});

        Group merfolkGroup;
        merfolkGroup.name = "Merfolk";
        Association merfolkAssociations;
        merfolkAssociations.category = "Food";
        merfolkAssociations.types = {"Fish", "Human", "Plants"};
        merfolkGroup.associations.insert({merfolkAssociations.category, merfolkAssociations});

        Group fishGroup;
        fishGroup.name = "Fish";

        characterComponentManager.addGroup(humanGroup);
        characterComponentManager.addGroup(merfolkGroup);
        characterComponentManager.addGroup(fishGroup);

        fisherman = entityManager.create("Fisherman");
        merman = entityManager.create("Merman");
        fish = entityManager.create("Fish");
        seaweed = entityManager.create("Seaweed");
        lilypad = entityManager.create("Lilypad");

        std::vector<Type> groups = {"Merfolk"};
        characterComponentManager.spawnComponent(merman, groups);

        groups = {"Fish"};
        characterComponentManager.spawnComponent(fish, groups);

        groups = {"Human", "Fisher"};
        characterComponentManager.spawnComponent(fisherman, groups);

        groups = {"Plants"};
        characterComponentManager.spawnComponent(seaweed, groups);
        characterComponentManager.spawnComponent(lilypad, groups);

        std::vector<Entity> knownEntities {fish};
        characterComponentManager.addKnowledge(merman, knownEntities);
    }
public:
    std::shared_ptr<AttributeComponentManager> attributeComponentManager;
    CharacterComponentManager characterComponentManager;
    EntityManager entityManager;
    Entity fisherman;
    Entity merman;
    Entity fish;
    Entity seaweed;
    Entity lilypad;

    OpinionComponentFixture() : Test(), attributeComponentManager(new AttributeComponentManager()), characterComponentManager(attributeComponentManager)
    {}

    ~OpinionComponentFixture() {}
};

TEST_F(OpinionComponentFixture, adding_opinion)
{
    std::vector<Opinion> opinionsBefore = characterComponentManager.getOpinions(merman, "Food");

    std::vector<Opinion> opinionsAfter = opinionsBefore;
    opinionsAfter.push_back({1.0, fisherman});

    characterComponentManager.addKnowledge(merman, {fisherman});
    std::vector<Opinion> opinionsActuallyAfter = characterComponentManager.getOpinions(merman, "Food");

    if (opinionsAfter.size() != opinionsActuallyAfter.size())
        FAIL();

    for (unsigned i = 0; i < opinionsAfter.size(); ++i)
    {
        if (opinionsAfter[i].entity.id != opinionsActuallyAfter[i].entity.id)
            FAIL();
    }

    SUCCEED();
}
// TODO: implement
TEST_F(OpinionComponentFixture, remove_opinion)
{
    SUCCEED();
}

TEST_F(OpinionComponentFixture, get_opinion_for_known_entity)
{
    SUCCEED();
}

TEST_F(OpinionComponentFixture, get_opinion_for_unknown_entity)
{
    SUCCEED();
}

TEST_F(OpinionComponentFixture, lowering_opinion)
{
    characterComponentManager.addKnowledge(merman, {fisherman});
    std::vector<Opinion> opinionsBefore = characterComponentManager.getOpinions(merman, "Food");

    characterComponentManager.setOpinionVariance(merman, "Food", fish, 0.0);
    std::vector<Opinion> opinionsAfter = characterComponentManager.getOpinions(merman, "Food");

    // The order of the two opinions should be swapped due to the sorting by variance.
    ASSERT_EQ(opinionsBefore.size(), 2);

    ASSERT_EQ(opinionsBefore[0].entity.id, opinionsAfter[1].entity.id);
    ASSERT_EQ(opinionsBefore[1].entity.id, opinionsAfter[0].entity.id);
}

// TODO: implement
TEST_F(OpinionComponentFixture, improving_opinion)
{
    SUCCEED();
}

TEST_F(OpinionComponentFixture, opinions_ordered_by_variance)
{
    characterComponentManager.addKnowledge(merman, {fisherman});
    characterComponentManager.addKnowledge(merman, {seaweed});
    characterComponentManager.addKnowledge(merman, {lilypad});
    characterComponentManager.setOpinionVariance(merman, "Food", fish, 0.8);
    characterComponentManager.setOpinionVariance(merman, "Food", fisherman, 0.2);
    characterComponentManager.setOpinionVariance(merman, "Food", seaweed, 0.5);
    characterComponentManager.setOpinionVariance(merman, "Food", lilypad, 0.1);

    std::vector<Opinion> opinions = characterComponentManager.getOpinions(merman, "Food");

    ASSERT_EQ(opinions.size(), 4);

    ASSERT_EQ(opinions[0].entity.id, fish.id);
    ASSERT_EQ(opinions[1].entity.id, seaweed.id);
    ASSERT_EQ(opinions[2].entity.id, fisherman.id);
    ASSERT_EQ(opinions[3].entity.id, lilypad.id);
}
