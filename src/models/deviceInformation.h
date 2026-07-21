#pragma once

#include <Arduino.h>

struct DeviceInformation
{
    float battery = -1;
    int8_t WiFi_Strength;
    String WiFi_Description;
    char time_string[64];
    int weekday = 0;
};