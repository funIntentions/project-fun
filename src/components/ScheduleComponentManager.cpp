//
// Created by Dan on 2/13/2016.
//
#include "ScheduleComponentManager.h"
#include "Schedules/Schedule.h"
#include "OwnershipComponentManager.h"
#include <Schedules/ScheduleEntry.h>
#include <Schedules/ScheduleInstance.h>
#include <util/Extra.h>

ScheduleComponentManager::ScheduleComponentManager(std::shared_ptr<ActionManager> actionManager,
                                                   std::shared_ptr<CharacterComponentManager> characterComponentManager,
                                                   std::shared_ptr<PositionComponentManager> positionComponentManager,
                                                   std::shared_ptr<OwnershipComponentManager> ownershipComponentManager,
                                                   std::shared_ptr<AttributeComponentManager> attributeComponentManager) :
        ComponentManager(),
        _actionManager(actionManager),
        _characterComponentManager(characterComponentManager),
        _positionComponentManager(positionComponentManager),
        _ownershipComponentManager(ownershipComponentManager),
        _attributeComponentManager(attributeComponentManager),
        time(0.0)
{
    _data.size = 0;

    readActions(actionManager);
    readSchedules();
}

void ScheduleComponentManager::readActions(std::shared_ptr<ActionManager> actionManager)
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

        std::shared_ptr<Operator> newOperator(new Operator());
        newOperator->name = name;

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
                PredicateTemplate predicate;
                auto name_itr = itr->FindMember("name");
                predicate.type = name_itr->value.GetString();
                auto param_itr = itr->FindMember("params");

                for (auto param = param_itr->value.Begin(); param != param_itr->value.End(); ++param)
                {
                    assert(param->IsString());
                    predicate.params.push_back(param->GetString());
                }

                newOperator->preconditions.push_back(actionManager->addPredicateTemplate(predicate));
            }
        }

        member_itr = action_itr->FindMember("addedEffects");
        if (member_itr != document.MemberEnd())
        {
            assert(member_itr->value.IsArray());
            for (auto itr = member_itr->value.Begin(); itr != member_itr->value.End(); ++itr)
            {
                assert(itr->IsObject());
                PredicateTemplate predicate;
                auto name_itr = itr->FindMember("name");
                predicate.type = name_itr->value.GetString();
                auto param_itr = itr->FindMember("params");

                for (auto param = param_itr->value.Begin(); param != param_itr->value.End(); ++param)
                {
                    assert(param->IsString());
                    predicate.params.push_back(param->GetString());
                }

                newOperator->addedEffects.push_back(actionManager->addPredicateTemplate(predicate));
            }
        }

        member_itr = action_itr->FindMember("subtractedEffects");
        if (member_itr != document.MemberEnd())
        {
            assert(member_itr->value.IsArray());
            for (auto itr = member_itr->value.Begin(); itr != member_itr->value.End(); ++itr)
            {
                assert(itr->IsObject());
                PredicateTemplate predicate;
                auto name_itr = itr->FindMember("name");
                predicate.type = name_itr->value.GetString();
                auto param_itr = itr->FindMember("params");

                for (auto param = param_itr->value.Begin(); param != param_itr->value.End(); ++param)
                {
                    assert(param->IsString());
                    predicate.params.push_back(param->GetString());
                }

                newOperator->subtractedEffects.push_back(actionManager->addPredicateTemplate(predicate));
            }
        }

        operators.push_back(newOperator);
        action->actionOperator = newOperator;
        actions.insert({action->getId(), action});
        actionNameToIdMap.insert({action->getName(), action->getId()});
    }

    delete json;
}

void ScheduleComponentManager::readSchedules()
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
    else if (type == SEQUENCE_SCHEDULE_ENTRY)
    {
        return new SequenceScheduleEntry(name, scheduleEntryTemplates.size(), startTime);
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

void ScheduleComponentManager::registerForAction(std::string action, OperatorCallbackFunction function)
{
    operatorCallbackFunctionMap.insert({action, function});
}

std::vector<int> ScheduleComponentManager::runSchedules(double deltaTime)
{
    double lastTime = time;
    time += deltaTime;

    if (time >= 24) {
        time = fmod(time, 24);
    }

    std::vector<int> effects;

    for (unsigned i = 0; i < _data.size; ++i)
    {
        if (_attributeComponentManager->isDead(_data.entity[i]))
            continue;

        if (_data.currentSchedule[i]->timeIsUp(lastTime, time))
        {
            _data.queuedActions[i].clear();
            _data.currentSchedule[i]->startNextScheduleEntry();
            _data.queuedActions[i].push_back(_data.currentSchedule[i]->chooseNewAction());
        }

        if (_data.queuedActions[i].empty())
            _data.queuedActions[i].push_back(_data.currentSchedule[i]->chooseNewAction());

        mapParameters(_data.entity[i], _data.queuedActions[i].back());
        if (!preconditionsMet(_data.queuedActions[i].back()))
        {
            //std::cout << "Preconditions Not Met" << std::endl;
            usePlanner(_data.entity[i], _data.queuedActions[i].back()->getPreconditions());
        }
        else if (_data.queuedActions[i].back()->perform(deltaTime))
        {
            updateState(_data.queuedActions[i].back());
            //effects.insert(effects.end(), newEffects.begin(), newEffects.end());
            _data.queuedActions[i].pop_back();
        }
    }

    return effects;
}

void ScheduleComponentManager::usePlanner(Entity entity, std::vector<int> preconditions)
{
    Instance instance = lookup(entity);

    std::vector<Operator> ops;

    for (auto op : operators)
    {
        ops.push_back(*(op.get()));
    }

    Operator start;
    start.addedEffects = getState(entity);
    Operator end;
    end.preconditions = preconditions;
    std::vector<PartialOrderPlan> plans = planner.findPartialOrderPlan(start, end, ops);

    if (plans.size() > 0)
    {
        std::vector<unsigned> totalOrderPlan = topologicalSort(plans[0], plans[0].start);

        for (auto step = totalOrderPlan.begin() + 1; step != totalOrderPlan.end() - 1; ++step)
        {
            std::unordered_map<unsigned, Operator>::iterator op = plans[0].steps.find(*step);
            if (op != plans[0].steps.end())
            {
                auto actionId = actionNameToIdMap.find(op->second.name);
                if (actionId != actionNameToIdMap.end())
                {
                    auto action = actions.find(actionId->second);
                    if (action != actions.end())
                    {
                        _data.queuedActions[instance.i].push_back(action->second->createActionInstance());
                    }
                }

            }
        }
    }
}

std::vector<int> ScheduleComponentManager::getState(Entity entity)
{
    std::vector<int> state;

    if ((_positionComponentManager->lookup(entity)).i != -1)
    {
        Entity location = _positionComponentManager->getLocation(entity);
        std::vector<std::string> categories = _characterComponentManager->getCategories(entity, location);

        for (auto category : categories)
        {
            PredicateTemplate predicateTemplate;
            predicateTemplate.type = "Location";
            predicateTemplate.params.push_back(category);
            predicateTemplate.params.push_back("Self");

            int id = _actionManager->getPredicateId(predicateTemplate);
            if (id != -1)
                state.push_back(id);
            else
                std::cout << "Predicate Unknown" << std::endl;
        }
    }

    if ((_ownershipComponentManager->lookup(entity)).i != -1)
    {
        std::vector<unsigned> belongings = _ownershipComponentManager->getBelongings(entity);
        for (unsigned belonging : belongings)
        {
            std::vector<std::string> categories = _characterComponentManager->getCategories(entity, {belonging});
            for (auto category : categories)
            {
                PredicateTemplate predicateTemplate;
                predicateTemplate.type = "Has";
                predicateTemplate.params.push_back(category);
                predicateTemplate.params.push_back("Self");

                int id = _actionManager->getPredicateId(predicateTemplate);
                if (id != -1)
                    state.push_back(id);
                else
                    std::cout << "Predicate Unknown" << std::endl;
            }
        }
    }

    return state;
}

void ScheduleComponentManager::mapParameters(Entity entity, ActionInstance* action)
{
    for (std::string param : action->getParameters())
    {
        std::vector<Opinion> opinions = _characterComponentManager->getOpinions(entity, param);
        if (!opinions.empty())
            action->mappedParameters[param] = opinions.front().entity; // The first opinion is always prioritized
    }
}

bool ScheduleComponentManager::preconditionsMet(ActionInstance* action) {

    std::vector<int> preconditions = action->getPreconditions();
    for (int id : preconditions)
    {
        PredicateTemplate predicateTemplate = _actionManager->getPredicateTemplate(id);

        if (predicateTemplate.type == "Location")
        {
            auto locationItr = action->mappedParameters.find(predicateTemplate.params[0]);
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
            if (locationItr != action->mappedParameters.end() && entityItr != action->mappedParameters.end())
            {
                Entity desiredLocation = locationItr->second;
                Entity entity = entityItr->second;
                Entity location = _positionComponentManager->getLocation(entity);

                if (location.id != desiredLocation.id)
                    return false;
            }
        }
        else if (predicateTemplate.type == "Health")
        {
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
            if (entityItr != action->mappedParameters.end())
            {
                std::string desiredHealth = predicateTemplate.params[0];
                Entity entity = entityItr->second;
                std::string health = _attributeComponentManager->getHealthState(entity);

                if (health != desiredHealth)
                    return false;
            }
        }
        else if (predicateTemplate.type == "Has")
        {
            // TODO: implement precondition match for ownership
        }
        else
        {
            std::cout << "Unhandled predicate: " << predicateTemplate.type << std::endl;
        }
    }
    return true;
}

void ScheduleComponentManager::updateState(ActionInstance* action)
{
    std::vector<int> effects = action->getActionEffects();
    for (int id : effects)
    {
        PredicateTemplate predicateTemplate = _actionManager->getPredicateTemplate(id);

        if (predicateTemplate.type == "Location")
        {
            auto locationItr = action->mappedParameters.find(predicateTemplate.params[0]);
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
            if (locationItr != action->mappedParameters.end() && entityItr != action->mappedParameters.end())
            {
                Entity desiredLocation = locationItr->second;
                Entity entity = entityItr->second;

                _positionComponentManager->changeLocation(entity, desiredLocation);
            }
            else
                std::cout << "Error: Parameter Mapping Not Found" << std::endl;
        }
        else if (predicateTemplate.type == "Health")
        {
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
            if (entityItr != action->mappedParameters.end())
            {
                std::string desiredHealth = predicateTemplate.params[0];
                Entity entity = entityItr->second;

                _attributeComponentManager->setHealthState(entity, desiredHealth);
            }
            else
                std::cout << "Error: Parameter Mapping Not Found" << std::endl;
        }
        else if (predicateTemplate.type == "Opinion")
        {
            if (predicateTemplate.params.size() == 2) // TODO: Do I need this?
            {
                auto opinionEntityItr = action->mappedParameters.find(predicateTemplate.params[1]);
                if (opinionEntityItr != action->mappedParameters.end())
                {
                    float newVariance = (float) atof(predicateTemplate.params[0].c_str());
                    Entity opinionEntity = opinionEntityItr->second;

                    _characterComponentManager->setAllOpinionVariances(predicateTemplate.params[1], opinionEntity, newVariance);
                }
                else
                    std::cout << "Error: Parameter Mapping Not Found" << std::endl;
            }
            else if (predicateTemplate.params.size() == 3)
            {
                auto entityItr = action->mappedParameters.find(predicateTemplate.params[0]);
                auto opinionEntityItr = action->mappedParameters.find(predicateTemplate.params[2]);
                if (entityItr != action->mappedParameters.end() && opinionEntityItr != action->mappedParameters.end())
                {
                    float newVariance = (float) atof(predicateTemplate.params[1].c_str());
                    Entity entity = entityItr->second;
                    Entity opinionEntity = opinionEntityItr->second;

                    _characterComponentManager->setOpinionVariance(entity, predicateTemplate.params[2], opinionEntity, newVariance);
                }
                else
                    std::cout << "Error: Parameter Mapping Not Found" << std::endl;
            }
        }
        else if (predicateTemplate.type == "Has")
        {
            // TODO: implement update state for ownership
        }
        else
        {
            std::cout << "Unhandled predicate: " << predicateTemplate.type << std::endl;
        }
    }
}

void ScheduleComponentManager::spawnComponent(Entity entity, std::string scheduleName, double currentTime)
{
    assert(schedules.size() > 0);

    _map.emplace(entity.index(), _data.size);
    _data.entity.push_back(entity);

    auto scheduleId = scheduleNameToIdMap.find(scheduleName);
    if (scheduleId != scheduleNameToIdMap.end())
    {
        auto scheduleItr = schedules.find(scheduleId->second);

        if (scheduleItr != schedules.end())
        {
            Schedule* schedule = scheduleItr->second;
            ScheduleInstance* scheduleInstance = new ScheduleInstance(schedule);
            scheduleInstance->chooseEntryForTime(currentTime);
            _data.currentSchedule.push_back(scheduleInstance);
            _data.queuedActions.push_back({scheduleInstance->chooseNewAction()});
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
    _data.queuedActions[i] = _data.queuedActions[last];
    _data.currentSchedule[i] = _data.currentSchedule[last];

    _map[last_e.index()] =  i;
    _map.erase(e.index());

    _data.entity.pop_back();
    _data.queuedActions.pop_back();
    _data.currentSchedule.pop_back();

    --_data.size;
}

