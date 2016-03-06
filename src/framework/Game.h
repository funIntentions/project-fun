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
#include "Graphics.h"
#include "Input.h"
#include "WorldState.h"

class Game
{
public:
    Game() : _graphics(new Graphics()),
             _input(new Input()),
             _entityManager(new EntityManager()),
             _scheduleComponentManager(new ScheduleComponentManager()),
             _locationComponentManager(new LocationComponentManager()),
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
                _scheduleComponentManager->runSchedules(period.count(), *_actionManager);

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
    std::shared_ptr<ActionManager> _actionManager;

    void readEntities(EntityManager& entityManager, ScheduleComponentManager& scheduleComponentManager)
    {
        std::ifstream in("data/World.json");
        std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        const char* json = contents.c_str();

        rapidjson::Document document;
        document.Parse(json);

        const rapidjson::Value& a = document["entities"];
        assert(a.IsArray());

        for (auto entityData = a.Begin(); entityData != a.End(); ++entityData)
        {
            Entity entity = entityManager.create();

            std::string entityName;
            std::string scheduleName;

            auto nameItr = entityData->FindMember("name");
            if (nameItr != entityData->MemberEnd())
                entityName = nameItr->value.GetString();

            auto scheduleItr = entityData->FindMember("schedule");
            if (scheduleItr != entityData->MemberEnd())
                scheduleName = scheduleItr->value.GetString();

            scheduleComponentManager.spawnComponent(entity, scheduleName, 0);
        }
    }

    void initialize()
    {
        std::cout << std::chrono::high_resolution_clock::period::den << std::endl;

        _graphics->initialize();
        _input->initialize(_graphics->window->window);

        Keyboard::keyPressedCallbackFunctions.push_back([this](int key) {this->keyPressed(key);});
        _scheduleComponentManager->registerForAction("travel", [this](Action travelActionTemplate, Entity traveller) {return this->_locationComponentManager->determineActionsOfEntity(travelActionTemplate, traveller, _actionManager);});

        Entity village = _entityManager->create();
        Entity forest = _entityManager->create();
        Entity meadow = _entityManager->create();
        Entity swamp = _entityManager->create();
        Entity planes = _entityManager->create();
        Entity caves = _entityManager->create();

        _locationComponentManager->spawnComponent(village, "Village", {forest, meadow});
        _locationComponentManager->spawnComponent(meadow, "Meadow", {swamp, planes, village});
        _locationComponentManager->spawnComponent(forest, "Forest", {village, swamp});
        _locationComponentManager->spawnComponent(swamp, "Swamp", {meadow, forest, caves});
        _locationComponentManager->spawnComponent(caves, "Caves", {planes, swamp});
        _locationComponentManager->spawnComponent(planes, "Planes", {meadow, caves});

        readEntities(*_entityManager, *_scheduleComponentManager);


        //Action action("test", 0, 0, 0);
        //std::vector<Operator> operators = _locationComponentManager->determineActionsOfEntity(action, nowhere, _actionManager);
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
