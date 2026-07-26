/*
 * Created on: 2026-07-26
 * Author(s): Toni Fey
 * License: MIT
 * Description:
 */

#pragma once

#include <Arduino.h>
#include <WiFi.h>

#include "i18n/i18n.h"
#include "config/networkConfig.h"
#include "resources/credentials.h"
#include "timer/timer.h"

class WiFiManager
{

public:
    bool init();
    bool init(const char *SSID, const char *PASSWORD);
    const char *getWiFidesc();
    inline void printWiFiInformation();
    int8_t RSSI();

private:
    RTC_DATA_ATTR uint8_t storedBSSID[6];
    RTC_DATA_ATTR int32_t storedChannel = 0;
    RTC_DATA_ATTR uint32_t storedIP = 0;
    RTC_DATA_ATTR uint32_t storedGateway = 0;
    RTC_DATA_ATTR uint32_t storedSubnet = 0;
    RTC_DATA_ATTR uint32_t storedDNS = 0;
};