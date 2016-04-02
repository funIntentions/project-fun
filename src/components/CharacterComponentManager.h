//
// Created by Dan on 3/17/2016.
//

#ifndef PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGER_H

#include <ComponentManager.h>
#include <algorithm>
#include <fstream>
#include <rapidjson/document.h>
#include "AttributeComponentManager.h"

typedef std::string Category;
typedef std::string Type;

struct Opinion
{
    float variance;
    Entity entity;
};

struct Association
{
    Category category;
    std::string preference;
    std::vector<Type> types;
};

struct Group
{
    std::string name;
    std::unordered_map<Category, Association> associations;
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
    std::shared_ptr<AttributeComponentManager> _attributeComponentManager;

public:

    CharacterComponentManager(std::shared_ptr<AttributeComponentManager> attributeComponentManager) : ComponentManager(), _attributeComponentManager(attributeComponentManager)
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
                        newAssociation.types.push_back(type->GetString());

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

    void spawnComponent(Entity entity, std::vector<std::string> groups)
    {
        _map.emplace(entity.index(), _data.size);

        Character character;
        character.groups = groups;

        _data.entity.push_back(entity);
        _data.character.push_back(character);

        ++_data.size;
    }

    std::vector<Opinion> getOpinions(Entity entity, Category category)
    {
        if (category == "Self")
            return {{1.0, entity}};

        Instance instance = lookup(entity);
        auto opinions = _data.character[instance.i].associations.find(category);

        if (opinions == _data.character[instance.i].associations.end())
        {
            std::cout << "I have no knowledge of the concept: " << category << std::endl;
            return {};
        }

        return opinions->second;
    }

    static bool sortOpinionsByVariance(const Opinion& lhs, const Opinion& rhs)
    {
        return lhs.variance > rhs.variance;
    }

    void setOpinionVariance(Entity entity, Category category, Entity opinionEntity, float variance)
    {
        Instance instance = lookup(entity);

        auto opinions = _data.character[instance.i].associations.find(category);

        if (opinions == _data.character[instance.i].associations.end())
        {
            std::cout << "I have no knowledge of the concept: " << category << std::endl;
        }
        else
        {
            for (Opinion& opinion : opinions->second)
            {
                if (opinion.entity.id == opinionEntity.id)
                    opinion.variance = variance;
            }

            std::sort(opinions->second.begin(), opinions->second.end(), sortOpinionsByVariance);
        }
    }

    void setAllOpinionVariances(Category category, Entity opinionEntity, float variance)
    {
        for (unsigned i = 0; i < _data.size; ++i)
        {
            setOpinionVariance(_data.entity[i], category, opinionEntity, variance);
        }
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

    std::vector<Category> getCategories(Entity entity, Entity otherEntity) {
        std::vector<Category> categories;

        Instance instance = lookup(entity);
        Character character = _data.character[instance.i];

        for (auto association : character.associations)
        {
            std::vector<Opinion> opinions = association.second;
            for (Opinion opinion : opinions)
            {
                if (opinion.entity.id == otherEntity.id)
                    categories.push_back(association.first);
            }
        }

        return categories;
    }

    void assignToCategory(Entity entity, Entity otherEntity)
    {
        Instance instance = lookup(entity);
        Character* character  = &_data.character[instance.i];
        instance = lookup(otherEntity);
        Character otherCharacter = _data.character[instance.i];

        // TODO: Seriously reduce this mass confusion!
        for (std::string characterGroup : character->groups) // Go through all groups that this entity belongs to.
        {
            Group group = groups.find(characterGroup)->second;

            for (auto association : group.associations) // For each group, go through the category -> types relationships
            {

                for (auto type : association.second.types) // Go through the types of entities that are associated with this category
                {
                    for (std::string otherCharacterType : otherCharacter.groups) // Determine if this other entity is of a type that has a association with a group this entity belongs to.
                    {
                        if (otherCharacterType == type)
                        {
                            float variance = 1.0f;
                            if (association.second.preference == "Alive" && _attributeComponentManager->isDead(otherEntity))
                                variance = 0.0f;


                            bool known = false;
                            for (Opinion opinion : character->associations[association.first])
                                if (opinion.entity.id == otherEntity.id)
                                    known = true;// Don't add to knowledge if the entity is already known

                            if (!known)
                            {
                                character->associations[association.first].push_back({variance, otherEntity});
                                std::sort(character->associations[association.first].begin(), character->associations[association.first].end(), sortOpinionsByVariance);
                            }
                        }
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
