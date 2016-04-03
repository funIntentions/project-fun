//
// Created by Dan on 2/6/2016.
//

#ifndef PARTIALORDERPLANNER_ACTION_H
#define PARTIALORDERPLANNER_ACTION_H

#include <string>
#include <vector>
#include <memory>
#include "ActionInstance.h"
#include "Predicate.h"
#include "components/Attribute.h"

class Action {
private:
    std::string name;
    int id;
    double minDuration;
    double maxDuration;

    double calculateDuration() const;
public:
    Action(const std::string& name, int id, double minDuration, double maxDuration)
            : name(name), id(id), minDuration(minDuration), maxDuration(maxDuration), attributes(std::vector<float>(Attribute::NumberOfAttributes)) {}
    ~Action(){}

    const std::string& getName() const { return name; }
    int getId() const { return id; }
    ActionInstance* createActionInstance() const;

    std::vector<float> attributes;
    std::vector<std::string> parameters;
    std::shared_ptr<Operator> actionOperator;
};



#endif //PARTIALORDERPLANNER_ACTION_H
