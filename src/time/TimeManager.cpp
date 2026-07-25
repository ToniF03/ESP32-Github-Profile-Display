/*
 * Created on: 2026-07-25
 * Author(s): Toni Fey
 * License: MIT
 * Description: Manages NTP synchronization, local time retrieval and
 *              formatted date/time strings for the ESP32 application.
 */

#include "TimeManager.h"

bool TimeManager::begin()
{
    configTime(TimeConfig::GmtOffset, TimeConfig::DstOffset, TimeConfig::Server);

    tm time;

    _synchronized = ::getLocalTime(&time);

    return _synchronized;
}

tm TimeManager::getLocalTime() const
{
    tm time;

    ::getLocalTime(&time);

    return time;
}

String TimeManager::getFormattedDate() const
{
    char timeStr[64];

    tm time;

    ::getLocalTime(&time);

    sprintf(timeStr, "%02d/%02d/%04d",
            time.tm_mday,
            time.tm_mon + 1,
            time.tm_year + 1900);

    return String(timeStr);
}

String TimeManager::getFormattedTime() const
{
    char timeStr[64];

    tm time;

    ::getLocalTime(&time);

    sprintf(timeStr, "%02d:%02d:%02d",
            time.tm_hour,
            time.tm_min,
            time.tm_sec);

    return String(timeStr);
}

String TimeManager::getFormattedDateTime() const
{
    char timeStr[64];

    tm time;

    ::getLocalTime(&time);

    sprintf(timeStr, "%02d/%02d/%04d %02d:%02d:%02d",
            time.tm_mday,
            time.tm_mon + 1,
            time.tm_year + 1900,
            time.tm_hour,
            time.tm_min,
            time.tm_sec);

    return String(timeStr);
}

uint64_t TimeManager::getSleepDurationUs() const
{
    tm time;
    float timeTillWakeUp;

    ::getLocalTime(&time);

    timeTillWakeUp -= (time.tm_min * 60 + time.tm_sec) * 1e6;

    return timeTillWakeUp;
}

uint8_t TimeManager::getWeekday() const {
    tm time;
    ::getLocalTime(&time);
    return time.tm_wday;
}