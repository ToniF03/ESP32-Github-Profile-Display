/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Network configuration settings for the ESP32 GitHub profile display
 */

#pragma once

#include <Arduino.h>

namespace Network
{
    const char Hostname[] = "PixelPioneer GitHub Display";

    const uint32_t Timeout = 30000;
}