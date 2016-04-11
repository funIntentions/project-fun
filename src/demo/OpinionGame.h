//
// Created by Dan on 4/2/2016.
//

#ifndef PARTIALORDERPLANNER_OPINIONGAME_H
#define PARTIALORDERPLANNER_OPINIONGAME_H

#include <framework/Game.h>
#include <EntityManager.h>
#include <iostream>
#include "components/ScheduleComponentManager.h"
#include "components/LocationComponentManager.h"
#include <fstream>
#include <rapidjson/document.h>
#include <math.h>
#include <components/PositionComponentManager.h>
#include <components/CharacterComponentManager.h>

class OpinionGame : public Game {
public:
    OpinionGame() : Game(800, 600, "Demo: Opinions"),
             _entityManager(new EntityManager()),
             _locationComponentManager(new LocationComponentManager()),
             _attributeComponentManager(new AttributeComponentManager()),
             _characterComponentManager(new CharacterComponentManager(_attributeComponentManager)),
             _positionComponentManager(new PositionComponentManager(_locationComponentManager, _characterComponentManager)),
             _actionManager(new ActionManager),
             _scheduleComponentManager(new ScheduleComponentManager(_actionManager, _characterComponentManager, _positionComponentManager, _attributeComponentManager))
    { }

    virtual void update(float period)
    {
        std::vector<int> addedEffects = _scheduleComponentManager->runSchedules(period);
        handleEffects(addedEffects);
    }

    virtual void render()
    {
        textRenderer->renderText("Bleep Bloop", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
private:
    std::shared_ptr<EntityManager> _entityManager;
    std::shared_ptr<LocationComponentManager> _locationComponentManager;
    std::shared_ptr<AttributeComponentManager> _attributeComponentManager;
    std::shared_ptr<CharacterComponentManager> _characterComponentManager;
    std::shared_ptr<PositionComponentManager> _positionComponentManager;
    std::shared_ptr<ActionManager> _actionManager;
    std::shared_ptr<ScheduleComponentManager> _scheduleComponentManager;
    std::vector<Entity> _entities;
    TextRenderer* textRenderer;

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
                        std::vector<Entity> locals;

                        /*for (auto connection = componentValue->value.Begin(); connection != componentValue->value.End(); ++connection)
                        {
                            std::string locationName = connection->GetString();
                            Entity locationEntity = _entityManager->getEntity(locationName);
                            locations.push_back(locationEntity);
                        }*/

                        _locationComponentManager->spawnComponent(*entity, entityName, locals);
                    }
                    else if (name == "attribute")
                    {
                        assert(componentValue->value.IsFloat());
                        float health = componentValue->value.GetFloat();
                        _attributeComponentManager->spawnComponent(*entity);
                        _attributeComponentManager->setAttribute(*entity, Attribute::Health, health);
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
                            std::string knownEntityName = known->GetString();
                            Entity knownEntity = _entityManager->getEntity(knownEntityName);
                            knownEntities.push_back(knownEntity);
                        }

                        _characterComponentManager->spawnComponent(*entity, groups);
                        _characterComponentManager->addKnowledge(*entity, knownEntities); // TODO: this should be called in a second pass, otherwise information will be missed
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
            PredicateTemplate predicateTemplate = _actionManager->getPredicateTemplate(id);

            std::cout << predicateTemplate.type << std::endl;
            for (std::string param : predicateTemplate.params)
            {
                std::cout << param << std::endl;
            }


            //Predicate predicate = _actionManager->getPredicate(id);

            /*if (predicate.type == "Location")
            {
                std::cout << "Entity Travelling: " << _entityManager->getName({predicate.params[1]}) << std::endl;
                _positionComponentManager->changeEntitiesLocation({predicate.params[1]}, {predicate.params[0]});
            }*/
        }
    }

    virtual void initialize()
    {
        Game::initialize();
        textRenderer = new TextRenderer(_graphics->createTextRenderer("fonts/SourceCodePro-Regular.ttf", 42));
        std::cout << std::chrono::high_resolution_clock::period::den << std::endl;
        _characterComponentManager->readGroups("data/World.json");
        Keyboard::keyPressedCallbackFunctions.push_back([this](int key) {this->keyPressed(key);});
        readEntities(*_entityManager);
    }

    void keyPressed(int key)
    {
        std::cout << "key pressed: " << key << std::endl;
    }

    void shutdown()
    {
        Game::shutdown();
    }
};


#endif //PARTIALORDERPLANNER_OPINIONGAME_H
