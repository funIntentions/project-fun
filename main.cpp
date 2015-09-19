#include <iostream>
#include <vector>
#include <sstream>
#include "PartialOrderPlanner.h"
#include "tests/Sussman.h"
#include "tests/Briefcase.h"
#include "tests/FruitBowl.h"
#include "tests/Table.h"
#include "helper/extras.h"
#include "entity/EntityManager.h"
#include "component/ComponentManagers.h"
#include "tests/WorldLocation.h"
#include "tests/Krulg.h"

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
        PartialOrderPlan plan = plans[0];

        vector<long> totalOrderPlan = getTotalOrderPlan(plan);

        if (totalOrderPlan.size() > 0)
        {
            unordered_map<long, Operator>::const_iterator op = plan.steps.find(*totalOrderPlan.begin());
            if (op != plan.steps.end())
            {
                cout << op->second.name << endl;

                if (op->second.playerAction)
                {
                    alterWorldState(op->second);
                }
            }
        }
    }

    void alterWorldState(Operator op)
    {
        for (int state : op.addedEffects)
        {
            if (std::find(worldState.addedEffects.begin(), worldState.addedEffects.end(), state) != worldState.addedEffects.end())
            {
                worldState.addedEffects.push_back(state);
            }
        }

        for (int state : op.subtractedEffects)
        {
            std::vector<int>::const_iterator it = std::find(worldState.addedEffects.begin(), worldState.addedEffects.end(), state);
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
        else if (command.verb == "Travel")
        {
            int location = locationComponentManager.findLocation(command.noun);

            if (location != -1)
            {

                locationComponentManager.changeEntitiesLocation(player, (WorldLocation)location);
                entitiesCloseBy = locationComponentManager.getEntitiesInLocation((WorldLocation)location);

                /*for (Entity entity : entitiesCloseBy)
                {
                    descriptionComponentManager.examineEntity(entity);
                }*/

            }
        }
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
    // TODO: Building new operators should be made much quicker/easier than this
    // TODO: check how long the algorithm takes

    /*Krulg krulg;
    SussmanDomain sussmanDomain;
    BriefcaseDomain briefcaseDomain;
    FruitBowlDomain fruitBowlDomain;
    TableDomain tableDomain;
    PartialOrderPlanner partialOrderPlanner(krulg.operators);
    vector<PartialOrderPlan> plans = partialOrderPlanner.findPartialOrderPlan(krulg.start, krulg.finish);

    // Display plan's Causal links
    if (plans.size() > 0)
    {
        printPlanInformation(plans[0]);
    }*/

    Adventure adventure;

    adventure.beginAdventure();

    return 0;
}


