//
// Created by Dan on 4/2/2016.
//

#ifndef PARTIALORDERPLANNER_PLACECOMPONENTMANAGER_H
#define PARTIALORDERPLANNER_PLACECOMPONENTMANAGER_H


#include <EntityManager.h>
#include <ComponentManager.h>

enum Place
{
    Marshland,
    Ashplanes,
    Darkvoid
};

class PlaceComponentManager : public ComponentManager {
private:
    struct InstanceData
    {
        unsigned size;
        std::vector<Entity> entity;
        std::vector<Place> place;
    };

    InstanceData _data;
    const int numOfPlaces = 3;
    std::string* places;

public:

    PlaceComponentManager() : ComponentManager()
    {
        places = new std::string[numOfPlaces];

        _data.size = 0;
        places[Marshland] = "marshland";
        places[Ashplanes] = "ashplane";
        places[Darkvoid] = "darkvoid";
    }

    ~PlaceComponentManager() {delete places;}

    int findPlace(std::string placeName)
    {
        for (int place = 0; place < numOfPlaces; ++place)
        {
            if (places[place] == placeName)
            {
                return  place;
            }
        }
        return -1;
    }

    Place getPlaceForEntity(Entity entity)
    {
        return _data.place[entity.id];
    }

    void changeEntitiesPlace(Entity entity, Place place)
    {
        _data.place[entity.id] = place;
    }

    std::vector<Entity> getEntitiesAtPlace(Place place)
    {
        std::vector<Entity> entities;

        for (int index = 0; index < _data.size; ++index)
        {
            if (_data.place[index] == place)
            {
                entities.push_back(_data.entity[index]);
            }
        }

        return entities;
    }

    void spawnComponent(Entity entity, Place place)
    {
        _map.emplace(entity.index(), _data.size);
        _data.entity.push_back(entity);
        _data.place.push_back(place);

        ++_data.size;
    }

    void destroy(unsigned i)
    {
        unsigned last = _data.size - 1;
        Entity e = _data.entity[i];
        Entity last_e = _data.entity[last];

        _data.entity[i] = _data.entity[last];
        _data.place[i] = _data.place[last];

        _map[last_e.index()] =  i;
        _map.erase(e.index());

        _data.entity.pop_back();
        _data.place.pop_back();

        --_data.size;
    }
};


#endif //PARTIALORDERPLANNER_PLACECOMPONENTMANAGER_H
