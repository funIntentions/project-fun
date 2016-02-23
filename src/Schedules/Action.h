//
// Created by Dan on 2/6/2016.
//

#ifndef PARTIALORDERPLANNER_ACTION_H
#define PARTIALORDERPLANNER_ACTION_H

#include <string>
#include <vector>
#include "ActionInstance.h"
#include "Predicate.h"

class Action {
private:
    std::string name;
    int id;
    double minDuration;
    double maxDuration;

    double calculateDuration() const;
public:
    Action(const std::string& name, int id, double minDuration, double maxDuration)
            : name(name), id(id), minDuration(minDuration), maxDuration(maxDuration) {}
    ~Action(){}

    const std::string& getName() const { return name; }
    int getId() const { return id; }
    ActionInstance* createActionInstance(int index = 0) const;

    std::vector<std::string> parameters;
    std::vector<Predicate> preconditions;
    std::vector<Predicate> addedEffects;
    std::vector<Predicate> subtractedEffects;
};


#endif //PARTIALORDERPLANNER_ACTION_H
