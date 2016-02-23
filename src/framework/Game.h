//
// Created by Dan on 2/14/2016.
//

#ifndef PARTIALORDERPLANNER_GAME_H
#define PARTIALORDERPLANNER_GAME_H

#include <EntityManager.h>
#include <chrono>
#include <memory>
#include <iostream>
#include <ScheduleComponentManager.h>
#include <fstream>
#include <rapidjson/document.h>
#include <math.h>
#include "Graphics.h"
#include "Input.h"

class Game
{
public:
    Game() : _graphics(new Graphics()),
             _input(new Input()),
             _entityManager(new EntityManager()),
             _scheduleComponentManager(new ScheduleComponentManager())
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
                _scheduleComponentManager->runSchedules(time, time + period.count(), period.count());

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

            // TODO: handle hours in day within schedule system
            if (time >= 24) {
                time = fmod(time, 24);
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

    void readEntities(EntityManager& entityManager, ScheduleComponentManager& scheduleComponentManager)
    {
        std::ifstream in("data/WorldTest.json");
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

        readEntities(*_entityManager, *_scheduleComponentManager);
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
