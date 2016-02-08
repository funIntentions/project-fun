//
// Created by dan on 14/07/15.
//

#ifndef PARTIALORDERPLANNER_COMPONENTMANAGERS_H
#define PARTIALORDERPLANNER_COMPONENTMANAGERS_H

#include <Schedules/Action.h>
#include <Schedules/Schedule.h>
#include <Schedules/ScheduleInstance.h>
#include <Schedules/ScheduleEntry.h>
#include "ComponentManager.h"
#include "tests/WorldLocation.h"

class DescriptionComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<std::string> name;
        std::vector<std::string> description;
    };

    InstanceData _data;

public:

    DescriptionComponentManager() : ComponentManager()
    {
        _data.size = 0;
    }

    void examineEntityWithName(std::string name)
    {
        std::string description = "Mysterious... It seems I can't examine this... thing?";

        for (int index = 0; index < _data.size; ++index)
        {
            if (_data.name[index] == name)
            {
                description = _data.description[index];
            }
        }

        std::cout << description << std::endl;
    }

    void examineEntity(Entity entity)
    {
        // TODO: check for entity in component manager
        std::cout << _data.description[entity.id] << std::endl;
    }

    void spawnComponent(Entity entity, std::string name, std::string description)
    {
        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);
        _data.name.push_back(name);
        _data.description.push_back(description);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.name[i] = _data.name[last];
        _data.description[i] = _data.description[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.name.pop_back();
        _data.description.pop_back();

        --_data.size;
    }
};

class HealthComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<float> health;
    };

    InstanceData _data;

public:

    HealthComponentManager() : ComponentManager()
    {
        _data.size = 0;
    }

    float getHealth(Entity e)
    {
        return _data.health[e.id];
    }

    float setHealth(Entity e, float health)
    {
        _data.health[e.id] = health;
    }

    void spawnComponent(Entity entity, float health)
    {
        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);
        _data.health.push_back(health);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.health[i] = _data.health[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.health.pop_back();

        --_data.size;
    }
};

class LocationComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<WorldLocation> location;
    };

    InstanceData _data;
    const int numOfLocations = 3;
    std::string* locations;

public:

    LocationComponentManager() : ComponentManager()
    {
        locations = new std::string[numOfLocations];

        _data.size = 0;
        locations[Marshland] = "Marshland";
        locations[Ashplanes] = "Ashplanes";
        locations[Darkvoid] = "Darkvoid";
    }

    int findLocation(std::string locationName)
    {
        for (int location = 0; location < numOfLocations; ++location)
        {
            if (locations[location] == locationName)
            {
                return  location;
            }
        }
        return -1;
    }

    WorldLocation getLocationForEntity(Entity entity)
    {
        return _data.location[entity.id];
    }

    void changeEntitiesLocation(Entity entity, WorldLocation location)
    {
        _data.location[entity.id] = location;
    }

    vector<Entity> getEntitiesInLocation(WorldLocation location)
    {
        vector<Entity> entities;

        for (int index = 0; index < _data.size; ++index)
        {
            if (_data.location[index] == location)
            {
                entities.push_back(_data.entity[index]);
            }
        }

        return entities;
    }

    void spawnComponent(Entity entity, WorldLocation location)
    {
        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);
        _data.location.push_back(location);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.location[i] = _data.location[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.location.pop_back();

        --_data.size;
    }
};

class ScheduleComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<ActionInstance*> currentAction;
        std::vector<ScheduleInstance*> currentSchedule;
    };

    InstanceData _data;

    std::unordered_map<int, Schedule*> schedules;
    std::unordered_map<int, ScheduleEntry*> scheduleEntryTemplates;
    std::unordered_map<int, Action*> actions;

public:

    ScheduleComponentManager() : ComponentManager()
    {
        _data.size = 0;

        Action* read = new Action("read read", 0);
        Action* dance = new Action("dance dance", 1);
        actions.insert({read->getId(), read});
        actions.insert({dance->getId(), dance});

        ScheduleEntry* reading = new SimpleScheduleEntry("reading", 0, 0);
        ScheduleEntry* dancing = new SimpleScheduleEntry("dancing", 1, 12);
        scheduleEntryTemplates.insert({reading->getId(), reading});
        scheduleEntryTemplates.insert({dancing->getId(), dancing});

        reading->addAction(read);
        dancing->addAction(dance);

        Schedule* sunday = new Schedule("Sunday", 0);
        sunday->addEntry(reading);
        sunday->addEntry(dancing);

        schedules.insert({sunday->getId(), sunday});
    }

    ~ScheduleComponentManager()
    {
        for (auto it = schedules.begin(); it != schedules.end(); ++it)
        {
            delete it->second;
        }
        schedules.clear();

        for (auto it = scheduleEntryTemplates.begin(); it != scheduleEntryTemplates.end(); ++it)
        {
            delete it->second;
        }
        scheduleEntryTemplates.clear();

        for (auto it = actions.begin(); it != actions.end(); ++it)
        {
            delete it->second;
        }
        actions.clear();
    }

    void runSchedules(double lastTime, double currentTime, double deltaTime)
    {
        for (int i = 0; i < _data.size; ++i)
        {
            if (_data.currentSchedule[i]->timeIsUp(lastTime, currentTime))
            {
                _data.currentSchedule[i]->startNextScheduleEntry();
                _data.currentAction[i] = _data.currentSchedule[i]->chooseNewAction();
            }

            if (_data.currentAction[i]->perform(deltaTime))
                _data.currentAction[i] = _data.currentSchedule[i]->chooseNewAction();

        }
    }

    // TODO: Replace all this test data
    void spawnComponent(Entity entity)
    {
        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);
        ActionInstance* action = new ActionInstance(new Action("testAction", 0), 5, 0);
        _data.currentAction.push_back(action);
        ScheduleInstance* schedule = new ScheduleInstance(new Schedule("test", 0));
        _data.currentSchedule.push_back(schedule);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.currentAction[i] = _data.currentAction[last];
        _data.currentSchedule[i] = _data.currentSchedule[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.currentAction.pop_back();
        _data.currentSchedule.pop_back();

        --_data.size;
    }
};

#endif //PARTIALORDERPLANNER_COMPONENTMANAGERS_H
