/*
 * Created on: 2026-07-25
 * Author(s): Toni Fey
 * License: MIT
 * Description: Timer class for measuring elapsed time
 */

#include "timer.h"

void Timer::begin()
{
    if (!_started)
    {
        _startTime = millis();
        _started = true;
    }
}

void Timer::reset()
{
    if (_started)
        _startTime = millis();
}

float Timer::elapsed() {
    return millis() - _startTime;
}