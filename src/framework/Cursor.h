//
// Created by Dan on 4/3/2016.
//

#ifndef PARTIALORDERPLANNER_CURSOR_H
#define PARTIALORDERPLANNER_CURSOR_H

#include <string>

struct Cursor
{
    float displayInterval;
    float timeLeft;
    bool displaying;

    Cursor(float displayInterval) : displayInterval(displayInterval), timeLeft(displayInterval), displaying(true)
    {

    }

    void update(float period)
    {
        timeLeft -= period;
        if (timeLeft <= 0)
        {
            timeLeft = displayInterval;
            displaying = !displaying;
        }
    }

    void display()
    {
        displaying = true;
        timeLeft = displayInterval;
    }

    std::string displayCursor()
    {
        return displaying ? "|" : "";
    }
};

#endif //PARTIALORDERPLANNER_CURSOR_H
