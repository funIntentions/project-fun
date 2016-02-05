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
std::vector<std::string> entities;

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
    std::vector<Predicate> negativePreconditions;
    std::vector<Predicate> addedEffects;
    std::vector<Predicate> subtractedEffects;

    Task(std::string op = "") : name(op)
    {
    }
};

//Task createTask(rapidjson::Value taskObject)
//{
//    Task task;
//    rapidjson::Value::ConstMemberIterator member_itr = taskObject.MemberBegin();
//    printf("Type of member %s is %s\n", member_itr->name.GetString(), kTypeNames[member_itr->value.GetType()]);
//}

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
                std::cout << addPredicate(predicate);
                task.positivePreconditions.push_back(predicate);
            }
        }

        member_itr = action_itr->FindMember("negativePreconditions");
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

                std::cout << addPredicate(predicate);
                task.negativePreconditions.push_back(predicate);
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

                std::cout << addPredicate(predicate);
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

                std::cout << addPredicate(predicate);
                task.subtractedEffects.push_back(predicate);
            }
        }
    }
}


void createOperator()
{

}

#endif //PARTIALORDERPLANNER_BRIEFCASE_H
