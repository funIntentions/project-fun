//
// Created by Dan on 4/20/2016.
//

#ifndef PARTIALORDERPLANNER_TYPECOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_TYPECOMPONENTMANAGER_H

#include <ComponentManager.h>
#include <algorithm>
#include <fstream>
#include <rapidjson/document.h>
#include <set>
#include "AttributeComponentManager.h"

typedef std::string Category;
typedef std::string Type;

struct Association
{
    Category category;
    std::string preference;
    std::set<Type> types;
};

struct Group
{
    std::string name;
    std::unordered_map<Category, Association> associations;
};

class TypeComponentManager : ComponentManager
{

private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<std::vector<std::string>> groups;
    };

    std::unordered_map<std::string, Group> groups;
    InstanceData _data;
    std::shared_ptr<AttributeComponentManager> _attributeComponentManager;

public:

    TypeComponentManager(std::shared_ptr<AttributeComponentManager> attributeComponentManager) : ComponentManager(), _attributeComponentManager(attributeComponentManager)
    {
        _data.size = 0;
    }

    void readGroups(std::string worldDataPath)
    {
        std::ifstream in(worldDataPath);
        std::string contents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
        const char* json = contents.c_str();

        rapidjson::Document document;
        document.Parse(json);

        const rapidjson::Value& a = document["groups"];
        assert(a.IsArray());

        for (auto group = a.Begin(); group != a.End(); ++group)
        {
            Group newGroup;

            assert(group->IsObject());

            auto member_itr = group->FindMember("group");
            if (member_itr != group->MemberEnd())
                newGroup.name = member_itr->value.GetString();

            member_itr = group->FindMember("associations");
            if (member_itr != document.MemberEnd())
            {
                assert(member_itr->value.IsArray());
                for (auto association = member_itr->value.Begin(); association != member_itr->value.End(); ++association)
                {
                    Association newAssociation;

                    assert(association->IsObject());
                    auto category = association->FindMember("category");
                    newAssociation.category = category->value.GetString();

                    auto preference = association->FindMember("preference");
                    if (preference != association->MemberEnd())
                        newAssociation.preference = preference->value.GetString();

                    auto types = association->FindMember("type");
                    assert(types->value.IsArray());
                    for (auto type = types->value.Begin(); type != types->value.End(); ++type)
                    {
                        assert(type->IsString());
                        newAssociation.types.insert(type->GetString());

                    }

                    newGroup.associations[newAssociation.category] = newAssociation;
                }
            }

            groups[newGroup.name] = newGroup;
        }

        delete json;
    }

    void addGroup(Group group)
    {
        groups.insert({group.name, group});
    }

    std::vector<Category> getCategories(Entity entity, Entity otherEntity)
    {
        if (entity.id == otherEntity.id)
            return {"Self"};

        std::vector<Category> categories;

        Instance instance = lookup(entity);
        std::vector<std::string> types = _data.groups[instance.i];
        Instance otherEntityInstance = lookup(otherEntity);
        std::vector<std::string> otherEntityTypes = _data.groups[otherEntityInstance.i];

        for (std::string type : types)
        {
            Group group = groups.find(type)->second; // TODO: check if type exists
            for (auto association : group.associations)
            {
                for (std::string otherEntityType : otherEntityTypes)
                {
                    if (association.second.types.find(otherEntityType) != association.second.types.end())
                        categories.push_back(association.first);
                }
            }
        }

        return categories;
    }

    void spawnComponent(Entity entity, std::vector<std::string> groups)
    {
        _map.emplace(entity.index(), _data.size);

        _data.entity.push_back(entity);
        _data.groups.push_back(groups);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.groups[i] = _data.groups[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.groups.pop_back();

        --_data.size;
    }
};

#endif //PARTIALORDERPLANNER_TYPECOMPONENTMANAGER_H
