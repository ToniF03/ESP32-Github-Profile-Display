#pragma once

#include <Arduino.h>

namespace Pins
{

    // ------------------- Display -------------------
    constexpr uint8_t BSY = 4;  // Busy pin
    constexpr uint8_t RST = 16; // Reset pin
    constexpr uint8_t DC = 17;  // Data/Command pin
    constexpr uint8_t CS = 5;   // Chip Select pin

}
