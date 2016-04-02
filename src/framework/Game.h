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

class Game
{
public:
    Game() : _graphics(new Graphics()),
             _input(new Input())
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
                update(period.count());

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
            render();
            _graphics->swapBuffers();
            _input->pollForInput();

            ++frames;
        }

        shutdown();
    }

    virtual void update(float period) = 0;

    virtual void render() = 0;

protected:
    std::shared_ptr<Graphics> _graphics;
    std::shared_ptr<Input> _input;

    virtual void initialize()
    {
        _graphics->initialize();
        _input->initialize(_graphics->window->window);
    }

    virtual void shutdown()
    {
        _graphics->shutdown();
    }
};

    #endif //PARTIALORDERPLANNER_GAME_H
