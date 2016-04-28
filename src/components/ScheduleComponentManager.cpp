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
                                                   std::shared_ptr<TypeComponentManager> typeComponentManager,
                                                   std::shared_ptr<OpinionComponentManager> opinionComponentManager,
                                                   std::shared_ptr<PositionComponentManager> positionComponentManager,
                                                   std::shared_ptr<OwnershipComponentManager> ownershipComponentManager,
                                                   std::shared_ptr<AttributeComponentManager> attributeComponentManager,
                                                   std::shared_ptr<StateComponentManager> stateComponentManager) :
        ComponentManager(),
        _actionManager(actionManager),
        _typeComponentManager(typeComponentManager),
        _opinionComponentManager(opinionComponentManager),
        _positionComponentManager(positionComponentManager),
        _ownershipComponentManager(ownershipComponentManager),
        _attributeComponentManager(attributeComponentManager),
        _stateComponentManager(stateComponentManager),
        time(0.0),
        speed(0.5)
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
        float health = 0;
        float materialism = 0;
        float morality = 0;

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

        member_itr = action_itr->FindMember("attributes");
        if (member_itr != action_itr->MemberEnd())
        {
            assert(member_itr->value.IsArray());
            auto attributes = member_itr->value.GetArray();
            assert(attributes.Size() == Attribute::NumberOfAttributes);
            action->attributes[Attribute::AGGRESSION] = attributes[Attribute::AGGRESSION].GetFloat();
            action->attributes[Attribute::CHARM] = attributes[Attribute::CHARM].GetFloat();
            action->attributes[Attribute::INTELLIGENCE] = attributes[Attribute::INTELLIGENCE].GetFloat();
        }

        member_itr = action_itr->FindMember("params");
        if (member_itr != document.MemberEnd())
        {
            assert(member_itr->value.IsArray());
            for (auto param_itr = member_itr->value.Begin(); param_itr != member_itr->value.End(); ++param_itr)
            {
                assert(param_itr->IsString());
                action->parameters.push_back(param_itr->GetString());
            }
        }

        member_itr = action_itr->FindMember("positivePreconditions");
        if (member_itr != document.MemberEnd())
        {
            assert(member_itr->value.IsArray());
            for (auto itr = member_itr->value.Begin(); itr != member_itr->value.End(); ++itr)
            {
                assert(itr->IsObject());
                assert(itr->MemberCount() == 1);

                PredicateTemplate predicate;
                predicate.type = itr->MemberBegin()->name.GetString();
                auto params = itr->MemberBegin()->value.GetArray();

                for (auto param = params.Begin(); param != params.End(); ++param)
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
                assert(itr->MemberCount() == 1);

                PredicateTemplate predicate;
                predicate.type = itr->MemberBegin()->name.GetString();
                auto params = itr->MemberBegin()->value.GetArray();

                for (auto param = params.Begin(); param != params.End(); ++param)
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
                assert(itr->MemberCount() == 1);

                PredicateTemplate predicate;
                predicate.type = itr->MemberBegin()->name.GetString();
                auto params = itr->MemberBegin()->value.GetArray();

                for (auto param = params.Begin(); param != params.End(); ++param)
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
                assert(actionData->IsString());

                auto itr = actionNameToIdMap.find(actionData->GetString());
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

void ScheduleComponentManager::runSchedules(double deltaTime, StoryLogger& storyLogger)
{
    double lastTime = time;
    time += deltaTime * speed;

    if (time >= 24) {
        time = fmod(time, 24);
        storyLogger.newDay();
    }

    storyLogger.setTime(time);

    for (unsigned i = 0; i < _data.size; ++i)
    {
        Entity entity = _data.entity[i];
        if (_stateComponentManager->lookup(entity).i != -1 && _stateComponentManager->getHealth(entity) == State::Health::Dead) // Do not run schedules for the dead
        {
            _data.queuedActions[i].clear();
            storyLogger.logState(entity, "Dead", _data.queuedActions[i]);
            continue;
        }

        if (_data.currentSchedule[i]->timeIsUp(lastTime, time))
        {
            _data.queuedActions[i].clear();
            _data.currentSchedule[i]->startNextScheduleEntry();
            _data.queuedActions[i].push_back(_data.currentSchedule[i]->chooseNewAction());
            storyLogger.logState(entity, _data.currentSchedule[i]->getName(), _data.queuedActions[i]);
        }

        if (_data.queuedActions[i].empty())
        {
            _data.queuedActions[i].push_back(_data.currentSchedule[i]->chooseNewAction());
            storyLogger.logState(entity, _data.currentSchedule[i]->getName(), _data.queuedActions[i]);
        }

        mapParameters(_data.entity[i], _data.queuedActions[i].back());
        if (!preconditionsMet(_data.queuedActions[i].back()))
        {
            usePlanner(_data.entity[i], _data.queuedActions[i].back()->getPreconditions());
            storyLogger.logState(entity, _data.currentSchedule[i]->getName(), _data.queuedActions[i]);
        }
        else if (_data.queuedActions[i].back()->perform(deltaTime))
        {
            updateState(_data.queuedActions[i].back(), storyLogger);
            _data.queuedActions[i].pop_back();
            storyLogger.logState(entity, _data.currentSchedule[i]->getName(), _data.queuedActions[i]);
        }
    }
}

void ScheduleComponentManager::usePlanner(Entity entity, std::vector<int> preconditions)
{
    Instance instance = lookup(entity);

    std::vector<Operator> ops = _attributeComponentManager->sortActionOperations(actions, entity);

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

    auto opinionMap = _opinionComponentManager->getOpinions(entity);
    Opinion self;
    self.entity = entity;
    self.variance = 1.0;
    opinionMap.insert({"Self", {self}});

    for (auto opinions : opinionMap)
    {
        if (opinions.second.size() == 0)
            continue;

        Category entityCategory = opinions.first;
        Entity known = opinions.second.front().entity;

        if ((_positionComponentManager->lookup(known)).i != -1)
        {
            Entity entityLocation = _positionComponentManager->getLocation(entity);
            Entity knownLocation = _positionComponentManager->getLocation(known);
            if (entityLocation.id == knownLocation.id)
            {
                PredicateTemplate predicateTemplate;
                predicateTemplate.type = "Location";
                predicateTemplate.params.push_back("Same");
                predicateTemplate.params.push_back(entityCategory);
                predicateTemplate.params.push_back("Self");

                int id = _actionManager->getPredicateId(predicateTemplate);
                if (id != -1)
                    state.push_back(id);
                else
                    std::cout << "getState: Predicate Unknown" << std::endl;
            }
            else
            {
                PredicateTemplate predicateTemplate;
                predicateTemplate.type = "Location";
                predicateTemplate.params.push_back("Different");
                predicateTemplate.params.push_back(entityCategory);
                predicateTemplate.params.push_back("Self");

                int id = _actionManager->getPredicateId(predicateTemplate);
                if (id != -1)
                    state.push_back(id);
                else
                    std::cout << "getState: Predicate Unknown" << std::endl;
            }

            Entity location = _positionComponentManager->getLocation(known);
            std::vector<std::pair<Category, float>> locationCategories = _typeComponentManager->getCategories(entity, location);

            for (auto locationCategory : locationCategories)
            {
                PredicateTemplate predicateTemplate;
                predicateTemplate.type = "Location";
                predicateTemplate.params.push_back(locationCategory.first);
                predicateTemplate.params.push_back(entityCategory);

                int id = _actionManager->getPredicateId(predicateTemplate);
                if (id != -1)
                    state.push_back(id);
                else
                    std::cout << "getState: Predicate Unknown" << std::endl;
            }
        }

        if ((_ownershipComponentManager->lookup(known)).i != -1)
        {
            if (_ownershipComponentManager->isOwnedBy(entity, known))
            {
                PredicateTemplate predicateTemplate;
                predicateTemplate.type = "Has";
                predicateTemplate.params.push_back(entityCategory);
                predicateTemplate.params.push_back("Self");

                int id = _actionManager->getPredicateId(predicateTemplate);
                if (id != -1)
                    state.push_back(id);
                else
                    std::cout << "getState: Predicate Unknown" << std::endl;
            }
        }

        if ((_stateComponentManager->lookup(known)).i != -1)
        {
            {
                State::Health health = _stateComponentManager->getHealth(known);

                PredicateTemplate predicateTemplate;
                predicateTemplate.type = "Health";
                std::string healthParam = health == State::Alive ? "Alive" : "Dead";
                predicateTemplate.params.push_back(healthParam);
                predicateTemplate.params.push_back(entityCategory);

                int id = _actionManager->getPredicateId(predicateTemplate);
                if (id != -1)
                    state.push_back(id);
                else
                    std::cout << "getState: Predicate Unknown" << std::endl;
            }

            {
                State::Activity activity = _stateComponentManager->getState(known);

                PredicateTemplate predicateTemplate;
                predicateTemplate.type = "State";
                std::string stateParam;

                if (activity == State::Activity::Fishing)
                {
                    stateParam = "Fishing";
                }
                else if (activity == State::Activity::Hooked)
                {
                    stateParam = "Hooked";
                }
                else if (activity == State::Activity::Apprehended)
                {
                    stateParam = "Apprehended";
                }
                else if (activity == State::Activity::Confronted)
                {
                    stateParam = "Confronted";
                }
                else
                {
                    stateParam = "None";
                }

                predicateTemplate.params.push_back(stateParam);
                predicateTemplate.params.push_back(entityCategory);

                int id = _actionManager->getPredicateId(predicateTemplate);
                if (id != -1)
                    state.push_back(id);
                else
                    std::cout << "getState: Predicate Unknown" << std::endl;
            }
        }
    }

    /*std::set<Entity> knowledge = _opinionComponentManager->getKnowledge(entity);
    knowledge.insert(entity);

    for (Entity known : knowledge)
    {
        std::vector<std::string> entityCategories = _typeComponentManager->getCategories(entity, known);

        if ((_positionComponentManager->lookup(known)).i != -1)
        {
            Entity location = _positionComponentManager->getLocation(known);
            std::vector<std::string> locationCategories = _typeComponentManager->getCategories(entity, location);

            for (auto entityCategory : entityCategories)
            {
                for (auto locationCategory : locationCategories)
                {
                    PredicateTemplate predicateTemplate;
                    predicateTemplate.type = "Location";
                    predicateTemplate.params.push_back(locationCategory);
                    predicateTemplate.params.push_back(entityCategory);

                    int id = _actionManager->getPredicateId(predicateTemplate);
                    if (id != -1)
                        state.push_back(id);
                    else
                        std::cout << "getState: Predicate Unknown" << std::endl;
                }
            }
        }

        if ((_ownershipComponentManager->lookup(known)).i != -1)
        {
            std::vector<Entity> belongings = _ownershipComponentManager->getBelongings(known);
            for (Entity belonging : belongings)
            {
                std::vector<std::string> belongingCategories = _typeComponentManager->getCategories(entity, belonging);
                for (auto entityCategory : entityCategories)
                {
                    for (auto belongingCategory : belongingCategories)
                    {
                        PredicateTemplate predicateTemplate;
                        predicateTemplate.type = "Has";
                        predicateTemplate.params.push_back(belongingCategory);
                        predicateTemplate.params.push_back(entityCategory);

                        int id = _actionManager->getPredicateId(predicateTemplate);
                        if (id != -1)
                            state.push_back(id);
                        else
                            std::cout << "getState: Predicate Unknown" << std::endl;
                    }
                }
            }
        }

        if ((_stateComponentManager->lookup(known)).i != -1)
        {
            State::Health health = _stateComponentManager->getHealth(known);

            for (auto entityCategory : entityCategories)
            {
                PredicateTemplate predicateTemplate;
                predicateTemplate.type = "Health";
                std::string healthParam = health == State::Alive ? "Alive" : "Dead";
                predicateTemplate.params.push_back(healthParam);
                predicateTemplate.params.push_back(entityCategory);

                int id = _actionManager->getPredicateId(predicateTemplate);
                if (id != -1)
                    state.push_back(id);
                else
                    std::cout << "getState: Predicate Unknown" << std::endl;
            }
        }
    }*/

    return state;
}

void ScheduleComponentManager::mapParameters(Entity entity, ActionInstance* action)
{
    std::vector<std::string> parameters = action->getParameters();
    for (std::string param : parameters)
    {
        std::vector<Opinion> opinions = _opinionComponentManager->getOpinions(entity, param);
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

            std::string operationType = predicateTemplate.params[0];

            if (operationType == "Same")
            {
                auto entityOneItr = action->mappedParameters.find(predicateTemplate.params[1]);
                auto entityTwoItr = action->mappedParameters.find(predicateTemplate.params[2]);
                if (entityOneItr != action->mappedParameters.end() && entityTwoItr != action->mappedParameters.end())
                {
                    Entity entityOne = entityOneItr->second;
                    Entity entityTwo = entityTwoItr->second;
                    Entity entityOneLocation = _positionComponentManager->getLocation(entityOne);
                    Entity entityTwoLocation = _positionComponentManager->getLocation(entityTwo);

                    if (entityOneLocation.id != entityTwoLocation.id)
                        return false;
                }
            }
            else if (operationType == "Different")
            {
                auto entityOneItr = action->mappedParameters.find(predicateTemplate.params[1]);
                auto entityTwoItr = action->mappedParameters.find(predicateTemplate.params[2]);
                if (entityOneItr != action->mappedParameters.end() && entityTwoItr != action->mappedParameters.end())
                {
                    Entity entityOne = entityOneItr->second;
                    Entity entityTwo = entityTwoItr->second;
                    Entity entityOneLocation = _positionComponentManager->getLocation(entityOne);
                    Entity entityTwoLocation = _positionComponentManager->getLocation(entityTwo);

                    if (entityOneLocation.id == entityTwoLocation.id)
                        return false;
                }
            }
            else
            {
                std::string desiredCategory = predicateTemplate.params[0];
                auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
                auto selfItr = action->mappedParameters.find("Self");
                if (entityItr != action->mappedParameters.end())
                {
                    Entity entity = entityItr->second;
                    Entity self = selfItr->second;
                    Entity location = _positionComponentManager->getLocation(entity);
                    std::vector<std::pair<Category, float>> locationCategories = _typeComponentManager->getCategories(self, location);
                    unsigned numLocationCategories = locationCategories.size();
                    for (unsigned i = 0; i < locationCategories.size(); ++i)
                    {
                        if (locationCategories[i].first == desiredCategory)
                            break;
                        else if (i+1 == numLocationCategories)
                            return false;
                    }
                }
            }
        }
        else if (predicateTemplate.type == "Health")
        {
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
            if (entityItr != action->mappedParameters.end())
            {
                std::string desiredHealth = predicateTemplate.params[0];
                Entity entity = entityItr->second;
                if (_stateComponentManager->lookup(entity).i < 0)
                    return false;

                State::Health health = _stateComponentManager->getHealth(entity);

                if (!(health == State::Health::Alive && "Alive" == desiredHealth || health == State::Health::Dead && "Dead" == desiredHealth))
                    return false;
            }
        }
        else if (predicateTemplate.type == "State")
        {
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
            if (entityItr != action->mappedParameters.end())
            {
                std::string desiredState = predicateTemplate.params[0];
                Entity entity = entityItr->second;
                if (_stateComponentManager->lookup(entity).i < 0)
                    return false;

                State::Activity state = _stateComponentManager->getState(entity);

                if (!(state == State::Activity::Fishing && "Fishing" == desiredState
                      || state == State::Activity::Hooked && "Hooked" == desiredState
                      || state == State::Activity::Apprehended && "Apprehended" == desiredState
                      || state == State::Activity::Confronted && "Confronted" == desiredState
                      || state == State::Activity::None && "None" == desiredState))
                    return false;
            }
        }
        else if (predicateTemplate.type == "Has")
        {
            auto belongingItr = action->mappedParameters.find(predicateTemplate.params[0]);
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
            if (belongingItr != action->mappedParameters.end() && entityItr != action->mappedParameters.end())
            {
                Entity belonging = belongingItr->second;
                Entity entity = entityItr->second;

                if (!_ownershipComponentManager->isOwnedBy(entity, belonging))
                    return false;
            }
        }
        else
        {
            std::cout << "Unhandled predicate: " << predicateTemplate.type << std::endl;
        }
    }
    return true;
}

void ScheduleComponentManager::updateState(ActionInstance* action, StoryLogger& storyLogger)
{
    std::vector<int> effects = action->getActionEffects();
    for (int id : effects)
    {
        PredicateTemplate predicateTemplate = _actionManager->getPredicateTemplate(id);

        if (predicateTemplate.type == "Location")
        {
            std::string operationType = predicateTemplate.params[0];
            if (operationType == "Same")
            {
                auto entityOneItr = action->mappedParameters.find(predicateTemplate.params[1]);
                auto entityTwoItr = action->mappedParameters.find(predicateTemplate.params[2]);
                if (entityOneItr != action->mappedParameters.end() && entityTwoItr != action->mappedParameters.end())
                {
                    Entity entityOne = entityOneItr->second;
                    Entity entityTwo = entityTwoItr->second;
                    Entity desiredLocation = _positionComponentManager->getLocation(entityOne);

                    storyLogger.logEvent({"travelled to"}, {entityTwo, desiredLocation});
                    _positionComponentManager->changeLocation(entityTwo, desiredLocation);
                }
                else
                    std::cout << "Error: Parameter Mapping Not Found" << std::endl;
            }
            else
            {
                auto locationItr = action->mappedParameters.find(predicateTemplate.params[0]);
                auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
                if (locationItr != action->mappedParameters.end() && entityItr != action->mappedParameters.end())
                {
                    Entity desiredLocation = locationItr->second;
                    Entity entity = entityItr->second;

                    storyLogger.logEvent({"travelled to"}, {entity, desiredLocation});
                    _positionComponentManager->changeLocation(entity, desiredLocation);
                }
                else
                    std::cout << "Error: Parameter Mapping Not Found" << std::endl;
            }
        }
        else if (predicateTemplate.type == "Health")
        {
            auto selfItr = action->mappedParameters.find("Self");
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
            if (entityItr != action->mappedParameters.end() && selfItr != action->mappedParameters.end())
            {
                std::string desiredHealth = predicateTemplate.params[0];
                Entity entity = entityItr->second;
                Entity self = selfItr->second;

                if (desiredHealth == "Alive") // TODO: Remove string checks
                {
                    storyLogger.logEvent({"has come to life."}, {entity});
                    _stateComponentManager->setHealth(entity, State::Health::Alive);
                }
                else
                {
                    storyLogger.logEvent({"has killed"}, {self, entity});
                    _stateComponentManager->setHealth(entity, State::Health::Dead);
                }
            }
            else
                std::cout << "Error: Parameter Mapping Not Found" << std::endl;
        }
        else if (predicateTemplate.type == "State")
        {
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
            if (entityItr != action->mappedParameters.end())
            {
                std::string newState = predicateTemplate.params[0];
                Entity entity = entityItr->second;

                if (newState == "Fishing")
                {
                    storyLogger.logEvent({"is fishing"}, {entity});
                    _stateComponentManager->setState(entity, State::Activity::Fishing);
                }
                else if (newState == "Hooked")
                {
                    storyLogger.logEvent({"is on fish hook"}, {entity});
                    _stateComponentManager->setState(entity, State::Activity::Hooked);
                }
                else if (newState == "Apprehended")
                {
                    storyLogger.logEvent({"is apprehended"}, {entity});
                    _stateComponentManager->setState(entity, State::Activity::Apprehended);
                }
                else if (newState == "Confronted")
                {
                    _stateComponentManager->setState(entity, State::Activity::Confronted);
                }
                else
                {
                    _stateComponentManager->setState(entity, State::Activity::None);
                }
            }
        }
        else if (predicateTemplate.type == "Opinion")
        {
            std::string operationType = predicateTemplate.params[0];

            if (operationType == "Remove")
            {
                auto entityItr = action->mappedParameters.find(predicateTemplate.params[2]);
                auto opinionEntityItr = action->mappedParameters.find(predicateTemplate.params[1]);

                Entity entity = entityItr->second;

                _opinionComponentManager->removeOpinion(entity, predicateTemplate.params[1]);
                continue;
            }

            auto entityItr = action->mappedParameters.find(predicateTemplate.params[3]);
            auto opinionEntityItr = action->mappedParameters.find(predicateTemplate.params[2]);

            if (entityItr == action->mappedParameters.end() || opinionEntityItr == action->mappedParameters.end())
            {
                std::cout << "Error: Parameter Mapping Not Found" << std::endl;
                continue;
            }

            float value = (float) atof(predicateTemplate.params[1].c_str());
            Entity entity = entityItr->second;
            Entity opinionEntity = opinionEntityItr->second;

            if (operationType == "Set")
            {
                _opinionComponentManager->setOpinionVariance(entity, predicateTemplate.params[2], opinionEntity, value);
            }
            else if (operationType == "AdjustOthers")
            {
                _opinionComponentManager->adjustOtherOpinionVariances(entity, predicateTemplate.params[2], opinionEntity, value);
            }
            else if (operationType == "Adjust")
            {
                _opinionComponentManager->adjustOpinionVariance(entity, predicateTemplate.params[2], opinionEntity, value);
            }
        }
        else if (predicateTemplate.type == "Has")
        {
            auto belongingItr = action->mappedParameters.find(predicateTemplate.params[0]);
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);
            if (belongingItr != action->mappedParameters.end() && entityItr != action->mappedParameters.end())
            {
                Entity belonging = belongingItr->second;
                Entity entity = entityItr->second;

                storyLogger.logEvent({"has"}, {entity, belonging});
                _ownershipComponentManager->giveOwnership(entity, belonging);
            }
        }
        else if (predicateTemplate.type == "Sleeping")
        {

            auto entityItr = action->mappedParameters.find(predicateTemplate.params[0]);
            auto locationItr = action->mappedParameters.find(predicateTemplate.params[1]);

            if (entityItr != action->mappedParameters.end() && locationItr != action->mappedParameters.end())
            {
                Entity entity = entityItr->second;
                Entity location = locationItr->second;

                storyLogger.logEvent({"is sleeping at"}, {entity, location});
            }
        }
        else if (predicateTemplate.type == "Schedule")
        {
            auto entityItr = action->mappedParameters.find(predicateTemplate.params[1]);

            auto scheduleId = scheduleNameToIdMap.find(predicateTemplate.params[0]);
            if (scheduleId != scheduleNameToIdMap.end() && entityItr != action->mappedParameters.end())
            {
                Entity entity = entityItr->second;
                Instance instance = lookup(entity);

                auto scheduleItr = schedules.find(scheduleId->second);

                if (scheduleItr != schedules.end())
                {
                    ScheduleInstance* oldSchedule = _data.currentSchedule[instance.i];
                    delete oldSchedule;
                    _data.queuedActions[instance.i].clear();

                    Schedule* schedule = scheduleItr->second;
                    ScheduleInstance* scheduleInstance = new ScheduleInstance(schedule);
                    scheduleInstance->chooseEntryForTime(0.0);
                    _data.currentSchedule[instance.i] = scheduleInstance;
                    _data.queuedActions[instance.i].push_back(_data.currentSchedule[instance.i]->chooseNewAction());
                    storyLogger.logState(entity, _data.currentSchedule[instance.i]->getName(), _data.queuedActions[instance.i]);
                    storyLogger.logEvent({"schedule changed to " + scheduleInstance->getName()}, {entity});
                }
            }
        }
        else
        {
            std::cout << "Unhandled predicate: " << predicateTemplate.type << std::endl;
        }
    }
}

void ScheduleComponentManager::spawnComponent(Entity entity, std::string scheduleName, double currentTime, StoryLogger& storyLogger)
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
            storyLogger.logState(entity, scheduleInstance->getName(), _data.queuedActions.back());
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

