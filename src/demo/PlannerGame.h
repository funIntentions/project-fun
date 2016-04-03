//
// Created by Dan on 4/2/2016.
//

#ifndef PARTIALORDERPLANNER_PLANNERGAME_H
#define PARTIALORDERPLANNER_PLANNERGAME_H

#include "framework/TextRenderer.h"
#include <framework/Game.h>
#include <PartialOrderPlanner.h>
#include <EntityManager.h>
#include <components/PlaceComponentManager.h>
#include <components/DescriptionComponentManager.h>
#include <domain/Adventure.h>
#include <util/Extra.h>

static const int GAME_WIDTH = 800;
static const int GAME_HEIGHT = 600;
static const int OUTPUT_MAX = 6;

static const glm::vec3 PLAYER_ACTION_COLOUR(0.5f, 0.8f, 0.5f);
static const glm::vec3 PLANNER_ACTION_COLOUR(0.8f, 0.8f, 0.2f);
static const glm::vec3 INPUT_COLOUR(0.5f, 0.8f, 0.2f);

struct Command
{
    Command()
    {

    }

    std::string verb;
    std::string noun;
};

struct ActionOutput
{
    std::string output;
    glm::vec3 colour;
};

class PlannerGame : public Game {
public:
    PlannerGame() : Game(GAME_WIDTH, GAME_HEIGHT, "Demo: Planner"), inputReady(false)
    { }

    virtual void update(float period)
    {
        if (inputReady)
        {
            Command command;
            command = parseInput(input, command);
            processCommand(command);
            input.clear();
            inputReady = false;
        }
    }

    virtual void render()
    {
        const float x = 0.0f;
        const float y = GAME_HEIGHT/1.2f;
        const float height = 42;

        for (int i = 0; i < output.size(); ++i)
        {
            textRenderer->renderText(output[i].output, x, y - (height * (i + 1)), 1.0f, output[i].colour);
        }

        // Render Input Text
        textRenderer->renderText(input, x, GAME_HEIGHT/1.2f, 1.0f, INPUT_COLOUR);
    }
private:
    EntityManager entityManager;
    DescriptionComponentManager descriptionComponentManager;
    PlaceComponentManager placeComponentManager;
    std::vector<Entity> entitiesCloseBy;
    PartialOrderPlanner* partialOrderPlanner;
    AdventureDomain adventureDomain;
    Operator worldState;
    Entity player;
    std::string input;
    std::vector<ActionOutput> output;
    bool inputReady;
    TextRenderer* textRenderer;

    virtual void initialize()
    {
        Game::initialize();

        std::cout << std::chrono::high_resolution_clock::period::den << std::endl;
        textRenderer = new TextRenderer(_graphics->createTextRenderer("fonts/SourceCodePro-Regular.ttf", 32));

        Keyboard::charPressedCallbackFunctions.push_back([this](unsigned codepoint) {this->textEntered(codepoint);});
        Keyboard::keyPressedCallbackFunctions.push_back([this](unsigned key) {this->keyPressed(key);});

        partialOrderPlanner = new PartialOrderPlanner();
        worldState = adventureDomain.start;
        processPlan();

        player = entityManager.create("krulg");
        Entity marshland = entityManager.create("marshland");
        Entity ashplane = entityManager.create("ashplane");
        Entity mysteryTroll = entityManager.create("troll");

        descriptionComponentManager.spawnComponent(player, "player", "I'm looking really good today.");
        descriptionComponentManager.spawnComponent(marshland, "marshland", "Very squishy.");
        descriptionComponentManager.spawnComponent(ashplane, "ashplane", "Try not to inhale.");

        placeComponentManager.spawnComponent(player, Place::Darkvoid);
        placeComponentManager.spawnComponent(marshland, Place::Marshland);
        placeComponentManager.spawnComponent(ashplane, Place::Ashplanes);
    }

    void processPlan()
    {
        std::vector<PartialOrderPlan> plans = partialOrderPlanner->findPartialOrderPlan(worldState, adventureDomain.finish, adventureDomain.operators);

        if (plans.size() > 0)
        {
            PartialOrderPlan plan = plans[0];

            std::vector<unsigned> totalOrderPlan = getTotalOrderPlan(plan);

            if (totalOrderPlan.size() > 2)
            {
                std::unordered_map<unsigned , Operator>::iterator op = plan.steps.find(*(totalOrderPlan.end()-2));
                if (op != plan.steps.end())
                {
                    if (adventureDomain.npcActions.find(op->second.name) != adventureDomain.npcActions.end()
                        && op->second.name != "finish" && op->second.name != "start")
                    {
                        alterWorldState(op->second);
                        addOutput(op->second.name, PLANNER_ACTION_COLOUR);
                        processPlan();
                    }
                }
            }
        }
    }

    void alterWorldState(Operator op)
    {
        for (int state : op.addedEffects)
        {
            if (std::find(worldState.addedEffects.begin(), worldState.addedEffects.end(), state) == worldState.addedEffects.end())
            {
                worldState.addedEffects.push_back(state);
            }
        }

        for (int state : op.subtractedEffects)
        {
            std::vector<int>::iterator it = std::find(worldState.addedEffects.begin(), worldState.addedEffects.end(), state);
            if (it != worldState.addedEffects.end())
            {
                worldState.addedEffects.erase(it);
            }
        }
    }

    void processCommand(Command command)
    {
        std::transform(command.verb.begin(), command.verb.end(), command.verb.begin(), ::tolower);
        std::transform(command.noun.begin(), command.noun.end(), command.noun.begin(), ::tolower);

        if (command.verb == "examine")
        {
            descriptionComponentManager.examineEntityWithName(command.noun);
        }
        else if (command.verb == "enslave")
        {
            Place playerLocation = placeComponentManager.getPlaceForEntity(player);
            int location = placeComponentManager.findPlace(command.noun);

            if (location != -1)
            {
                Operator playerAction;
                if (playerLocation == location)
                {
                    if (playerLocation == Marshland)
                    {
                        playerAction = *adventureDomain.enslaveMarshland;
                    }
                    else if (playerLocation == Ashplanes)
                    {
                        playerAction = *adventureDomain.enslaveAshplane;
                    }
                }

                alterWorldState(playerAction);
                addOutput(playerAction.name, PLAYER_ACTION_COLOUR);
            }
        }
        else if (command.verb == "help")
        {
            Place playerLocation = placeComponentManager.getPlaceForEntity(player);
            int location = placeComponentManager.findPlace(command.noun);

            if (location != -1)
            {
                Operator playerAction;
                if (playerLocation == location)
                {
                    if (playerLocation == Marshland)
                    {
                        playerAction = *adventureDomain.helpMarshland;
                    }
                    else if (playerLocation == Ashplanes)
                    {
                        playerAction = *adventureDomain.helpAshplane;
                    }
                }

                alterWorldState(playerAction);
                addOutput(playerAction.name, PLAYER_ACTION_COLOUR);
            }
        }
        else if (command.verb == "travel")
        {
            Place playerLocation = placeComponentManager.getPlaceForEntity(player);
            int location = placeComponentManager.findPlace(command.noun);

            if (location != -1)
            {

                Place nextLocation = (Place)location;
                placeComponentManager.changeEntitiesPlace(player, (Place)location);
                entitiesCloseBy = placeComponentManager.getEntitiesAtPlace((Place)location);

                //for (Entity entity : entitiesCloseBy)
                //{
                //    descriptionComponentManager.examineEntity(entity);
                //}
                Operator playerAction;

                if (playerLocation == Marshland)
                {
                    if (nextLocation == Ashplanes)
                    {
                        playerAction = *adventureDomain.marshlandToAshplane;
                    }
                    else if (nextLocation == Darkvoid)
                    {
                        playerAction = *adventureDomain.marshlandToDarkvoid;
                    }
                }
                else if (playerLocation == Ashplanes)
                {
                    if (nextLocation == Marshland)
                    {
                        playerAction = *adventureDomain.ashplaneToMarshland;
                    }
                    else if (nextLocation == Darkvoid)
                    {
                        playerAction = *adventureDomain.ashplaneToDarkvoid;
                    }
                }
                else if (playerLocation == Darkvoid)
                {
                    if (nextLocation == Marshland)
                    {
                        playerAction = *adventureDomain.darkvoidToMarshland;
                    }
                    else if (nextLocation == Ashplanes)
                    {
                        playerAction = *adventureDomain.darkvoidToAshplane;
                    }
                }

                alterWorldState(playerAction);
                addOutput(playerAction.name, PLAYER_ACTION_COLOUR);
            }
        }

        processPlan();
    }

    void addOutput(std::string actionOutput, glm::vec3 colour)
    {
        output.insert(output.begin(), {actionOutput, colour});

        if (output.size() > OUTPUT_MAX)
            output.pop_back();
    }

    std::vector<std::string>& split(const std::string &s, char delimiter, std::vector<std::string>& tokens) {
        std::stringstream ss(s);
        std::string item;

        while (std::getline(ss, item, delimiter))
        {
            tokens.push_back(item);
        }

        return tokens;
    }

    Command& parseInput(const std::string& input, Command& command)
    {
        std::vector<std::string> words;

        split(input, ' ', words);

        if (words.size() != 2)
        {
            command.verb = "invalid";
            command.noun = "llama";
        }
        else
        {
            command.verb = words.at(0);
            command.noun = words.at(1);
        }

        return command;
    }

    void keyPressed(int key)
    {
        if (key == GLFW_KEY_ENTER)
            inputReady = true;
        else if (key == GLFW_KEY_BACKSPACE && input.size() > 0)
            input = std::string(input.begin(), input.end() - 1);
    }

    void textEntered(unsigned codepoint)
    {
        std::string character = utf8chr(codepoint);
        input.append(character);
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


#endif //PARTIALORDERPLANNER_PLANNERGAME_H
