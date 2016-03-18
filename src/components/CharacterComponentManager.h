//
// Created by Dan on 3/17/2016.
//

#ifndef PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGER_H

#include <ComponentManager.h>
#include <fstream>
#include <rapidjson/document.h>

typedef std::string Category;
typedef std::string Type;

struct Opinion
{
    float variance;
    Entity entity;
};

struct Group
{
    std::string name;
    std::unordered_map<Category, std::vector<Type>> associations;
};

struct Character
{
    std::vector<std::string> groups;
    std::unordered_map<Category, std::vector<Opinion>> associations;
    std::vector<Entity> knowledge;
};

class CharacterComponentManager : ComponentManager
{

private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<Character> character;
    };

    std::unordered_map<std::string, Group> groups;
    InstanceData _data;

public:

    CharacterComponentManager() : ComponentManager()
    {
        _data.size = 0;

        readGroups();
    }

    void readGroups()
    {
        std::ifstream in("data/World.json");
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
                    assert(association->IsObject());
                    auto category = association->FindMember("category");
                    std::string categoryName = category->value.GetString();

                    auto types = association->FindMember("type");
                    assert(types->value.IsArray());
                    for (auto type = types->value.Begin(); type != types->value.End(); ++type)
                    {
                        assert(type->IsString());
                        newGroup.associations[categoryName].push_back(type->GetString());

                    }
                }
            }

            groups[newGroup.name] = newGroup;
        }

        delete json;
    }

    void spawnComponent(Entity entity, std::vector<std::string> groups)
    {
        _map.emplace(entity.index(), _data.size);

        Character character;
        character.groups = groups;

        _data.entity.push_back(entity);
        _data.character.push_back(character);

        ++_data.size;
    }

    void addKnowledge(Entity entity, std::vector<Entity> knowledge)
    {
        for (Entity known : knowledge)
        {
            Instance instance = lookup(known);
            Entity otherEntity = _data.entity[instance.i];

            assignToCategory(entity, otherEntity);
        }
    }

    void assignToCategory(Entity entity, Entity otherEntity)
    {
        Instance instance = lookup(entity);
        Character* character  = &_data.character[instance.i];
        instance = lookup(otherEntity);
        Character otherCharacter = _data.character[instance.i];
        // TODO: Seriously reduce this mass confusion!
        for (std::string characterGroup : character->groups)
        {
            Group group = groups.find(characterGroup)->second;

            for (auto association : group.associations)
            {
                for (auto type : association.second)
                {
                    for (std::string otherCharacterType : otherCharacter.groups)
                    {
                        if (otherCharacterType == type)
                            character->associations[association.first].push_back({1.0f, otherEntity});
                    }
                }
            }
        }
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.character[i] = _data.character[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.character.pop_back();

        --_data.size;
    }
};

#endif //PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGER_H