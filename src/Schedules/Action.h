//
// Created by Dan on 2/6/2016.
//

#ifndef PARTIALORDERPLANNER_ACTION_H
#define PARTIALORDERPLANNER_ACTION_H

#include <string>
#include "ActionInstance.h"

class Action {
private:
    std::string name;
    int id;
    double minDuration;
    double maxDuration;

    double calculateDuration() const;
public:
    Action(const std::string& name, int id) : name(name), id(id) {}
    ~Action(){}

    const std::string& getName() const { return name; }
    int getId() const { return id; }
    ActionInstance* createActionInstance(int index = 0) const;
};


#endif //PARTIALORDERPLANNER_ACTION_H
