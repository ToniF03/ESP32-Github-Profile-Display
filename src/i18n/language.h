/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Declaration of localized string identifiers used by the ESP32
 *              GitHub profile display and status screens (i18n strings).
 */

#pragma once

struct Strings
{
    const char* contributions;
    const char* longestStreak;
    const char* currentStreak;
    const char* averagePerDay;
    const char* mostInDay;

    const char* wifiFailed;
    const char* excellent;
    const char* good;
    const char* fair;
    const char* weak;
    const char* noConnection;
};