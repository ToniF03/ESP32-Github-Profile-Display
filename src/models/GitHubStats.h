/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Data model for storing GitHub profile statistics used by the ESP32 dashboard and status screens
 */

#pragma once

struct GitHubStats
{
    int contributions;
    int longestStreak;
    int currentStreak;
    int maxContributions;
    float averageContributions;
    uint8_t commits[372];
};