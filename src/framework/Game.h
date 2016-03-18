//
// Created by Dan on 2/14/2016.
//

#ifndef PARTIALORDERPLANNER_GAME_H
#define PARTIALORDERPLANNER_GAME_H

#include <EntityManager.h>
#include <chrono>
#include <memory>
#include <iostream>
#include "components/ScheduleComponentManager.h"
#include "components/LocationComponentManager.h"
#include <fstream>
#include <rapidjson/document.h>
#include <math.h>
#include <components/PositionComponentManager.h>
#include <components/CharacterComponentManager.h>
#include "Graphics.h"
#include "Input.h"
#include "WorldState.h"

// TODO: pull all the game logic in to an game object that extends from this.
class Game
{
public:
    Game() : _graphics(new Graphics()),
             _input(new Input()),
             _entityManager(new EntityManager()),
             _scheduleComponentManager(new ScheduleComponentManager()),
             _locationComponentManager(new LocationComponentManager()),
             _positionComponentManager(new PositionComponentManager(_locationComponentManager)),
             _characterComponentManager(new CharacterComponentManager()),
             _actionManager(new ActionManager)
    { }

    ~Game() {}

    void run()
    {
        initialize();

        const std::chrono::duration<float> period { 1.f / 60.f };

        double time = 0;
        int frames = 0;
        auto lastTicks = std::chrono::system_clock::now();
        auto fps = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed { 0 };

        while (_graphics->windowIsOpen())
        {
            auto ticks = std::chrono::system_clock::now();
            std::chrono::duration<float> interval { ticks - lastTicks };

            elapsed += interval;
            while (elapsed > period)
            {
                _scheduleComponentManager->updateStates(_positionComponentManager, _actionManager);
                std::vector<int> addedEffects = _scheduleComponentManager->runSchedules(period.count());
                handleEffects(addedEffects);

                elapsed -= period;
                time += period.count();
            }

            lastTicks = ticks;

            if (ticks - fps >= std::chrono::duration<float>(10))
            {
                std::cout << "frames: " <<frames << " duration: " << std::chrono::duration<float>(ticks - fps).count() << std::endl;
                std::cout << "fps: " << (((float) frames) / std::chrono::duration<float>(ticks - fps).count()) << std::endl;
                fps = std::chrono::system_clock::now();
                frames = 0;
            }

            _graphics->clearScreen();

            _graphics->swapBuffers();
            _input->pollForInput();

            ++frames;
        }

        shutdown();
    }

private:
    std::shared_ptr<Input> _input;
    std::shared_ptr<Graphics> _graphics;
    std::shared_ptr<EntityManager> _entityManager;
    std::shared_ptr<ScheduleComponentManager> _scheduleComponentManager;
    std::shared_ptr<LocationComponentManager> _locationComponentManager;
    std::shared_ptr<PositionComponentManager> _positionComponentManager;
    std::shared_ptr<CharacterComponentManager> _characterComponentManager;
    std::shared_ptr<ActionManager> _actionManager;
    std::vector<Entity> _entities;

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
                        _scheduleComponentManager->spawnComponent(*entity, scheduleName, 0.0);
                    }
                    else if (name == "position")
                    {
                        assert(componentValue->value.IsString());
                        std::string positionName = componentValue->value.GetString();
                        _positionComponentManager->spawnComponent(*entity, _entityManager->getEntity(positionName));
                    }
                    else if (name == "location")
                    {
                        assert(componentValue->value.IsArray());
                        std::vector<Entity> locations;
                        for (auto connection = componentValue->value.Begin(); connection != componentValue->value.End(); ++connection)
                        {
                            std::string locationName = connection->GetString();
                            Entity locationEntity = _entityManager->getEntity(locationName);
                            locations.push_back(locationEntity);
                        }

                        _locationComponentManager->spawnComponent(*entity, entityName, locations);
                    }
                    else if (name == "character")
                    {
                        assert(componentValue->value.IsArray());
                        std::vector<Type> groups;
                        for (auto type = componentValue->value.Begin(); type != componentValue->value.End(); ++type)
                        {
                            std::string typeName = type->GetString();
                            groups.push_back(typeName);
                        }

                        auto knowledge = component->FindMember("knowledge");
                        assert(knowledge->value.IsArray());
                        std::vector<Entity> knownEntities;
                        for (auto known = knowledge->value.Begin(); known != knowledge->value.End(); ++known)
                        {
                            std::string entityName = known->GetString();
                            Entity knownEntity = _entityManager->getEntity(entityName);
                            knownEntities.push_back(knownEntity);
                        }

                        _characterComponentManager->spawnComponent(*entity, groups);
                        _characterComponentManager->addKnowledge(*entity, knownEntities);
                    }
                }
            }
            ++entity;
        }
    }

    void handleEffects(std::vector<int> addedEffects)
    {
        for (int id : addedEffects)
        {
            Predicate predicate = _actionManager->getPredicate(id);

            if (predicate.type == "at")
            {
                std::cout << "Entity Travelling: " << _entityManager->getName({predicate.params[1]}) << std::endl;
                _positionComponentManager->changeEntitiesLocation({predicate.params[1]}, {predicate.params[0]});
            }
        }
    }

    void initialize()
    {
        std::cout << std::chrono::high_resolution_clock::period::den << std::endl;

        _graphics->initialize();
        _input->initialize(_graphics->window->window);

        Keyboard::keyPressedCallbackFunctions.push_back([this](int key) {this->keyPressed(key);});
        _scheduleComponentManager->registerForAction("travel", [this](Action travelActionTemplate, Entity traveller) {return this->_locationComponentManager->determineActionsOfEntity(travelActionTemplate, traveller, _actionManager);});

        readEntities(*_entityManager);
    }

    void keyPressed(int key)
    {
        std::cout << "key pressed: " << key << std::endl;
    }

    void shutdown()
    {
        _graphics->shutdown();
    }
};

    #endif //PARTIALORDERPLANNER_GAME_H
