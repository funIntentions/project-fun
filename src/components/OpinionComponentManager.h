//
// Created by Dan on 3/17/2016.
//

#ifndef PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGER_H

#include <ComponentManager.h>
#include <algorithm>
#include <fstream>
#include <algorithm>
#include <rapidjson/document.h>
#include <util/Extra.h>
#include "AttributeComponentManager.h"
#include "TypeComponentManager.h"
#include "demo/StoryLogger.h"

typedef std::string Category;
typedef std::string Type;

struct Opinion
{
    float variance;
    Entity entity;
};

class OpinionComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<std::unordered_map<Category, std::vector<Opinion>>> opinions;
        std::vector<std::set<Entity>> knowledge;
    };

    InstanceData _data;
    std::shared_ptr<AttributeComponentManager> _attributeComponentManager;
    std::shared_ptr<TypeComponentManager> _typeComponentManager;
    std::shared_ptr<StoryLogger> _storyLogger;

    float _highestVariance = 1.0f;
    float _lowestVariance = -1.0f;
public:
    OpinionComponentManager(std::shared_ptr<AttributeComponentManager> attributeComponentManager, std::shared_ptr<TypeComponentManager> typeComponentManager, std::shared_ptr<StoryLogger> storyLogger) :
            ComponentManager(), _attributeComponentManager(attributeComponentManager), _typeComponentManager(typeComponentManager), _storyLogger(storyLogger)
    {
        _data.size = 0;
    }

    void spawnComponent(Entity entity, std::vector<Entity> knowledge)
    {
        _map.emplace(entity.index(), _data.size);

        _data.entity.push_back(entity);
        _data.opinions.push_back(std::unordered_map<Category, std::vector<Opinion>>());
        _data.knowledge.push_back(std::set<Entity>());

        addKnowledge(entity, knowledge);

        ++_data.size;
    }

    std::set<Entity> getKnowledge(Entity entity)
    {
        Instance instance = lookup(entity);
        assert (instance.i != -1);

        return _data.knowledge[instance.i];
    }

    std::unordered_map<Category, std::vector<Opinion>> getOpinions(Entity entity)
    {
        Instance instance = lookup(entity);
        assert(instance.i != -1);
        return _data.opinions[instance.i];
    }

    void removeOpinion(Entity entity, Category category)
    {
        Instance instance = lookup(entity);
        auto opinions = _data.opinions[instance.i].find(category);

        if (opinions != _data.opinions[instance.i].end() && opinions->second.size() > 0)
        {
            opinions->second.erase(opinions->second.begin());
        }
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

    void setOpinionVariance(Entity entity, Category category, Entity opinionEntity, float value)
    {
        Instance instance = lookup(entity);

        auto opinions = _data.opinions[instance.i].find(category);

        if (opinions == _data.opinions[instance.i].end())
        {
            std::cout << "setOpinionVariance: I have no knowledge of the concept: " << category << std::endl;
        }
        else
        {
            for (Opinion& opinion : opinions->second)
            {
                if (opinion.entity.id == opinionEntity.id)
                {
                    opinion.variance = std::max(_lowestVariance, std::min(value, _highestVariance));
                }
            }

            std::sort(opinions->second.begin(), opinions->second.end(), sortOpinionsByVariance);
        }
    }

    void adjustOpinionVariance(Entity entity, Category category, Entity opinionEntity, float value)
    {
        Instance instance = lookup(entity);

        auto opinions = _data.opinions[instance.i].find(category);

        if (opinions == _data.opinions[instance.i].end())
        {
            std::cout << "setOpinionVariance: I have no knowledge of the concept: " << category << std::endl;
        }
        else
        {
            for (Opinion& opinion : opinions->second)
            {
                if (opinion.entity.id == opinionEntity.id)
                {
                    opinion.variance += value;
                    opinion.variance = std::max(_lowestVariance, std::min(opinion.variance, _highestVariance));
                }
            }

            std::sort(opinions->second.begin(), opinions->second.end(), sortOpinionsByVariance);
        }
    }

    void adjustOtherOpinionVariances(Entity entity, Category category, Entity opinionEntity, float value)
    {
        Instance instance = lookup(entity);

        auto opinions = _data.opinions[instance.i].find(category);

        if (opinions == _data.opinions[instance.i].end())
        {
            std::cout << "increaseOtherOpinionVariances: I have no knowledge of the concept: " << category << std::endl;
        }
        else
        {
            for (Opinion& opinion : opinions->second)
            {
                if (opinion.entity.id != opinionEntity.id)
                {
                    opinion.variance += value;
                    opinion.variance = std::max(_lowestVariance, std::min(opinion.variance, _highestVariance));
                }
            }

            std::sort(opinions->second.begin(), opinions->second.end(), sortOpinionsByVariance);
        }
    }

    void addKnowledge(Entity entity, std::vector<Entity> knowledge)
    {
        Instance instance = lookup(entity);
        if (instance.i == -1) return;

        for (Entity known : knowledge)
        {
            if (_data.knowledge[instance.i].find(known) != _data.knowledge[instance.i].end())
            {
                std::cout << "Entity already known: " + to_string(known.id) << std::endl;
                continue;
            }

            _data.knowledge[instance.i].insert(known);
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
            _storyLogger->logEvent({"added as new " + category + " opinion for"}, {otherEntity, entity});
            std::cout << "Adding opinion: " + to_string(otherEntity.id) + "in category: " + category << std::endl;
            float variance = 0.0f;
            opinionMap[category].push_back({variance, otherEntity});
            std::sort(opinionMap[category].begin(), opinionMap[category].end(), sortOpinionsByVariance);
        }
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.opinions[i] = _data.opinions[last];
        _data.knowledge[i] = _data.knowledge[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.opinions.pop_back();
        _data.knowledge.pop_back();

        --_data.size;
    }
};

#endif //PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGER_H
