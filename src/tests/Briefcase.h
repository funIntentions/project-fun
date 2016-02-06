//
// Created by dan on 29/06/15.
//

#ifndef PARTIALORDERPLANNER_BRIEFCASE_H
#define PARTIALORDERPLANNER_BRIEFCASE_H

#include "../PartialOrderPlan.h"
#include <tuple>
#include <fstream>
#include <algorithm>
#include "rapidjson/reader.h"
#include "rapidjson/error/en.h"
#include "rapidjson/document.h"

enum BriefcaseStates
{
    dictionaryAtOffice = 0, // 0
    paycheckAtOffice = 1, // 1
    dictionaryAtHome = 2, // 2
    paycheckAtHome = 3, // 3
    dictionaryInBriefcase = 4, // 4
    paycheckInBriefcase = 5, // 5
    briefcaseAtHome = 6, // 6
    briefcaseAtOffice = 7 // 7
};

/*struct Predicate
{
    int type;
    std::tuple<int> params;
};

std::unordered_map<Predicate, size_t> predicates;

std::vector<int> entities =
        {0, // dictionary
         1, // paycheck
         2, // briefcase
         3, // home
         4}; // office
*/
/*
 * (:action Put
 * :parameters (location ?l, briefcase ?c, object ?o)
 * :preconditions (not(in(?o, ?c)) and at(?l, ?c) and at(?l, ?o))
 * :effects (not(at(?l, ?o)) and in(?o, ?c)) )
 *
 * (:action Remove
 * :parameters (location ?l, briefcase ?c, object ?o)
 * :preconditions (in(?o, ?c) and at(?l, ?c))
 * :effects (at(?l, ?o) and not(in(?o, ?c))) )
 *
 * (:action Move
 * :parameters (location ?l, briefcase ?c, location ?l2)
 * :preconditions ((at(?l, ?c)) and not(at(?l2, ?c)))
 * :effects (at(?l2, ?c) and not(at(?l, ?c))) )
 */

struct BriefcaseDomain
{
public:

    std::vector<Operator> operators;
    Operator start, finish;

    BriefcaseDomain()
    {
        start.name = "start";
        finish.name = "finish";

        start.addedEffects = {briefcaseAtHome, dictionaryAtHome, paycheckInBriefcase};
        finish.preconditions = {dictionaryAtOffice, paycheckAtHome};

        Operator homeToOffice("HomeToOffice");
        homeToOffice.preconditions = {briefcaseAtHome};
        homeToOffice.addedEffects = {briefcaseAtOffice};
        homeToOffice.subtractedEffects = {briefcaseAtHome};

        Operator officeToHome("OfficeToHome");
        officeToHome.preconditions = {briefcaseAtOffice};
        officeToHome.addedEffects = {briefcaseAtHome};
        officeToHome.subtractedEffects = {briefcaseAtOffice};

        Operator takeOutPaycheckAtHome("takeOutPaycheckAtHome");
        takeOutPaycheckAtHome.preconditions = {paycheckInBriefcase, briefcaseAtHome};
        takeOutPaycheckAtHome.addedEffects = {paycheckAtHome};
        takeOutPaycheckAtHome.subtractedEffects = {paycheckInBriefcase};

        Operator takeOutPaycheckAtOffice("takeOutPaycheckAtOffice");
        takeOutPaycheckAtOffice.preconditions = {paycheckInBriefcase, briefcaseAtOffice};
        takeOutPaycheckAtOffice.addedEffects = {paycheckAtOffice};
        takeOutPaycheckAtOffice.subtractedEffects = {paycheckInBriefcase};

        Operator putPaycheckInAtHome("putPaycheckInAtHome");
        putPaycheckInAtHome.preconditions = {paycheckAtHome, briefcaseAtHome};
        putPaycheckInAtHome.addedEffects = {paycheckInBriefcase};
        putPaycheckInAtHome.subtractedEffects = {paycheckAtHome};

        Operator putPaycheckInAtOffice("putPaycheckInAtOffice");
        putPaycheckInAtOffice.preconditions = {paycheckAtOffice, briefcaseAtOffice};
        putPaycheckInAtOffice.addedEffects = {paycheckInBriefcase};
        putPaycheckInAtOffice.subtractedEffects = {paycheckAtOffice};

        Operator takeOutDictionaryAtHome("takeOutDictionaryAtHome");
        takeOutDictionaryAtHome.preconditions = {dictionaryInBriefcase, briefcaseAtHome};
        takeOutDictionaryAtHome.addedEffects = {dictionaryAtHome};
        takeOutDictionaryAtHome.subtractedEffects = {dictionaryInBriefcase};

        Operator takeOutDictionaryAtOffice("takeOutDictionaryAtOffice");
        takeOutDictionaryAtOffice.preconditions = {dictionaryInBriefcase, briefcaseAtOffice};
        takeOutDictionaryAtOffice.addedEffects = {dictionaryAtOffice};
        takeOutDictionaryAtOffice.subtractedEffects = {dictionaryInBriefcase};

        Operator putDictionaryInAtHome("putDictionaryInAtHome");
        putDictionaryInAtHome.preconditions = {dictionaryAtHome, briefcaseAtHome};
        putDictionaryInAtHome.addedEffects = {dictionaryInBriefcase};
        putDictionaryInAtHome.subtractedEffects = {dictionaryAtHome};

        Operator putDictionaryInAtOffice("putDictionaryInAtOffice");
        putDictionaryInAtOffice.preconditions = {dictionaryAtOffice, briefcaseAtOffice};
        putDictionaryInAtOffice.addedEffects = {dictionaryInBriefcase};
        putDictionaryInAtOffice.subtractedEffects = {dictionaryAtOffice};

        operators = {officeToHome,
                      homeToOffice,
                      takeOutPaycheckAtHome,
                      takeOutPaycheckAtOffice,
                      putPaycheckInAtHome,
                      putPaycheckInAtOffice,
                      takeOutDictionaryAtHome,
                      takeOutDictionaryAtOffice,
                      putDictionaryInAtHome,
                      putDictionaryInAtOffice};

    }


};

static const char* kTypeNames[] =
        { "Null", "False", "True", "Object", "Array", "String", "Number" };

struct Predicate
{
    std::string type;
    std::vector<std::string> params;

    bool operator==(const Predicate& other) const {
        return (type == other.type
                && params == other.params);
    }
};

namespace std {
    template <>
    struct hash<Predicate>
    {
        std::size_t operator()(const Predicate& predicate) const
        {
            std::size_t seed = 0;
            for(auto& param : predicate.params) {
                seed ^= std::hash<std::string>()(param) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            return (std::hash<std::string>()(predicate.type)) ^ seed;
        }
    };
}

std::unordered_map<Predicate, size_t> predicates;

size_t addPredicate(Predicate predicate)
{
    size_t id = predicates.size();
    auto itr = predicates.find(predicate);
    if (itr != predicates.end())
        id = itr->second;
    else
        predicates.insert({predicate, id});
    return id;
}

struct Task
{
    std::string name;
    std::vector<std::string> parameters;
    std::vector<Predicate> positivePreconditions;
    std::vector<Predicate> addedEffects;
    std::vector<Predicate> subtractedEffects;

    Task(std::string op = "") : name(op)
    {
    }
};

std::unordered_map<std::string, Task> tasks;
std::vector<std::string> entities;
std::vector<Operator> operators;
std::vector<Operator> briefcaseOperators;
Operator briefcaseStart;
Operator briefcaseEnd;

Operator generateOperator(const Task& templateTask, const std::vector<std::string>& entities, std::string temp) // TODO: Remove temp and the string compare
{
    Operator anOperator;
    anOperator.name = templateTask.name + temp;

    assert(templateTask.parameters.size() == entities.size());

    std::unordered_map<std::string, std::string> paramMapping;
    for (int i = 0; i < templateTask.parameters.size(); ++i)
    {
        paramMapping.insert({templateTask.parameters[i], entities[i]});
    }

    for (Predicate precondition : templateTask.positivePreconditions)
    {
        Predicate newPrecondition;
        newPrecondition.type = precondition.type;

        for (std::string param : precondition.params)
        {
            auto itr = paramMapping.find(param);
            if (itr != paramMapping.end())
            {
                newPrecondition.params.push_back(itr->second);
            }
        }

        anOperator.preconditions.push_back(addPredicate(newPrecondition));
    }

    for (Predicate effect : templateTask.addedEffects)
    {
        Predicate newEffect;
        newEffect.type = effect.type;

        for (std::string param : effect.params)
        {
            auto itr = paramMapping.find(param);
            if (itr != paramMapping.end())
            {
                newEffect.params.push_back(itr->second);
            }
        }

        anOperator.addedEffects.push_back(addPredicate(newEffect));
    }

    for (Predicate effect : templateTask.subtractedEffects)
    {
        Predicate newEffect;
        newEffect.type = effect.type;

        for (std::string param : effect.params)
        {
            auto itr = paramMapping.find(param);
            if (itr != paramMapping.end())
            {
                newEffect.params.push_back(itr->second);
            }
        }

        anOperator.subtractedEffects.push_back(addPredicate(newEffect));
    }

    return anOperator;
}

void parseJsonData()
{
    std::ifstream in("data/Briefcase.json");
    std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    const char* json = contents.c_str();

    rapidjson::Document document;
    document.Parse(json);

    const rapidjson::Value& e = document["entities"];
    assert(e.IsArray());

    for (auto entity_itr = e.Begin(); entity_itr != e.End(); ++entity_itr)
    {
        assert(entity_itr->IsString());
        entities.push_back(entity_itr->GetString());
    }

    const rapidjson::Value& a = document["actions"];
    assert(a.IsArray());

    for (auto action_itr = a.Begin(); action_itr != a.End(); ++action_itr)
    {
        assert(action_itr->IsObject());
        Task task;

        auto member_itr = action_itr->FindMember("name");
        if (member_itr != action_itr->MemberEnd())
            task.name = member_itr->value.GetString();

        member_itr = action_itr->FindMember("params");
        if (member_itr != document.MemberEnd())
        {
            assert(member_itr->value.IsArray());
            for (auto param_itr = member_itr->value.Begin(); param_itr != member_itr->value.End(); ++param_itr)
            {
                assert(param_itr->IsObject());
                auto param_member = param_itr->FindMember("name");
                task.parameters.push_back(param_member->value.GetString());
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

                task.positivePreconditions.push_back(predicate);
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

                task.addedEffects.push_back(predicate);
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

                task.subtractedEffects.push_back(predicate);
            }
        }

        // Generate Params
        /*std::vector<std::string> partialParams[entities.size()];

        for (int param = 0; param < task.parameters.size(); ++param)
        {
            for (int entity = 0; entity < entities.size(); ++entity)
            {
                partialParams[entity].push_back(entities[entity]);
            }
        }*/

        tasks.insert({task.name, task});
    }

    briefcaseOperators.push_back(generateOperator(tasks["put"], {"Home", "Briefcase", "Paycheck"}, "0"));
    briefcaseOperators.push_back(generateOperator(tasks["put"], {"Home", "Briefcase", "Dictionary"}, "1"));
    briefcaseOperators.push_back(generateOperator(tasks["put"], {"Office", "Briefcase", "Paycheck"}, "2"));
    briefcaseOperators.push_back(generateOperator(tasks["put"], {"Office", "Briefcase", "Dictionary"}, "3"));
    briefcaseOperators.push_back(generateOperator(tasks["remove"], {"Office", "Briefcase", "Dictionary"}, "4"));
    briefcaseOperators.push_back(generateOperator(tasks["remove"], {"Office", "Briefcase", "Paycheck"}, "5"));
    briefcaseOperators.push_back(generateOperator(tasks["remove"], {"Home", "Briefcase", "Dictionary"}, "6"));
    briefcaseOperators.push_back(generateOperator(tasks["remove"], {"Home", "Briefcase", "Paycheck"}, "7"));
    briefcaseOperators.push_back(generateOperator(tasks["move"], {"Home", "Briefcase", "Office"}, "8"));
    briefcaseOperators.push_back(generateOperator(tasks["move"], {"Office", "Briefcase", "Home"}, "9"));

    Predicate startPredicate;
    startPredicate.type = "at";
    startPredicate.params = {"Home", "Dictionary"};
    Predicate startPredicate2;
    startPredicate2.type = "at";
    startPredicate2.params = {"Home", "Briefcase"};
    Predicate startPredicate3;
    startPredicate3.type = "in";
    startPredicate3.params = {"Paycheck", "Briefcase"};

    Predicate endPredicate;
    endPredicate.type = "at";
    endPredicate.params = {"Office", "Dictionary"};
    Predicate endPredicate2;
    endPredicate2.type = "at";
    endPredicate2.params = {"Home", "Paycheck"};

    size_t sp = addPredicate(startPredicate);
    size_t sp2 = addPredicate(startPredicate2);
    size_t sp3 = addPredicate(startPredicate3);

    size_t ep = addPredicate(endPredicate);
    size_t ep2 = addPredicate(endPredicate2);

    briefcaseStart.name = "start";
    briefcaseStart.addedEffects = {sp, sp2, sp3};

    briefcaseEnd.name = "end";
    briefcaseEnd.preconditions = {ep, ep2};
}

#endif //PARTIALORDERPLANNER_BRIEFCASE_H