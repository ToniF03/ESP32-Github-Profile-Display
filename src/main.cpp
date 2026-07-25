/*
 * Created on: 2025-09-17
 * Author(s): Toni Fey
 * License: MIT
 * Description: ESP32 GitHub profile display for contribution stats and streaks
 */

// Libraries for ESP32 functionality
#include <WiFi.h> // WiFi connectivity

// Project resources
#include <resources/credentials.h>

// Project configs
#include "config/pins.h"
#include "config/networkConfig.h"
#include "config/timeConfig.h"
#include "config/displayConfig.h"
#include "config/layout.h"

// Project includes
#include "GitHub/GitHubParser.h"
#include "i18n/i18n.h"
#include "models/deviceInformation.h"
#include "models/GitHubProfile.h"
#include "models/GitHubStats.h"
#include "display/displayRenderer.h"
#include "time/TimeManager.h"

// Initialize 7.5" e-paper display
GxEPD2_BW<GxEPD2_750_GDEY075T7, GxEPD2_750_GDEY075T7::HEIGHT> display(GxEPD2_750_GDEY075T7(Pins::CS, Pins::DC, Pins::RST, Pins::BSY));

DeviceInformation deviceInformation;
GitHubProfile profile;
GitHubStats stats;
GitHubParser ghParser(GITHUB_USERNAME);
DisplayRenderer renderer;
TimeManager tm;

/**
 * Put the ESP32 into deep sleep mode to save power
 * Wakes up after 1 hour to refresh the display
 */
void goDeepSleep()
{
  renderer.hibernate();
  // Go to deep sleep for 1 hour (3.6e9 microseconds = 3,600,000,000 µs)
  esp_sleep_enable_timer_wakeup(TimeConfig::SleepTime);
  Serial.println("ESP goes to deep sleep now");
  Serial.flush();
  esp_deep_sleep_start();
}

/**
 * Display a WiFi connection failure message on the e-paper screen
 * and then enter deep sleep mode
 */
void failedConnection()
{
  renderer.drawConnectionError();
  goDeepSleep();
}

/**
 * Convert WiFi signal strength (RSSI) to human-readable description
 * @param rssi Signal strength in dBm
 * @return String description of signal quality
 */
const char *getWiFidesc(int rssi)
{
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

/**
 * Initialize WiFi connection with timeout and error handling
 * Sets hostname and connects to configured network
 * Calls failedConnection() if unable to connect within 30 seconds
 */
void initWiFi()
{
  WiFi.setHostname(Network::Hostname);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  float connectionBegin = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
    if (millis() - connectionBegin >= Network::Timeout)
    {
      failedConnection();
    }
  }
  Serial.println();
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
  Serial.print(getWiFidesc(WiFi.RSSI()));
  Serial.print(" (");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm)");
  Serial.println();
  Serial.println("--------------------------------");
  deviceInformation.WiFi_Strength = WiFi.RSSI();
  deviceInformation.WiFi_Description = getWiFidesc(WiFi.RSSI());
  delay(10);
}

/**
 * Setup function - runs once at startup
 * Initializes display, connects to WiFi, fetches GitHub data,
 * renders the display, and enters deep sleep
 */
void setup()
{
  Serial.begin(115200);

  renderer.init(0, GxEPD_BLACK);

  initWiFi();

  tm.begin();

  // Format and display current time
  Serial.println(tm.getFormattedDateTime());
  strcpy(deviceInformation.time_string, tm.getFormattedDateTime().c_str());
  deviceInformation.weekday = tm.getWeekday();

  profile = ghParser.getProfile();
  stats = ghParser.getStatistics(deviceInformation.weekday);

  // Draw the GitHub Dashboard
  renderer.drawDashboard(stats, profile, deviceInformation);

  // Enter deep sleep to conserve power until next update
  goDeepSleep();
}

// Loop function - not used since device enters deep sleep after setup
void loop() {}
