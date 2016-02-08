#include <iostream>
#include <vector>
#include <sstream>
#include "PartialOrderPlanner.h"
#include "tests/Sussman.h"
#include "tests/Briefcase.h"
#include "tests/FruitBowl.h"
#include "tests/Table.h"
#include "extras.h"
#include "EntityManager.h"
#include "ComponentManagers.h"
#include "tests/WorldLocation.h"
#include "tests/Krulg.h"
#include <chrono>

std::vector<std::string>& split(const std::string &s, char delimiter, std::vector<std::string>& tokens) {
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delimiter))
    {
        tokens.push_back(item);
    }

    return tokens;
}

struct Command
{
    Command()
    {

    }

    string verb;
    string noun;
};

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

struct World
{
private:
    EntityManager entityManager;
    DescriptionComponentManager descriptionComponentManager;
    LocationComponentManager locationComponentManager;
    std::vector<Entity> entitiesCloseBy;
    Entity player;
    Krulg krulg;
    Operator worldState;
    PartialOrderPlanner* partialOrderPlanner;
public:
    World()
    {
        partialOrderPlanner = new PartialOrderPlanner(krulg.operators);
        worldState = krulg.start;
        processPlan();

        player = entityManager.create();
        Entity marshland = entityManager.create();
        Entity ashplane = entityManager.create();
        Entity mysteryTroll = entityManager.create();

        descriptionComponentManager.spawnComponent(player, "Player", "I'm looking really good today.");
        descriptionComponentManager.spawnComponent(marshland, "Marshland", "Very quishy.");
        descriptionComponentManager.spawnComponent(ashplane, "Ashplane", "Try not to inhale.");

        locationComponentManager.spawnComponent(player, WorldLocation::Darkvoid);
        locationComponentManager.spawnComponent(marshland, WorldLocation::Marshland);
        locationComponentManager.spawnComponent(ashplane, WorldLocation::Ashplanes);
    }

    void processPlan()
    {
        vector<PartialOrderPlan> plans = partialOrderPlanner->findPartialOrderPlan(worldState, krulg.finish);

        if (plans.size() > 0)
        {
            PartialOrderPlan plan = plans[0];

            vector<long> totalOrderPlan = getTotalOrderPlan(plan);

            if (totalOrderPlan.size() > 2)
            {
                unordered_map<long, Operator>::iterator op = plan.steps.find(*(totalOrderPlan.end()-2));
                if (op != plan.steps.end())
                {
                    if ((op->second.name != "finish" && op->second.name != "start"))
                    {
                        alterWorldState(op->second);
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
        if (command.verb == "Examine")
        {
            descriptionComponentManager.examineEntityWithName(command.noun);
        }
        else if (command.verb == "Enslave")
        {
            WorldLocation playerLocation = locationComponentManager.getLocationForEntity(player);
            int location = locationComponentManager.findLocation(command.noun);

            if (location != -1)
            {
                Operator playerAction;
                if (playerLocation == location)
                {
                    if (playerLocation == Marshland)
                    {
                        playerAction = *krulg.enslaveMarshland;
                    }
                    else if (playerLocation == Ashplanes)
                    {
                        playerAction = *krulg.enslaveAshplane;
                    }
                }

                alterWorldState(playerAction);
            }
        }
        else if (command.verb == "Help")
        {
            WorldLocation playerLocation = locationComponentManager.getLocationForEntity(player);
            int location = locationComponentManager.findLocation(command.noun);

            if (location != -1)
            {
                Operator playerAction;
                if (playerLocation == location)
                {
                    if (playerLocation == Marshland)
                    {
                        playerAction = *krulg.helpMarshland;
                    }
                    else if (playerLocation == Ashplanes)
                    {
                        playerAction = *krulg.helpAshplane;
                    }
                }

                alterWorldState(playerAction);
            }
        }
        else if (command.verb == "Travel")
        {
            WorldLocation playerLocation = locationComponentManager.getLocationForEntity(player);
            int location = locationComponentManager.findLocation(command.noun);

            if (location != -1)
            {

                WorldLocation nextLocation = (WorldLocation)location;
                locationComponentManager.changeEntitiesLocation(player, (WorldLocation)location);
                entitiesCloseBy = locationComponentManager.getEntitiesInLocation((WorldLocation)location);

                /*for (Entity entity : entitiesCloseBy)
                {
                    descriptionComponentManager.examineEntity(entity);
                }*/
                Operator playerAction;

                if (playerLocation == Marshland)
                {
                    if (nextLocation == Ashplanes)
                    {
                        playerAction = *krulg.marshlandToAshplane;
                    }
                    else if (nextLocation == Darkvoid)
                    {
                        playerAction = *krulg.marshlandToDarkvoid;
                    }
                }
                else if (playerLocation == Ashplanes)
                {
                    if (nextLocation == Marshland)
                    {
                        playerAction = *krulg.ashplaneToMarshland;
                    }
                    else if (nextLocation == Darkvoid)
                    {
                        playerAction = *krulg.ashplaneToDarkvoid;
                    }
                }
                else if (playerLocation == Darkvoid)
                {
                    if (nextLocation == Marshland)
                    {
                        playerAction = *krulg.darkvoidToMarshland;
                    }
                    else if (nextLocation == Ashplanes)
                    {
                        playerAction = *krulg.darkvoidToAshplane;
                    }
                }

                alterWorldState(playerAction);
            }
        }

        processPlan();
    }

};

struct Adventure
{
    World* world;

    Adventure()
    {
        world = new World();
    }

    ~Adventure()
    {
        delete(world);
    }

    void beginAdventure()
    {
        string input;
        Command command;

        while (!finished(command))
        {
            input.clear();
            cout << "What will Krulg do now? ";
            getline(cin, input);
            command.verb.clear();
            command.noun.clear();

            command = parseInput(input, command);
            world->processCommand(command);
        }
    }

    bool finished(Command command)
    {
        return (command.verb == "QUIT");
    }
};

int main()
{
    cout << chrono::high_resolution_clock::period::den << endl;
    EntityManager entityManager;
    Entity entityOne = entityManager.create();
    Entity entityTwo = entityManager.create();

    ScheduleComponentManager scheduleComponentManager;

    double t = 0.0;

    scheduleComponentManager.spawnComponent(entityOne, t);
    //scheduleComponentManager.spawnComponent(entityTwo, t);

    double HOURS_IN_DAY = 24;

    auto currentTime = chrono::high_resolution_clock::now();
    while (t < 300)
    {
        auto newTime = chrono::high_resolution_clock::now();
        double frameTime = chrono::duration_cast<chrono::nanoseconds>(newTime - currentTime).count()/1000000000.0;
        currentTime = newTime;

        scheduleComponentManager.runSchedules(t, t + frameTime, frameTime);
        t += frameTime;

        if (t >= HOURS_IN_DAY) {
            t = fmod(t, HOURS_IN_DAY);
        }
    }


    /*parseJsonData();
    // TODO: Building new operators should be made much quicker/easier than this
    // TODO: check how long the algorithm takes
    PartialOrderPlanner partialOrderPlanner(briefcaseOperators);
    vector<PartialOrderPlan> plans = partialOrderPlanner.findPartialOrderPlan(briefcaseStart, briefcaseEnd);
    if (plans.size() > 0)
    {
        printPlanInformation(plans[0]);
    }*/

    /*Krulg krulg;
    SussmanDomain sussmanDomain;
    BriefcaseDomain briefcaseDomain;
    FruitBowlDomain fruitBowlDomain;
    TableDomain tableDomain;
    PartialOrderPlanner partialOrderPlanner(sussmanDomain.operators);
    vector<PartialOrderPlan> plans = partialOrderPlanner.findPartialOrderPlan(sussmanDomain.start, sussmanDomain.finish);

    // Display plan's Causal links
    if (plans.size() > 0)
    {
        printPlanInformation(plans[0]);
    }*/

    //Adventure adventure;

    //adventure.beginAdventure();

    return 0;
}


