//
// Created by Dan on 2/22/2016.
//

#ifndef PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGERS_H
#define PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGERS_H

#include "ComponentManager.h"
#include "ActionManager.h"
#include <string>

/*class PositionComponentManager : public ComponentManager
{

};*/

class PlaceComponentManager : public ComponentManager
{
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<std::string> location;
        std::vector<Entity> northLocation;
        std::vector<Entity> southLocation;
        std::vector<Entity> eastLocation;
        std::vector<Entity> westLocation;

    };

    InstanceData _data;

public:

    PlaceComponentManager() : ComponentManager()
    {
        _data.size = 0;
    }

    std::vector<Operator> determineActionsOfEntity(Action travelTemplate, Entity traveller)
    {
        std::vector<Operator> ops;

        for (int i = 0; i < _data.size; ++i)
        {
            /*ops.push_back(ActionManager::buildOperator(travelTemplate, {_data.entity[i], _data.northLocation, traveller})); //Params: Entity fromLocation, Entity toLocation, Entity traveller
            ops.push_back(ActionManager::buildOperator(travelTemplate, {_data.entity[i], _data.southLocation, traveller}));
            ops.push_back(ActionManager::buildOperator(travelTemplate, {_data.entity[i], _data.westLocation, traveller}));
            ops.push_back(ActionManager::buildOperator(travelTemplate, {_data.entity[i], _data.eastLocation, traveller}));*/
        }

        return ops;
    }

    void spawnComponent(Entity entity, std::string location, Entity north, Entity south, Entity east, Entity west)
    {

        _map.emplace(entity.index(), _data.size);

        _data.entity.push_back(entity);
        _data.location.push_back(location);
        _data.northLocation.push_back(north);
        _data.southLocation.push_back(south);
        _data.eastLocation.push_back(east);
        _data.westLocation.push_back(west);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.location[i] = _data.location[last];
        _data.northLocation[i] = _data.northLocation[last];
        _data.southLocation[i] = _data.southLocation[last];
        _data.westLocation[i] = _data.westLocation[last];
        _data.eastLocation[i] = _data.eastLocation[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.location.pop_back();
        _data.northLocation.pop_back();
        _data.southLocation.pop_back();
        _data.westLocation.pop_back();
        _data.eastLocation.pop_back();

        --_data.size;
    }
};


#endif //PARTIALORDERPLANNER_CHARACTERCOMPONENTMANAGERS_H
