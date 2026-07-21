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