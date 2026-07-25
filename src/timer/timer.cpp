/*
 * Created on: 2026-07-25
 * Author(s): Toni Fey
 * License: MIT
 * Description: Timer class for measuring elapsed time
 */

#include "timer.h"

void Timer::begin()
{
    if (!started)
    {
        startTime = millis();
        started = true;
    }
}

void Timer::reset()
{
    if (started)
        startTime = millis();
}

float Timer::elapsed() {
    return millis() - startTime;
}