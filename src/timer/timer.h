/*
 * Created on: 2026-07-25
 * Author(s): Toni Fey
 * License: MIT
 * Description: Timer class for measuring elapsed time
 */

#pragma once

#include <Arduino.h>

class Timer
{
public:
    void begin();
    void reset();
    float elapsed();

private:
    float _startTime = -1;
    bool _started = true;
};