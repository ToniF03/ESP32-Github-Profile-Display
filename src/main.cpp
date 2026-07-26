/*
 * Created on: 2025-09-17
 * Author(s): Toni Fey
 * License: MIT
 * Description: ESP32 GitHub profile display for contribution stats and streaks
 */

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
#include "WiFiManager/WiFiManager.h"

DeviceInformation deviceInformation;
GitHubProfile profile;
GitHubStats stats;
GitHubParser ghParser(GITHUB_USERNAME);
DisplayRenderer renderer;
TimeManager tm;
WiFiManager wifimg;

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
 * Setup function - runs once at startup
 * Initializes display, connects to WiFi, fetches GitHub data,
 * renders the display, and enters deep sleep
 */
void setup()
{
  Serial.begin(115200);

  renderer.init(0, GxEPD_BLACK);

  if (!wifimg.init()) {
    renderer.drawConnectionError();
    goDeepSleep();
  }

  deviceInformation.WiFi_Strength = wifimg.RSSI();
  deviceInformation.WiFi_Description = wifimg.getWiFidesc();

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
