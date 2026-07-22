/*
* Created on: 2025-09-17
* Author(s): Toni Fey
* License: MIT
 * Description: ESP32 GitHub profile display for contribution stats and streaks
*/

// Libraries for ESP32 functionality
#include <ArduinoJson.h>       // JSON parsing for API responses
#include <GxEPD2_BW.h>         // E-paper display driver
#include <WiFi.h>              // WiFi connectivity
#include <WiFiClientSecure.h>  // Secure HTTPS client
#include <HTTPClient.h>        // HTTP client for API calls
#include <time.h>              // Time and date functions

// Project resources
#include <resources/credentials.h>

// Project configs
#include "config/pins.h"
#include "config/networkConfig.h"
#include "config/timeConfig.h"
#include "config/displayConfig.h"
#include "config/layout.h"
#include "i18n/i18n.h"
#include "models/deviceInformation.h"
#include "models/GitHubProfile.h"
#include "models/GitHubStats.h"

// Initialize 7.5" e-paper display
GxEPD2_BW<GxEPD2_750_GDEY075T7, GxEPD2_750_GDEY075T7::HEIGHT> display(GxEPD2_750_GDEY075T7(Pins::CS, Pins::DC, Pins::RST, Pins::BSY));

DeviceInformation deviceInformation;
GitHubProfile profile;
GitHubStats stats;

float timeTillWakeUp = 3.6e9; // 1 hour in microseconds

/**
 * Put the ESP32 into deep sleep mode to save power
 * Wakes up after 1 hour to refresh the display
 */
void goDeepSleep()
{
  display.hibernate();
  // Go to deep sleep for 1 hour (3.6e9 microseconds = 3,600,000,000 µs)
  Serial.println(TimeConfig::SleepTime / 1e6);
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

// Global time structure for NTP time synchronization
struct tm timeinfo;

/**
 * Fetch data from a given URL using HTTPS
 * @param URL The HTTPS URL to fetch data from
 * @return Response payload as a String, or empty string on failure
 */
String receiveData(const char *URL)
{
  String payload;
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;

  if (https.begin(client, URL))
  {                             // Use the secure client
    int httpCode = https.GET(); // Perform the GET request

    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK)
      {
        payload = https.getString();
      }
    }
    else
    {
      Serial.printf("[HTTPS] GET failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end(); // Free resources
  }
  else
  {
    Serial.println("[HTTPS] Unable to connect");
  }
  return payload;
}

/**
 * Fetch GitHub contribution calendar data using GraphQL API
 * Retrieves one year of contribution data ending on current date
 * @param username GitHub username to query
 * @return JSON response containing contribution calendar data
 */
String receiveChartInformation(String username)
{
  String payload;
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient https;
  if (https.begin(client, "https://api.github.com/graphql"))
  {
    String authHeader = String("Bearer ") + GITHUB_PAT;
    https.addHeader("Authorization", authHeader);
    https.addHeader("Content-Type", "application/json");

    char timeStr[64];
    sprintf(timeStr, "%04d-%02d-%02d",
            timeinfo.tm_year + 1900,
            timeinfo.tm_mon + 1,
            timeinfo.tm_mday);

    timeinfo.tm_year -= 1;
    time_t adjusted = mktime(&timeinfo);

    const int n = deviceInformation.weekday == 7 ? 0 : deviceInformation.weekday;
    adjusted -= (time_t)n * 24 * 60 * 60;

    localtime_r(&adjusted, &timeinfo);

    char timeStr2[64];
    sprintf(timeStr2, "%04d-%02d-%02d",
            timeinfo.tm_year + 1900,
            timeinfo.tm_mon + 1,
            timeinfo.tm_mday);

    String graphQLQuery = String("{\"query\":\"query { user(login: \\\"") + username + "\\\") { contributionsCollection(from: \\\"" + timeStr2 + "T00:00:00Z\\\", to: \\\"" + timeStr + "T23:59:59Z\\\") { contributionCalendar { totalContributions weeks { contributionDays { date contributionCount } } } } } }\"}";

    int httpCode = https.POST(graphQLQuery); // Perform the GET request

    if (httpCode > 0)
    {
      if (httpCode == HTTP_CODE_OK)
      {
        payload = https.getString();
      }
    }
    else
    {
      Serial.printf("[HTTPS] GET failed, error: %s\n", https.errorToString(httpCode).c_str());
    }

    https.end(); // Free resources
  }
  return payload;
}

/**
 * Setup function - runs once at startup
 * Initializes display, connects to WiFi, fetches GitHub data,
 * renders the display, and enters deep sleep
 */
void setup()
{
  Serial.begin(115200);
  display.init();
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  initWiFi();

  // Configure NTP time sync (GMT+1, DST offset 1 hour)
  configTime(3600, 3600, "pool.ntp.org");
  int tries = 0;
  Serial.print("Obtaining time");
  while (tries < 10 && !getLocalTime(&timeinfo))
  {
    Serial.print(".");
    tries++;
    delay(100);
  }
  if (tries == 10)
  {
    Serial.println("Failed to obtain time");
    failedConnection();
  }
  Serial.println();

  // Format and display current time
  Serial.println(&timeinfo, "Current time: %A, %B %d %Y %H:%M:%S");
  timeTillWakeUp -= (timeinfo.tm_min * 60 + timeinfo.tm_sec) * 1e6;
  char timeStr[64];
  sprintf(timeStr, "%02d/%02d/%04d %02d:%02d:%02d",
          timeinfo.tm_mday,
          timeinfo.tm_mon + 1,
          timeinfo.tm_year + 1900,
          timeinfo.tm_hour,
          timeinfo.tm_min,
          timeinfo.tm_sec);
  sprintf(deviceInformation.time_string, "%02d/%02d/%04d %02d:%02d:%02d",
          timeinfo.tm_mday,
          timeinfo.tm_mon + 1,
          timeinfo.tm_year + 1900,
          timeinfo.tm_hour,
          timeinfo.tm_min,
          timeinfo.tm_sec);
  deviceInformation.weekday = timeinfo.tm_wday;

  // Fetch GitHub profile data (followers, following, name)

  String profileJson = receiveData(String(String("https://api.github.com/users/") + GITHUB_USERNAME).c_str());
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, profileJson);
  if (error)
  {
    Serial.print("Error occured while fetching forecast: ");
    Serial.println(error.c_str());
  }

  profile.followers = doc["followers"].as<int>();
  profile.following = doc["following"].as<int>();
  profile.name = doc["name"].as<String>();

  // Array to store daily contribution counts for the past year (53 weeks * 7 days)
  error = deserializeJson(doc, receiveChartInformation(GITHUB_USERNAME));
  if (error)
  {
    Serial.print("Error occured while fetching forecast: ");
    Serial.println(error.c_str());
  }

  // Initialize statistics variables
  int longestStreak = 0;        // Longest consecutive days with contributions
  int streak = 0;               // Temporary streak counter
  int maxContributions = 0;     // Highest contributions in a single day
  int daysWithContributions = 0; // Total days with at least one contribution
  float avgContributions = 0.0;  // Average contributions per day
  char *wifiStrengthDesc = (char *)getWiFidesc(deviceInformation.WiFi_Strength);

  stats.contributions = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["totalContributions"].as<int>();
  for (int i = 0; i <= 371; i++)
  {
    stats.commits[i] = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][i / 7]["contributionDays"][i % 7]["contributionCount"].as<int>();

    // Calculate longest streak
    if (stats.commits[i] > 0)
    {
      streak++;
      if (streak > longestStreak)
        longestStreak = streak;
    }
    else
    {
      streak = 0;
    }

    // Calculate max contributions in a day
    if (stats.commits[i] > maxContributions)
    {
      maxContributions = stats.commits[i];
    }

    // Calculate days with contributions
    if (stats.commits[i] > 0)
    {
      daysWithContributions++;
    }
  }

  // Calculate average contributions per day, rounded to 2 decimal places
  avgContributions = (float)stats.contributions / (365 + deviceInformation.weekday);
  avgContributions = roundf(avgContributions * 100) / 100;

  // Calculate current active streak (consecutive days from today backwards)
  int currentStreak = 0;
  for (int i = 371 - (7 - deviceInformation.weekday); i >= 0; i--)
  {
    if (stats.commits[i] > 0)
      currentStreak++;
    else
      break;
  }

  // Main display rendering loop
  do
  {
    display.clearScreen();

    int16_t tbx, tby;   // Text bounding box x, y
    uint16_t tbw, tbh;  // Text bounding box width, height

    // Footer

    

    // Statistics

    // Heatmap


  } while (display.nextPage());
  
  // Enter deep sleep to conserve power until next update
  goDeepSleep();
}

// Loop function - not used since device enters deep sleep after setup
void loop() {}
