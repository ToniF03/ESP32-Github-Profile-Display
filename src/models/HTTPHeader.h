/*
 * Created on: 2026-07-25
 * Author(s): Toni Fey
 * License: MIT
 * Description: Represents a single HTTP header (key/value pair) used when
 *              sending or parsing HTTP requests and responses. This lightweight
 *              struct is intended for use with Arduino/ESP32 networking
 *              utilities within the project.
 */

#pragma once

#include <Arduino.h>

struct HTTPHeader {
    String key;
    String value;
};