/*
 * Created on: 2026-07-25
 * Author(s): Toni Fey
 * License: MIT
 * Description: Manages NTP synchronization, local time retrieval and
 *              formatted date/time strings for the ESP32 application.
 */

#pragma once

#include <Arduino.h>

#include "../config/timeConfig.h"

class TimeManager
{
public:
    bool begin();
    bool isSynchronized() const;
    tm getLocalTime() const;
    String getFormattedDate() const;
    String getFormattedTime() const;
    String getFormattedDateTime() const;
    uint64_t getSleepDurationUs() const;

private:
    bool _synchronized = false;
};