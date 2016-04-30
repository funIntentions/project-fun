//
// Created by Dan on 4/16/2016.
//

#include <gtest/gtest.h>
#include <components/ScheduleComponentManager.h>

class ScheduleComponentFixture : public ::testing::Test {
protected:
    virtual void TearDown() {}

    virtual void SetUp()
    { }
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
    ASSERT_EQ(_scheduleComponentManager->getNumActions(), 4);

    Action* action = _scheduleComponentManager->getAction("StandAtGuardPost");
    ASSERT_EQ(action->parameters.size(), 2);
    ASSERT_EQ(action->actionOperator->preconditions.size(), 0);
    ASSERT_EQ(action->actionOperator->addedEffects.size(), 3);
    action = _scheduleComponentManager->getAction("TravelHome");
    ASSERT_EQ(action->parameters.size(), 2);
    ASSERT_EQ(action->actionOperator->preconditions.size(), 0);
    ASSERT_EQ(action->actionOperator->addedEffects.size(), 1);
    action = _scheduleComponentManager->getAction("Sleep");
    ASSERT_EQ(action->parameters.size(), 2);
    ASSERT_EQ(action->actionOperator->preconditions.size(), 1);
    ASSERT_EQ(action->actionOperator->addedEffects.size(), 1);
    action = _scheduleComponentManager->getAction("ServeTime");
    ASSERT_EQ(action->parameters.size(), 1);
    ASSERT_EQ(action->actionOperator->preconditions.size(), 1);
    ASSERT_EQ(action->actionOperator->addedEffects.size(), 0);
}

TEST_F(ScheduleComponentFixture, schedule_loading)
{
    ASSERT_EQ(_scheduleComponentManager->getNumActions(), 4);

    Schedule* schedule = _scheduleComponentManager->getSchedule("NightGuard");
    ASSERT_EQ(schedule->getNumEntries(), 2);
    schedule = _scheduleComponentManager->getSchedule("DayGuard");
    ASSERT_EQ(schedule->getNumEntries(), 2);
    schedule = _scheduleComponentManager->getSchedule("Prisoner");
    ASSERT_EQ(schedule->getNumEntries(), 1);
}

TEST_F(ScheduleComponentFixture, schedule_entry_change_multiple_entries)
{
    Entity character = _entityManager->create("Character");
    _scheduleComponentManager->spawnComponent(character, "NightGuard", 0, *_storyLogger);
    ScheduleInstance* scheduleInstance = _scheduleComponentManager->getEntitySchedule(character);
    ASSERT_EQ(scheduleInstance->currentEntry(), 1);
    scheduleInstance->startNextScheduleEntry();
    ASSERT_EQ(scheduleInstance->currentEntry(), 0);
}

TEST_F(ScheduleComponentFixture, schedule_entry_change_single_entry)
{
    Entity character = _entityManager->create("Character");
    _scheduleComponentManager->spawnComponent(character, "Prisoner", 0, *_storyLogger);
    ScheduleInstance* scheduleInstance = _scheduleComponentManager->getEntitySchedule(character);
    ASSERT_EQ(scheduleInstance->currentEntry(), 0);
    scheduleInstance->startNextScheduleEntry();
    ASSERT_EQ(scheduleInstance->currentEntry(), 0);
}

TEST_F(ScheduleComponentFixture, schedule_entry_24_hours_start_time)
{
    Entity character = _entityManager->create("Character");
    _scheduleComponentManager->spawnComponent(character, "NightGuard", 0, *_storyLogger);
    ScheduleInstance* scheduleInstance = _scheduleComponentManager->getEntitySchedule(character);
    ASSERT_EQ(scheduleInstance->currentEntry(), 1);
}

TEST_F(ScheduleComponentFixture, schedule_entry_0_hours_start_time)
{
    Entity character = _entityManager->create("Character");
    _scheduleComponentManager->spawnComponent(character, "NightGuard", 24, *_storyLogger);
    ScheduleInstance* scheduleInstance = _scheduleComponentManager->getEntitySchedule(character);
    ASSERT_EQ(scheduleInstance->currentEntry(), 1);
}

TEST_F(ScheduleComponentFixture, select_action_from_simple_schedule_entry)
{
    Entity character = _entityManager->create("Character");
    _scheduleComponentManager->spawnComponent(character, "Prisoner", 0, *_storyLogger);
    ScheduleInstance* scheduleInstance = _scheduleComponentManager->getEntitySchedule(character);
    int action = scheduleInstance->getActionIndex();
    scheduleInstance->chooseNewAction();
    int nextAction = scheduleInstance->getActionIndex();
    // Should be the same;
    ASSERT_EQ(action, nextAction);
}

TEST_F(ScheduleComponentFixture, select_actions_from_sequence_schedule_entry)
{
    Entity character = _entityManager->create("Character");
    _scheduleComponentManager->spawnComponent(character, "DayGuard", 21, *_storyLogger);
    ScheduleInstance* scheduleInstance = _scheduleComponentManager->getEntitySchedule(character);
    int action = scheduleInstance->getActionIndex();
    scheduleInstance->chooseNewAction();
    int nextAction = scheduleInstance->getActionIndex();
    // Should not be the same;
    ASSERT_NE(action, nextAction);
}

TEST_F(ScheduleComponentFixture, perform_action)
{
    Entity character = _entityManager->create("Character");
    _scheduleComponentManager->spawnComponent(character, "DayGuard", 21, *_storyLogger);
    ScheduleInstance* scheduleInstance = _scheduleComponentManager->getEntitySchedule(character);
    ActionInstance* actionInstance = scheduleInstance->chooseNewAction();

    double duration = actionInstance->getDuration();
    ASSERT_TRUE(actionInstance->perform(duration));
}
