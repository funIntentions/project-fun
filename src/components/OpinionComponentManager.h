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
#include "TypeComponentManager.h"

typedef std::string Category;
typedef std::string Type;

struct Opinion
{
    float variance;
    Entity entity;
};

class OpinionComponentManager : ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<std::unordered_map<Category, std::vector<Opinion>>> opinions;
    };

    InstanceData _data;
    std::shared_ptr<AttributeComponentManager> _attributeComponentManager;
    std::shared_ptr<TypeComponentManager> _typeComponentManager;

public:
    OpinionComponentManager(std::shared_ptr<AttributeComponentManager> attributeComponentManager, std::shared_ptr<TypeComponentManager> typeComponentManager) :
            ComponentManager(), _attributeComponentManager(attributeComponentManager), _typeComponentManager(typeComponentManager)
    {
        _data.size = 0;
    }

    void spawnComponent(Entity entity, std::vector<Entity> knowledge)
    {
        _map.emplace(entity.index(), _data.size);

        _data.entity.push_back(entity);
        _data.opinions.push_back(std::unordered_map<Category, std::vector<Opinion>>());

        addKnowledge(entity, knowledge);

        ++_data.size;
    }

    std::vector<Opinion> getOpinions(Entity entity, Category category)
    {
        if (category == "Self")
            return {{1.0, entity}};

        Instance instance = lookup(entity);
        auto opinions = _data.opinions[instance.i].find(category);

        if (opinions == _data.opinions[instance.i].end())
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

        auto opinions = _data.opinions[instance.i].find(category);

        if (opinions == _data.opinions[instance.i].end())
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
            assignToCategory(entity, known);
        }
    }

    void assignToCategory(Entity entity, Entity otherEntity)
    {
        Instance instance = lookup(entity);
        auto& opinionMap = _data.opinions[instance.i];

        std::vector<Category> categories = _typeComponentManager->getCategories(entity, otherEntity);
        for (Category category : categories)
        {
            std::vector<Opinion> categoryOpinions = opinionMap[category];

            int i = 0;
            unsigned opinionCount = categoryOpinions.size();
            for (; i < opinionCount; ++i)
            {
                Opinion existingOpinion = categoryOpinions[i];
                if (existingOpinion.entity.id == otherEntity.id)
                    break;
            }
            if (i == opinionCount) // doesn't exist yet
            {
                float variance = 0.0f;
                opinionMap[category].push_back({variance, otherEntity});
                std::sort(opinionMap[category].begin(), opinionMap[category].end(), sortOpinionsByVariance);
            }
        }
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.opinions[i] = _data.opinions[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.opinions.pop_back();

        --_data.size;
    }
};

#endif //PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGER_H
