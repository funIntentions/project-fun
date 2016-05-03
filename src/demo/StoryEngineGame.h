//
// Created by Dan on 4/2/2016.
//

#ifndef PARTIALORDERPLANNER_OPINIONGAME_H
#define PARTIALORDERPLANNER_OPINIONGAME_H

#include <framework/Game.h>
#include <framework/EntityManager.h>
#include <iostream>
#include "components/ScheduleComponentManager.h"
#include "components/LocationComponentManager.h"
#include "Constants.h"
#include "StoryLogger.h"
#include <fstream>
#include <rapidjson/document.h>
#include <math.h>
#include <string>
#include <components/PositionComponentManager.h>
#include <components/OpinionComponentManager.h>
#include <framework/Cursor.h>
#include <util/Extra.h>

class StoryEngineGame : public Game {
public:
    StoryEngineGame() : Game(SE_GAME_WIDTH, SE_GAME_HEIGHT, "Demo: Story Engine"),
                    _entityManager(new EntityManager()),
                    _storyLogger(new StoryLogger(_entityManager)),
                    _stateComponentManager(new StateComponentManager()),
                    _locationComponentManager(new LocationComponentManager()),
                    _attributeComponentManager(new AttributeComponentManager()),
                    _typeComponentManager(new TypeComponentManager(_attributeComponentManager)),
                    _opinionComponentManager(new OpinionComponentManager(_attributeComponentManager, _typeComponentManager, _storyLogger)),
                    _positionComponentManager(new PositionComponentManager(_locationComponentManager, _opinionComponentManager)),
                    _actionManager(new ActionManager),
                    _ownershipComponentManager(new OwnershipComponentManager(_actionManager, _opinionComponentManager)),
                    _scheduleComponentManager(new ScheduleComponentManager(_actionManager,
                                                                           _typeComponentManager,
                                                                           _opinionComponentManager,
                                                                           _positionComponentManager,
                                                                           _ownershipComponentManager,
                                                                           _attributeComponentManager,
                                                                           _stateComponentManager)),
                    cursor(0.5f)
    { }

    virtual void update(float period)
    {
        if (!pause)
            _scheduleComponentManager->runSchedules(period, *_storyLogger);
    }

    virtual void render()
    {
        const float x = 42.0f;
        const float y = GAME_HEIGHT/1.2f;
        const float height = 14;

        double time = _scheduleComponentManager->getTimeOfDay();
        std::string timeText = time_format(time);
        textRenderer->renderText("Time: " + timeText, GAME_WIDTH/2, height, 1.0f, TIME_COLOUR);
        textRenderer->renderText("Events", GAME_WIDTH/4, height * 2, 1.0f, TIME_COLOUR);
        textRenderer->renderText("Character State", GAME_WIDTH/1.3f, height * 2, 1.0f, TIME_COLOUR);

        for (int i = 0; i < _storyLogger->events.size(); ++i)
        {
            textRenderer->renderText(_storyLogger->events[i].time + ": " + _storyLogger->events[i].description, x, y - (height * (i + 1)), 1.0f, PLANNER_ACTION_COLOUR);
        }

        int i = 0;
        for (auto state : _storyLogger->states)
        {
            textRenderer->renderText(state.second.description, GAME_WIDTH/1.3f - x, y - (height * (i + 1)), 1.0f, PLANNER_ACTION_COLOUR);
            ++i;
        }

        // Render Input Text
        //textRenderer->renderText(input + cursor.displayCursor(), x, GAME_HEIGHT/1.2f, 1.0f, INPUT_COLOUR);
    }
private:
    std::shared_ptr<EntityManager> _entityManager;
    std::shared_ptr<StoryLogger> _storyLogger;
    std::shared_ptr<StateComponentManager> _stateComponentManager;
    std::shared_ptr<LocationComponentManager> _locationComponentManager;
    std::shared_ptr<AttributeComponentManager> _attributeComponentManager;
    std::shared_ptr<TypeComponentManager> _typeComponentManager;
    std::shared_ptr<OpinionComponentManager> _opinionComponentManager;
    std::shared_ptr<PositionComponentManager> _positionComponentManager;
    std::shared_ptr<ActionManager> _actionManager;
    std::shared_ptr<OwnershipComponentManager> _ownershipComponentManager;
    std::shared_ptr<ScheduleComponentManager> _scheduleComponentManager;
    std::vector<Entity> _entities;
    TextRenderer* textRenderer;
    std::string input;
    std::vector<ActionOutput> output;
    bool inputReady;
    bool pause;
    Cursor cursor;

    void readEntities(EntityManager& entityManager)
    {
        std::ifstream in("data/World.json");
        std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        const char* json = contents.c_str();

        rapidjson::Document document;
        document.Parse(json);

        const rapidjson::Value& a = document["entities"];
        assert(a.IsArray());

        // First create all entities
        for (auto entityData = a.Begin(); entityData != a.End(); ++entityData)
        {

            std::string entityName;
            //std::string scheduleName;

            auto nameItr = entityData->FindMember("name");
            if (nameItr != entityData->MemberEnd())
                entityName = nameItr->value.GetString();

            Entity entity = entityManager.create(entityName);
            _entities.push_back(entity);

            std::cout << "New Entity: " << entityName << " - " << entity.id << std::endl;
        }

        // Next setup their components
        auto entity = _entities.begin();
        for (auto entityData = a.Begin(); entityData != a.End() && entity != _entities.end(); ++entityData)
        {
            std::string entityName;
            auto nameItr = entityData->FindMember("name");
            if (nameItr != entityData->MemberEnd())
                entityName = nameItr->value.GetString();

            auto components = entityData->FindMember("components");

            if (components != document.MemberEnd())
            {
                assert(components->value.IsArray());
                for (auto component = components->value.Begin(); component != components->value.End(); ++component)
                {
                    assert(component->IsObject());
                    auto componentName = component->FindMember("name");

                    std::string name = componentName->value.GetString();
                    auto componentValue = component->FindMember("value");
                    if (name == "schedule")
                    {
                        assert(componentValue->value.IsString());
                        std::string scheduleName = componentValue->value.GetString();
                        _scheduleComponentManager->spawnComponent(*entity, scheduleName, 0.0, *_storyLogger);
                    }
                    else if (name == "position")
                    {
                        assert(componentValue->value.IsString());
                        std::string positionName = componentValue->value.GetString();
                        _positionComponentManager->spawnComponent(*entity, _entityManager->getEntity(positionName));
                    }
                    else if (name == "state")
                    {
                        assert(componentValue->value.IsString());
                        std::string health = componentValue->value.GetString();
                        if (health == "Alive")
                            _stateComponentManager->spawnComponent(*entity, State::Health::Alive, State::Activity::None);
                        else
                            _stateComponentManager->spawnComponent(*entity, State::Health::Dead, State::Activity::None);
                    }
                    else if (name == "ownership")
                    {
                        _ownershipComponentManager->spawnComponent(*entity);

                        if (componentValue != component->MemberEnd() && componentValue->value.IsString())
                        {
                            Entity owner = _entityManager->getEntity(componentValue->value.GetString());
                            _ownershipComponentManager->giveOwnership(owner, *entity);
                        }
                    }
                    else if (name == "location")
                    {
                        assert(componentValue->value.IsArray());
                        std::vector<Entity> locals;

                        _locationComponentManager->spawnComponent(*entity, entityName, locals);
                    }
                    else if (name == "attribute")
                    {
                        assert(componentValue->value.IsArray());
                        auto attributes = componentValue->value.GetArray();
                        assert(attributes.Size() == Attribute::NumberOfAttributes);
                        float aggression = attributes[0].GetFloat();
                        float charm = attributes[1].GetFloat();
                        float intelligence = attributes[2].GetFloat();

                        _attributeComponentManager->spawnComponent(*entity);
                        _attributeComponentManager->setAttribute(*entity, Attribute::AGGRESSION, aggression);
                        _attributeComponentManager->setAttribute(*entity, Attribute::CHARM, charm);
                        _attributeComponentManager->setAttribute(*entity, Attribute::INTELLIGENCE, intelligence);
                    }
                    else if (name == "type")
                    {
                        assert(componentValue->value.IsArray());
                        std::vector<std::string> groups;
                        for (auto type = componentValue->value.Begin(); type != componentValue->value.End(); ++type)
                        {
                            std::string typeName = type->GetString();
                            groups.push_back(typeName);
                        }

                        _typeComponentManager->spawnComponent(*entity, groups);
                    }
                    else if (name == "opinion") // TODO: this should be called in a second pass, otherwise information will be missed
                    {
                        auto knowledge = component->FindMember("knowledge");
                        assert(knowledge->value.IsArray());
                        std::vector<Entity> knownEntities;
                        for (auto known = knowledge->value.Begin(); known != knowledge->value.End(); ++known)
                        {
                            std::string knownEntityName = known->GetString();
                            Entity knownEntity = _entityManager->getEntity(knownEntityName);
                            knownEntities.push_back(knownEntity);
                        }

                        _opinionComponentManager->spawnComponent(*entity, knownEntities);
                    }
                }
            }
            ++entity;
        }
    }

    virtual void initialize()
    {
        Game::initialize();
        textRenderer = new TextRenderer(_graphics->createTextRenderer("fonts/SourceCodePro-Regular.ttf", 12));
        std::cout << std::chrono::high_resolution_clock::period::den << std::endl;
        _typeComponentManager->readGroups("data/World.json");

        //Keyboard::charPressedCallbackFunctions.push_back([this](unsigned codepoint) {this->textEntered(codepoint);});
        Keyboard::keyPressedCallbackFunctions.push_back([this](unsigned key) {this->keyPressed(key);});
        //Keyboard::keyRepeatedCallbackFunctions.push_back([this](unsigned key) {this->keyPressed(key);});

        readEntities(*_entityManager);
    }

    void keyPressed(int key)
    {
        if (key == GLFW_KEY_ENTER)
            pause = !pause;
    }

    void textEntered(unsigned codepoint)
    {
        std::string character = utf8chr(codepoint);
        input.append(character);
        cursor.display();
    }

    std::string utf8chr(unsigned cp)
    {
        char c[5]={ 0x00,0x00,0x00,0x00,0x00 };
        if     (cp<=0x7F) { c[0] = cp;  }
        else if(cp<=0x7FF) { c[0] = (cp>>6)+192; c[1] = (cp&63)+128; }
        else if(0xd800<=cp && cp<=0xdfff) {} //invalid block of utf8
        else if(cp<=0xFFFF) { c[0] = (cp>>12)+224; c[1]= ((cp>>6)&63)+128; c[2]=(cp&63)+128; }
        else if(cp<=0x10FFFF) { c[0] = (cp>>18)+240; c[1] = ((cp>>12)&63)+128; c[2] = ((cp>>6)&63)+128; c[3]=(cp&63)+128; }
        return std::string(c);
    }

    void shutdown()
    {
        Game::shutdown();
    }
};


#endif //PARTIALORDERPLANNER_OPINIONGAME_H