//
// Created by Dan on 2/13/2016.
//
#include "ScheduleComponentManager.h"
#include "Schedules/Schedule.h"
#include "Schedules/Action.h"
#include <Schedules/ScheduleEntry.h>
#include <Schedules/ScheduleInstance.h>
#include <fstream>
#include <rapidjson/document.h>
#include <iostream>

ScheduleComponentManager::ScheduleComponentManager() : ComponentManager()
{
    _data.size = 0;

    readActions();
    readSchedules();
}

void ScheduleComponentManager::readActions()
{
    std::ifstream in("data/SchedulesTest.json");
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

        member_itr = action_itr->FindMember("params");
        if (member_itr != document.MemberEnd())
        {
            assert(member_itr->value.IsArray());
            for (auto param_itr = member_itr->value.Begin(); param_itr != member_itr->value.End(); ++param_itr)
            {
                assert(param_itr->IsObject());
                auto param_member = param_itr->FindMember("name");
                action->parameters.push_back(param_member->value.GetString());
            }
        }

        member_itr = action_itr->FindMember("positivePreconditions");
        if (member_itr != document.MemberEnd())
        {
            assert(member_itr->value.IsArray());
            for (auto itr = member_itr->value.Begin(); itr != member_itr->value.End(); ++itr)
            {
                assert(itr->IsObject());
                Predicate predicate;
                auto name_itr = itr->FindMember("name");
                predicate.type = name_itr->value.GetString();
                auto param_itr = itr->FindMember("params");

                for (auto param = param_itr->value.Begin(); param != param_itr->value.End(); ++param)
                {
                    assert(param->IsString());
                    predicate.params.push_back(param->GetString());
                }

                action->preconditions.push_back(predicate);
            }
        }

        member_itr = action_itr->FindMember("addedEffects");
        if (member_itr != document.MemberEnd())
        {
            assert(member_itr->value.IsArray());
            for (auto itr = member_itr->value.Begin(); itr != member_itr->value.End(); ++itr)
            {
                assert(itr->IsObject());
                Predicate predicate;
                auto name_itr = itr->FindMember("name");
                predicate.type = name_itr->value.GetString();
                auto param_itr = itr->FindMember("params");

                for (auto param = param_itr->value.Begin(); param != param_itr->value.End(); ++param)
                {
                    assert(param->IsString());
                    predicate.params.push_back(param->GetString());
                }

                action->addedEffects.push_back(predicate);
            }
        }

        member_itr = action_itr->FindMember("subtractedEffects");
        if (member_itr != document.MemberEnd())
        {
            assert(member_itr->value.IsArray());
            for (auto itr = member_itr->value.Begin(); itr != member_itr->value.End(); ++itr)
            {
                assert(itr->IsObject());
                Predicate predicate;
                auto name_itr = itr->FindMember("name");
                predicate.type = name_itr->value.GetString();
                auto param_itr = itr->FindMember("params");

                for (auto param = param_itr->value.Begin(); param != param_itr->value.End(); ++param)
                {
                    assert(param->IsString());
                    predicate.params.push_back(param->GetString());
                }

                action->subtractedEffects.push_back(predicate);
            }
        }

        actions.insert({action->getId(), action});
        actionNameToIdMap.insert({action->getName(), action->getId()});
    }

    delete json;
}

void ScheduleComponentManager::readSchedules()
{
    std::ifstream in("data/SchedulesTest.json");
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

ScheduleEntry* ScheduleComponentManager::scheduleEntryFactory(const std::string& name, const std::string& type, double startTime)
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

ScheduleComponentManager::~ScheduleComponentManager()
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

void ScheduleComponentManager::runSchedules(double lastTime, double currentTime, double deltaTime)
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

void ScheduleComponentManager::spawnComponent(Entity entity, std::string scheduleName, double currentTime)
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

void ScheduleComponentManager::destroy(unsigned i)
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

