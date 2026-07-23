/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Display renderer interface for the ESP32 GitHub profile dashboard and status screens
 */

#pragma once

#include "../config/displayConfig.h"
#include "../models/GitHubProfile.h"
#include "../models/GitHubStats.h"
#include "dithering.h"

class DisplayRenderer
{
public:
    explicit DisplayRenderer(DisplayConfig::DisplayType &display);

    void drawDashboard(const GitHubStats &stats,
                       const GitHubProfile &profile,
                       const DeviceInformation &deviceInformation);
    void drawConnectionError();
    void init(const int rotation, const uint16_t textColor);

private:
    DisplayConfig::DisplayType &display;

    Dithering dithering;

    void drawStatistics(const GitHubStats &stats);
    void drawHeatmap(const GitHubStats &stats, const DeviceInformation &deviceInformation);
    void drawFooter(const GitHubProfile &profile,
                    const DeviceInformation &deviceInformation);
};