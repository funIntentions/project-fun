//
// Created by dan on 14/07/15.
//

#ifndef PARTIALORDERPLANNER_COMPONENTMANAGERS_H
#define PARTIALORDERPLANNER_COMPONENTMANAGERS_H

#include <Schedules/Action.h>
#include <Schedules/Schedule.h>
#include <Schedules/ScheduleInstance.h>
#include <Schedules/ScheduleEntry.h>
#include <tests/Krulg.h>
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
    std::unordered_map<std::string, int> actionNameToIdMap;
    std::unordered_map<std::string, int> entryNameToIdMap;
    std::unordered_map<std::string, int> scheduleNameToIdMap;
    const std::string SIMPLE_SCHEDULE_ENTRY = "simple";

public:

    ScheduleComponentManager() : ComponentManager()
    {
        _data.size = 0;

        /*Action* read = new Action("read read", 0, 3, 6);
        Action* dance = new Action("dance dance", 1, 4, 8);
        actions.insert({read->getId(), read});
        actions.insert({dance->getId(), dance});

        ScheduleEntry* reading = new SimpleScheduleEntry("reading", 0, 20);
        ScheduleEntry* dancing = new SimpleScheduleEntry("dancing", 1, 10);
        scheduleEntryTemplates.insert({reading->getId(), reading});
        scheduleEntryTemplates.insert({dancing->getId(), dancing});

        reading->addAction(read);
        dancing->addAction(dance);

        Schedule* sunday = new Schedule("Sunday", 0);
        sunday->addEntry(reading);
        sunday->addEntry(dancing);

        schedules.insert({sunday->getId(), sunday});*/

        readActions();
        readSchedules();
        readEntities();
    }

    void readEntities()
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
            std::string entityName;
            std::string scheduleName;

            auto nameItr = entityData->FindMember("name");
            if (nameItr != entityData->MemberEnd())
                entityName = nameItr->value.GetString();

            auto scheduleItr = entityData->FindMember("schedule");
            if (scheduleItr != entityData->MemberEnd())
                scheduleName = scheduleItr->value.GetString();

            spawnComponent({0}, scheduleName, 0);
        }
    }

    void readActions()
    {
        std::ifstream in("data/Schedules.json");
        std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        const char* json = contents.c_str();

        rapidjson::Document document;
        document.Parse(json);

        const rapidjson::Value& a = document["actions"];
        assert(a.IsArray());

        for (auto action_itr = a.Begin(); action_itr != a.End(); ++action_itr)
        {
            assert(action_itr->IsObject());
            std::string name;
            double minDuration = 0;
            double maxDuration = 0;

            auto member_itr = action_itr->FindMember("name");
            if (member_itr != action_itr->MemberEnd())
                name = member_itr->value.GetString();

            member_itr = action_itr->FindMember("minDuration");
            if (member_itr != action_itr->MemberEnd())
                minDuration = member_itr->value.GetDouble();

            member_itr = action_itr->FindMember("maxDuration");
            if (member_itr != action_itr->MemberEnd())
                maxDuration = member_itr->value.GetDouble();

            Action* action = new Action(name, actions.size(), minDuration, maxDuration);
            actions.insert({action->getId(), action});
            actionNameToIdMap.insert({action->getName(), action->getId()});
        }

        delete json;
    }

    void readSchedules()
    {
        std::ifstream in("data/Schedules.json");
        std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        const char* json = contents.c_str();

        rapidjson::Document document;
        document.Parse(json);

        const rapidjson::Value& a = document["scheduleEntryTemplates"];
        assert(a.IsArray());

        for (auto entryTemplateData = a.Begin(); entryTemplateData != a.End(); ++entryTemplateData)
        {
            assert(entryTemplateData->IsObject());
            std::string name;
            std::string type;
            double startTime = 0;

            auto memberItr = entryTemplateData->FindMember("name");
            if (memberItr != entryTemplateData->MemberEnd())
                name = memberItr->value.GetString();

            memberItr = entryTemplateData->FindMember("type");
            if (memberItr != entryTemplateData->MemberEnd())
                type = memberItr->value.GetString();

            memberItr = entryTemplateData->FindMember("startTime");
            if (memberItr != entryTemplateData->MemberEnd())
                startTime = memberItr->value.GetDouble();

            ScheduleEntry* entry = scheduleEntryFactory(name, type, startTime);

            memberItr = entryTemplateData->FindMember("actions");
            if (memberItr != document.MemberEnd())
            {
                assert(memberItr->value.IsArray());
                for (auto actionData = memberItr->value.Begin(); actionData != memberItr->value.End(); ++actionData)
                {
                    assert(actionData->IsObject());
                    auto nameMember = actionData->FindMember("name");

                    if (nameMember != actionData->MemberEnd())
                    {
                        auto itr = actionNameToIdMap.find(nameMember->value.GetString());
                        if (itr != actionNameToIdMap.end())
                        {
                            int actionId = itr->second;
                            auto actionItr = actions.find(actionId);
                            if (actionItr != actions.end())
                            {
                                Action* action = actionItr->second;
                                entry->addAction(action);
                            }
                        }
                    }
                }
            }

            scheduleEntryTemplates.insert({entry->getId(), entry});
            entryNameToIdMap.insert({entry->getName(), entry->getId()});
        }

        const rapidjson::Value& scheduleData = document["schedules"];
        assert(scheduleData.IsArray());

        for (auto scheduleItr = scheduleData.Begin(); scheduleItr != scheduleData.End(); ++scheduleItr)
        {
            assert(scheduleItr->IsObject());
            std::string name;

            auto memberItr = scheduleItr->FindMember("name");
            if (memberItr != scheduleItr->MemberEnd())
                name = memberItr->value.GetString();

            Schedule* schedule = new Schedule(name, schedules.size());

            auto entryData = scheduleItr->FindMember("entries");
            if (entryData != scheduleItr->MemberEnd())
            {
                assert(entryData->value.IsArray());
                for (auto entryItr = entryData->value.Begin(); entryItr != entryData->value.End(); ++entryItr)
                {
                    std::string entryName;
                    std::string entryTemplate;
                    double startTime = 0;

                    auto entryMember = entryItr->FindMember("name");
                    if (entryMember != entryItr->MemberEnd())
                        name = entryMember->value.GetString();

                    entryMember = entryItr->FindMember("template");
                    if (entryMember != entryItr->MemberEnd())
                        entryTemplate = entryMember->value.GetString();

                    entryMember = entryItr->FindMember("startTime");
                    if (entryMember != entryItr->MemberEnd())
                        startTime = entryMember->value.GetDouble();

                    auto entryTemplateIdItr = entryNameToIdMap.find(entryTemplate);
                    if (entryTemplateIdItr != entryNameToIdMap.end())
                    {
                        auto scheduleEntry = scheduleEntryTemplates.find(entryTemplateIdItr->second);
                        if (scheduleEntry != scheduleEntryTemplates.end())
                        {
                            ScheduleEntry* entry = scheduleEntry->second->clone(name, scheduleEntryTemplates.size(), startTime);
                            schedule->addEntry(entry);
                        }
                    }
                }
            }

            schedules.insert({schedule->getId(), schedule});
            scheduleNameToIdMap.insert({schedule->getName(), schedule->getId()});
        }

        delete json;
    }


    ScheduleEntry* scheduleEntryFactory(const std::string& name, const std::string& type, double startTime)
    {
        if (type == SIMPLE_SCHEDULE_ENTRY)
        {
            return new SimpleScheduleEntry(name, scheduleEntryTemplates.size(), startTime);
        }
        else
        {
            printf("[WARNING] Unknown Schedule type %s for schedule %s\n", type.c_str(), name.c_str());
            return NULL;
        }
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
                std::cout << "Entity: " << i << " New Entry" << std::endl;
                _data.currentSchedule[i]->startNextScheduleEntry();
                _data.currentAction[i] = _data.currentSchedule[i]->chooseNewAction();
                std::cout << "Entity: " << i << " New Action 1: " << _data.currentAction[i]->getActionName() << std::endl;
            }

            if (_data.currentAction[i]->perform(deltaTime))
            {
                _data.currentAction[i] = _data.currentSchedule[i]->chooseNewAction();
                std::cout << "Entity: " << i << " New Action 2: " << _data.currentAction[i]->getActionName() << std::endl;
            }

        }
    }

    // TODO: Replace all this test data
    void spawnComponent(Entity entity, std::string scheduleName, double currentTime)
    {
        assert(schedules.size() > 0);

        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);

        auto Iditr = scheduleNameToIdMap.find(scheduleName);
        if (Iditr != scheduleNameToIdMap.end())
        {
            auto scheduleItr = schedules.find(Iditr->second);

            if (scheduleItr != schedules.end())
            {
                Schedule* schedule = scheduleItr->second;
                ScheduleInstance* scheduleInstance = new ScheduleInstance(schedule);
                scheduleInstance->chooseEntryForTime(currentTime);
                _data.currentSchedule.push_back(scheduleInstance);
                _data.currentAction.push_back(scheduleInstance->chooseNewAction());
            }

            ++_data.size;
        }
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
