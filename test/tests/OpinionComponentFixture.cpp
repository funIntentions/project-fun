//
// Created by Dan on 3/27/2016.
//

#include <gtest/gtest.h>
#include <components/OpinionComponentManager.h>

class OpinionComponentFixture : public ::testing::Test {
protected:
    virtual void TearDown() {}

    virtual void SetUp()
    {
        Group humanGroup;
        humanGroup.name = "Human";
        Association humanAssociations;
        humanAssociations.category = "Food";
        humanAssociations.types = {{"Fish", 0.0}};
        humanGroup.associations.insert({humanAssociations.category, humanAssociations});

        Group merfolkGroup;
        merfolkGroup.name = "Merfolk";
        Association merfolkAssociations;
        merfolkAssociations.category = "Food";
        merfolkAssociations.types = {{"Fish", 0.0}, {"Human", 0.0}, {"Plants", 0.0}};
        merfolkGroup.associations.insert({merfolkAssociations.category, merfolkAssociations});

        Group fishGroup;
        fishGroup.name = "Fish";

        _typeComponentManager->addGroup(humanGroup);
        _typeComponentManager->addGroup(merfolkGroup);
        _typeComponentManager->addGroup(fishGroup);

        fisherman = _entityManager->create("Fisherman");
        merman = _entityManager->create("Merman");
        fish = _entityManager->create("Fish");
        seaweed = _entityManager->create("Seaweed");
        lilypad = _entityManager->create("Lilypad");

        std::vector<std::string> groups = {"Merfolk"};
        _typeComponentManager->spawnComponent(merman, groups);
        _opinionComponentManager->spawnComponent(merman, {});

        groups = {"Fish"};
        _typeComponentManager->spawnComponent(fish, groups);
        _opinionComponentManager->spawnComponent(fish, {});

        groups = {"Human", "Fisher"};
        _typeComponentManager->spawnComponent(fisherman, groups);
        _opinionComponentManager->spawnComponent(fisherman, {});

        groups = {"Plants"};
        _typeComponentManager->spawnComponent(seaweed, groups);
        _typeComponentManager->spawnComponent(lilypad, groups);
        _opinionComponentManager->spawnComponent(seaweed, {});
        _opinionComponentManager->spawnComponent(lilypad, {});

        std::vector<Entity> knownEntities {fish};
        _opinionComponentManager->addKnowledge(merman, knownEntities);
    }
public:
    std::shared_ptr<EntityManager> _entityManager;
    std::shared_ptr<AttributeComponentManager> _attributeComponentManager;
    std::shared_ptr<TypeComponentManager> _typeComponentManager;
    std::shared_ptr<StoryLogger> _storyLogger;
    std::shared_ptr<OpinionComponentManager> _opinionComponentManager;
    Entity fisherman;
    Entity merman;
    Entity fish;
    Entity seaweed;
    Entity lilypad;

    OpinionComponentFixture() : Test(),
                                _entityManager(new EntityManager()),
                                _attributeComponentManager(new AttributeComponentManager()),
                                _typeComponentManager(new TypeComponentManager(_attributeComponentManager)),
                                _storyLogger(new StoryLogger(_entityManager)),
                                _opinionComponentManager(new OpinionComponentManager(_attributeComponentManager, _typeComponentManager, _storyLogger))
    {}

    ~OpinionComponentFixture() {}
};

TEST_F(OpinionComponentFixture, adding_opinion)
{
    std::vector<Opinion> opinionsBefore = _opinionComponentManager->getOpinions(merman, "Food");

    std::vector<Opinion> opinionsAfter = opinionsBefore;
    opinionsAfter.push_back({1.0, fisherman});

    _opinionComponentManager->addKnowledge(merman, {fisherman});
    std::vector<Opinion> opinionsActuallyAfter = _opinionComponentManager->getOpinions(merman, "Food");

    if (opinionsAfter.size() != opinionsActuallyAfter.size())
        FAIL();

    for (unsigned i = 0; i < opinionsAfter.size(); ++i)
    {
        if (opinionsAfter[i].entity.id != opinionsActuallyAfter[i].entity.id)
            FAIL();
    }

    SUCCEED();
}

TEST_F(OpinionComponentFixture, remove_opinion)
{
    std::vector<Opinion> opinions = _opinionComponentManager->getOpinions(merman, "Food");
    ASSERT_EQ(opinions.size(), 1);

    _opinionComponentManager->removeOpinion(merman, "Food");

    opinions = _opinionComponentManager->getOpinions(merman, "Food");
    ASSERT_EQ(opinions.size(), 0);
}

TEST_F(OpinionComponentFixture, get_opinion_for_known_category)
{
    std::vector<Opinion> opinions = _opinionComponentManager->getOpinions(merman, "Food");

    ASSERT_EQ(opinions.size(), 1);
    ASSERT_EQ(opinions[0].entity.id, fish.id);
}

TEST_F(OpinionComponentFixture, get_opinion_for_unknown_category)
{
    std::vector<Opinion> opinions = _opinionComponentManager->getOpinions(merman, "Enemy");

    ASSERT_EQ(opinions.size(), 0);
}

TEST_F(OpinionComponentFixture, lowering_opinion)
{
    _opinionComponentManager->addKnowledge(merman, {fisherman});
    _opinionComponentManager->setOpinionVariance(merman, "Food", fisherman, 0.0f);
    _opinionComponentManager->setOpinionVariance(merman, "Food", fish, 1.0f);
    std::vector<Opinion> opinionsBefore = _opinionComponentManager->getOpinions(merman, "Food");

    _opinionComponentManager->setOpinionVariance(merman, "Food", fish, -1.0f);
    std::vector<Opinion> opinionsAfter = _opinionComponentManager->getOpinions(merman, "Food");

    // The order of the two opinions should be swapped due to the sorting by variance.
    ASSERT_EQ(opinionsBefore.size(), 2);

    ASSERT_EQ(opinionsBefore[0].entity.id, opinionsAfter[1].entity.id);
    ASSERT_EQ(opinionsBefore[1].entity.id, opinionsAfter[0].entity.id);
}

TEST_F(OpinionComponentFixture, improving_opinion)
{
    _opinionComponentManager->addKnowledge(merman, {fisherman});
    _opinionComponentManager->setOpinionVariance(merman, "Food", fisherman, 0.0f);
    _opinionComponentManager->setOpinionVariance(merman, "Food", fish, -1.0f);
    std::vector<Opinion> opinionsBefore = _opinionComponentManager->getOpinions(merman, "Food");

    _opinionComponentManager->setOpinionVariance(merman, "Food", fish, 1.0f);
    std::vector<Opinion> opinionsAfter = _opinionComponentManager->getOpinions(merman, "Food");

    // The order of the two opinions should be swapped due to the sorting by variance.
    ASSERT_EQ(opinionsBefore.size(), 2);

    ASSERT_EQ(opinionsBefore[0].entity.id, opinionsAfter[1].entity.id);
    ASSERT_EQ(opinionsBefore[1].entity.id, opinionsAfter[0].entity.id);
}

TEST_F(OpinionComponentFixture, opinions_ordered_by_variance)
{
    _opinionComponentManager->addKnowledge(merman, {fisherman});
    _opinionComponentManager->addKnowledge(merman, {seaweed});
    _opinionComponentManager->addKnowledge(merman, {lilypad});
    _opinionComponentManager->setOpinionVariance(merman, "Food", fish, 0.8);
    _opinionComponentManager->setOpinionVariance(merman, "Food", fisherman, 0.2);
    _opinionComponentManager->setOpinionVariance(merman, "Food", seaweed, 0.5);
    _opinionComponentManager->setOpinionVariance(merman, "Food", lilypad, 0.1);

    std::vector<Opinion> opinions = _opinionComponentManager->getOpinions(merman, "Food");

    ASSERT_EQ(opinions.size(), 4);

    ASSERT_EQ(opinions[0].entity.id, fish.id);
    ASSERT_EQ(opinions[1].entity.id, seaweed.id);
    ASSERT_EQ(opinions[2].entity.id, fisherman.id);
    ASSERT_EQ(opinions[3].entity.id, lilypad.id);
}
