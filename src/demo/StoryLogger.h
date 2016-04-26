//
// Created by Dan on 4/22/2016.
//

#ifndef PARTIALORDERPLANNER_EVENTLOGGER_H
#define PARTIALORDERPLANNER_EVENTLOGGER_H

#include <string>
#include <memory>
#include <EntityManager.h>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <schedules/ActionInstance.h>

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
    unsigned _eventLimit;
    std::ofstream _logFile;
public:
    std::vector<EventLog> events;
    std::unordered_map<unsigned, StateLog> states;


    StoryLogger(std::shared_ptr<EntityManager> entityManager, unsigned limit = 30) : _entityManager(entityManager), _eventLimit(limit), _logFile("StoryLog.txt")
    {
        if (_logFile.is_open())
            _logFile.clear();
    }

    ~StoryLogger()
    {
        if (_logFile.is_open())
            _logFile.close();
    }

    void logEvent(double time, std::vector<std::string> description, std::vector<Entity> entities)
    {

        EventLog log;
        log.time = time_format(time);

        unsigned numEntities = entities.size();
        if (numEntities == 0)
        {
            return;
        }

        log.description += _entityManager->getName(entities[0]);

        for (unsigned i = 0; i < description.size(); ++i)
        {
            log.description += ' ' + description[i];

            if (i + 1 < numEntities)
            {
                log.description += ' ' + _entityManager->getName(entities[i + 1]);
            }
        }

        if (_logFile.is_open())
        {
            _logFile << log.time << ": " << log.description << std::endl;
        }

        events.insert(events.begin(), log);
        if (events.size() > _eventLimit)
            events.pop_back();
    }

    void logState(Entity entity, std::string schedule, std::vector<ActionInstance*>& queuedActions)
    {
        if (states.find(entity.id) == states.end())
            states[entity.id] = StateLog();

        StateLog log = states[entity.id];
        log.description.clear();

        log.description += _entityManager->getName(entity) + ": ";
        log.description += "[" + schedule + "]";

        if (queuedActions.size() != 0)
        {
            unsigned lastIndex = queuedActions.size() - 1;
            for (int i = lastIndex; i >= 0; --i)
            {
                if (i == lastIndex)
                    log.description += "-> " + queuedActions[i]->getActionName();
                else
                    log.description += ", " + queuedActions[i]->getActionName();
            }
        }

        states[entity.id] = log;
    }
};

#endif //PARTIALORDERPLANNER_EVENTLOGGER_H
