//
// Created by Dan on 4/22/2016.
//

#ifndef PARTIALORDERPLANNER_EVENTLOGGER_H
#define PARTIALORDERPLANNER_EVENTLOGGER_H

#include <string>
#include <memory>
#include <EntityManager.h>
#include <iomanip>
#include <sstream>

inline std::string time_format (double time)
{
    std::ostringstream oss;

    oss << std::fixed << std::setw(5) << std::setfill ('0') << std::setprecision (2) << time;

    return oss.str();
}

struct EventLog
{
    std::string time;
    std::string description;
};

struct StateLog
{
    std::string description;
};

struct StoryLogger
{
private:
    std::shared_ptr<EntityManager> _entityManager;
public:
    std::vector<EventLog> events;
    std::unordered_map<unsigned, StateLog> states;

    StoryLogger(std::shared_ptr<EntityManager> entityManager) : _entityManager(entityManager)
    { }

    void logEvent(double time, std::vector<std::string> description, std::vector<Entity> entities)
    {
        unsigned numEntities = entities.size();
        if (numEntities == 0)
        {
            return;
        }

        std::string event;
        event += _entityManager->getName(entities[0]);

        for (unsigned i = 0; i < description.size(); ++i)
        {
            event += ' ' + description[i];

            if (i + 1 < numEntities)
            {
                event += ' ' + _entityManager->getName(entities[i + 1]);
            }
        }

        EventLog log;
        log.time = time_format(time);
        log.description = event;

        events.insert(events.begin(), log);
    }
};

#endif //PARTIALORDERPLANNER_EVENTLOGGER_H
