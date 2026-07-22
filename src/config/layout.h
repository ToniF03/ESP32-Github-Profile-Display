/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Layout constants for positioning the ESP32 GitHub profile dashboard and status screens
 */

#pragma once

#include <Arduino.h>

namespace Layout
{
    constexpr int StatisticsTop = 20;
    constexpr int StatisticsLeft = 20;

    constexpr int HeatmapX = 5;
    constexpr int HeatmapY = 220;

    constexpr int LeftCardX = 350;
    constexpr int RightCardX = 555;

    constexpr int cardTextMargin = 15;
    constexpr int cardStrokeWidth = 15;
}