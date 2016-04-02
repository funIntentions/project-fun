//
// Created by Dan on 4/2/2016.
//

#ifndef PARTIALORDERPLANNER_PLANNERGAME_H
#define PARTIALORDERPLANNER_PLANNERGAME_H

#include <framework/Game.h>

class PlannerGame : public Game {
public:
    PlannerGame() : Game(800, 600, "Demo: Planner")
    { }

    virtual void update(float period)
    {
    }

    virtual void render()
    {
        _graphics->textRenderer->renderText("Text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    }
private:
    virtual void initialize()
    {
        Game::initialize();

        std::cout << std::chrono::high_resolution_clock::period::den << std::endl;
        Keyboard::keyPressedCallbackFunctions.push_back([this](int key) {this->keyPressed(key);});
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


#endif //PARTIALORDERPLANNER_PLANNERGAME_H
