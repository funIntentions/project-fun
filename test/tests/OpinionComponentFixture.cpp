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
        humanGroup.associations.insert({"Food", {"Fish"}});

        Group merfolkGroup;
        merfolkGroup.name = "Merfolk";
        merfolkGroup.associations.insert({"Food", {"Fish", "Human"}});

        Group fishGroup;
        fishGroup.name = "Fish";

        characterComponentManager.addGroup(humanGroup);
        characterComponentManager.addGroup(merfolkGroup);
        characterComponentManager.addGroup(fishGroup);

        fisherman = entityManager.create("Fisherman");
        merman = entityManager.create("Merman");
        fish = entityManager.create("Fish");

        std::vector<Type> groups = {"Merfolk"};
        characterComponentManager.spawnComponent(merman, groups);

        groups = {"Fish"};
        characterComponentManager.spawnComponent(fish, groups);

        groups = {"Human", "Fisher"};
        characterComponentManager.spawnComponent(fisherman, groups);

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

TEST_F(OpinionComponentFixture, changing_opinion)
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
