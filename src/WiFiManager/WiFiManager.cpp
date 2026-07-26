/*
 * Created on: 2026-07-26
 * Author(s): Toni Fey
 * License: MIT
 * Description:
 */

#include "WiFiManager.h"

int8_t WiFiManager::RSSI()
{
    return WiFi.RSSI();
}

const char *WiFiManager::getWiFidesc()
{
    int8_t rssi = WiFi.RSSI();
    if (rssi == 0)
    {
        return getStrings().noConnection;
    }
    else if (rssi >= -50)
    {
        return getStrings().excellent;
    }
    else if (rssi >= -60)
    {
        return getStrings().good;
    }
    else if (rssi >= -70)
    {
        return getStrings().fair;
    }
    else
    { // rssi < -70
        return getStrings().weak;
    }
}

inline void WiFiManager::printWiFiInformation()
{
    Serial.println("--------------------------------");
    Serial.println();
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    Serial.print("Connected as ");
    Serial.println(WiFi.getHostname());
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("MAC-Address: ");
    Serial.println(WiFi.macAddress());
    Serial.print("Connection Strength: ");
    Serial.print(getWiFidesc());
    Serial.print(" (");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm)");
    Serial.println();
    Serial.println("--------------------------------");
}

bool WiFiManager::init()
{
    return init(WIFI_SSID, WIFI_PASSWORD);
}

bool WiFiManager::init(const char *SSID, const char *PASSWORD)
{
    WiFi.setHostname(Network::Hostname);

    if (storedChannel != 0 && storedIP != 0)
    {
        WiFi.config(IPAddress(storedIP), IPAddress(storedGateway), IPAddress(storedSubnet), IPAddress(storedDNS));
        WiFi.begin(SSID, PASSWORD, storedChannel, storedBSSID, true);
    }
    else
        WiFi.begin(SSID, PASSWORD);

    Timer timer;
    timer.begin();

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(200);

        if (timer.elapsed() >= Network::Timeout)
        {
            return false;
        }
    }
    Serial.println();

    if (storedIP == 0)
    {
        storedChannel = WiFi.channel();
        memcpy(storedBSSID, WiFi.BSSID(), 6);
        storedIP = WiFi.localIP();
        storedGateway = WiFi.gatewayIP();
        storedSubnet = WiFi.subnetMask();
        storedDNS = WiFi.dnsIP();
    }

    delay(10);
    
    return true;
}