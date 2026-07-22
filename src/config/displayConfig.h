/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Display configuration for the ESP32 GitHub profile project — defines the
 *              e-paper display type alias and compile-time width/height constants.
 */

#pragma once

#include <Arduino.h>
#include <GxEPD2_BW.h>

#include "pins.h"

namespace DisplayConfig
{
    using DisplayType =
        GxEPD2_BW<
            GxEPD2_750_GDEY075T7,
            GxEPD2_750_GDEY075T7::HEIGHT>;

    constexpr uint16_t Width = 800;
    constexpr uint16_t Height = 480;
}