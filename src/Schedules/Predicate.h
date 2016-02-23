//
// Created by Dan on 2/22/2016.
//

#ifndef PARTIALORDERPLANNER_PREDICATE_H
#define PARTIALORDERPLANNER_PREDICATE_H

#include <string>
#include <vector>

struct Predicate
{
    std::string type;
    std::vector<std::string> params;

    bool operator==(const Predicate& other) const {
        return (type == other.type
                && params == other.params);
    }
};

namespace std {
    template <>
    struct hash<Predicate>
    {
        std::size_t operator()(const Predicate& predicate) const
        {
            std::size_t seed = 0;
            for(auto& param : predicate.params) {
                seed ^= std::hash<std::string>()(param) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            }

            return (std::hash<std::string>()(predicate.type)) ^ seed;
        }
    };
}

#endif //PARTIALORDERPLANNER_PREDICATE_H
