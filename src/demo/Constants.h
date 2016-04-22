//
// Created by Dan on 4/21/2016.
//

#ifndef PARTIALORDERPLANNER_CONSTANTS_H
#define PARTIALORDERPLANNER_CONSTANTS_H

#include <glm-0.9.7.1/glm/vec3.hpp>
#include <string>

static const int GAME_WIDTH = 800;
static const int GAME_HEIGHT = 600;
static const int OUTPUT_MAX = 6;

static const glm::vec3 PLAYER_ACTION_COLOUR(0.5f, 0.8f, 0.5f);
static const glm::vec3 PLANNER_ACTION_COLOUR(0.8f, 0.8f, 0.2f);
static const glm::vec3 INPUT_COLOUR(0.8f, 0.8f, 0.8f);
static const glm::vec3 TIME_COLOUR(0.8f, 0.8f, 0.5f);

struct ActionOutput
{
    std::string output;
    glm::vec3 colour;
};

#endif //PARTIALORDERPLANNER_CONSTANTS_H
