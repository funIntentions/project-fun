//
// Created by Dan on 4/16/2016.
//

#include <gtest/gtest.h>
#include <components/ScheduleComponentManager.h>

class ScheduleComponentFixture : public ::testing::Test {
protected:
    virtual void TearDown() {}

    virtual void SetUp()
    {
        character = _entityManager->create("Character");
        //_scheduleComponentManager->spawnComponent(character, "Fisherman", 0.0);
    }
public:
    std::shared_ptr<EntityManager> _entityManager;
    std::shared_ptr<StoryLogger> _storyLogger;
    std::shared_ptr<LocationComponentManager> _locationComponentManager;
    std::shared_ptr<AttributeComponentManager> _attributeComponentManager;
    std::shared_ptr<TypeComponentManager> _typeComponentManager;
    std::shared_ptr<OpinionComponentManager> _opinionComponentManager;
    std::shared_ptr<PositionComponentManager> _positionComponentManager;
    std::shared_ptr<ActionManager> _actionManager;
    std::shared_ptr<OwnershipComponentManager> _ownershipComponentManager;
    std::shared_ptr<StateComponentManager> _stateComponentManager;
    std::shared_ptr<ScheduleComponentManager> _scheduleComponentManager;
    Entity character;

    ScheduleComponentFixture() : Test(),
                                 _entityManager(new EntityManager()),
                                 _storyLogger(new StoryLogger(_entityManager)),
                                 _locationComponentManager(new LocationComponentManager()),
                                 _attributeComponentManager(new AttributeComponentManager()),
                                 _typeComponentManager(new TypeComponentManager(_attributeComponentManager)),
                                 _opinionComponentManager(new OpinionComponentManager(_attributeComponentManager, _typeComponentManager, _storyLogger)),
                                 _positionComponentManager(new PositionComponentManager(_locationComponentManager, _opinionComponentManager)),
                                 _actionManager(new ActionManager),
                                 _ownershipComponentManager(new OwnershipComponentManager(_actionManager, _opinionComponentManager)),
                                 _stateComponentManager(new StateComponentManager()),
                                 _scheduleComponentManager(new ScheduleComponentManager(_actionManager,
                                                                                        _typeComponentManager,
                                                                                        _opinionComponentManager,
                                                                                        _positionComponentManager,
                                                                                        _ownershipComponentManager,
                                                                                        _attributeComponentManager,
                                                                                        _stateComponentManager))
    {}

    ~ScheduleComponentFixture() {}
};


TEST_F(ScheduleComponentFixture, action_loading)
{
    SUCCEED();
}

TEST_F(ScheduleComponentFixture, schedule_loading)
{
    SUCCEED();
}

TEST_F(ScheduleComponentFixture, schedule_entry_change_multiple_entries)
{
    SUCCEED();
}

TEST_F(ScheduleComponentFixture, schedule_entry_change_single_entry)
{
    SUCCEED();
}

TEST_F(ScheduleComponentFixture, schedule_entry_24_hours_start_time)
{
    SUCCEED();
}

TEST_F(ScheduleComponentFixture, schedule_entry_0_hours_start_time)
{
    SUCCEED();
}

TEST_F(ScheduleComponentFixture, select_action_from_simple_schedule_entry)
{
    SUCCEED();
}

TEST_F(ScheduleComponentFixture, select_actions_from_sequence_schedule_entry)
{
    SUCCEED();
}

TEST_F(ScheduleComponentFixture, select_actions_from_weighted_schedule_entry)
{
    SUCCEED();
}

TEST_F(ScheduleComponentFixture, perform_action)
{
    SUCCEED();
}
