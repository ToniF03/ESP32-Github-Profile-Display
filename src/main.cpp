// Libraries for ESP32 functionality
#include <ArduinoJson.h>        // JSON parsing for API responses
#include <GxEPD2_BW.h>         // E-paper display driver
#include <WiFi.h>              // WiFi connectivity
#include <WiFiClientSecure.h>  // Secure HTTPS client
#include <HTTPClient.h>        // HTTP client for API calls
#include <time.h>              // Time and date functions

// Project resources
#include <resources/icons/icons.h>
#include <resources/fonts/fonts.h>
#include <resources/credentials.h>

// E-paper display pin definitions
#define BSY 4   // Busy pin
#define RST 16  // Reset pin
#define DC 17   // Data/Command pin
#define CS 5    // Chip Select pin

/*
 * Screen Connection
 * BSY  ->  4
 * RST  ->  16
 * DC   ->  17
 * CS   ->  5
 * SCK  ->  18
 * SDI  ->  23
 */

// Initialize 7.5" e-paper display
GxEPD2_BW<GxEPD2_750_GDEY075T7, GxEPD2_750_GDEY075T7::HEIGHT> display(GxEPD2_750_GDEY075T7(CS, DC, RST, BSY));

// Bayer 4x4 dithering matrix for grayscale simulation on black/white display
const uint8_t bayer4x4[4][4] = {
    {0, 8, 2, 10},
    {12, 4, 14, 6},
    {3, 11, 1, 9},
    {15, 7, 13, 5}};

// Global variables for GitHub data
int contributions = 0;      // Total contributions in the last year
String GITHUB_NAME;         // User's display name
int followers = 0;          // Number of followers
int following = 0;          // Number of following
int8_t WiFiStrength = 0;    // WiFi signal strength in dBm

/**
 * Determine if a pixel should be black using Bayer dithering
 * @param x X coordinate of the pixel
 * @param y Y coordinate of the pixel
 * @param level Grayscale level (0=white, 17=black, 1-16=dithered)
 * @return true if pixel should be black, false if white
 */
inline bool isPixelBlack(uint16_t x, uint16_t y, uint8_t level)
{
  if (level == 0)
    return false; // totally white
  if (level == 17)
    return true; // totally black
  // map level 1-16 to threshold 0-15
  uint8_t threshold = level - 1;
  return bayer4x4[y % 4][x % 4] < threshold;
}

/**
 * Draw a single pixel with dithered grayscale
 * @param x X coordinate
 * @param y Y coordinate
 * @param level Grayscale level (0-17)
 */
void drawGrayPixel(int x, int y, uint8_t level)
{
  if (isPixelBlack(x, y, level))
    display.drawPixel(x, y, GxEPD_BLACK);
  else
    display.drawPixel(x, y, GxEPD_WHITE);
}

/**
 * Draw a rectangle outline with dithered grayscale
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param w Width
 * @param h Height
 * @param level Grayscale level (0-17)
 */
void drawGrayRect(int x, int y, int w, int h, uint8_t level)
{
  for (int i = x; i < x + w; i++)
  {
    drawGrayPixel(i, y, level);
    drawGrayPixel(i, y + h - 1, level);
  }
  for (int j = y; j < y + h; j++)
  {
    drawGrayPixel(x, j, level);
    drawGrayPixel(x + w - 1, j, level);
  }
}

/**
 * Fill a rectangle with dithered grayscale
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param w Width
 * @param h Height
 * @param level Grayscale level (0-17)
 */
void fillGrayRect(int x, int y, int w, int h, uint8_t level)
{
  for (int j = y; j < y + h; j++)
  {
    for (int i = x; i < x + w; i++)
    {
      drawGrayPixel(i, j, level);
    }
  }
}

/**
 * Draw a line using Bresenham's algorithm with dithered grayscale
 * @param x0 Start X coordinate
 * @param y0 Start Y coordinate
 * @param x1 End X coordinate
 * @param y1 End Y coordinate
 * @param level Grayscale level (0-17)
 */
void drawGrayLine(int x0, int y0, int x1, int y1, uint8_t level)
{
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;
  while (true)
  {
    drawGrayPixel(x0, y0, level);
    if (x0 == x1 && y0 == y1)
      break;
    e2 = 2 * err;
    if (e2 >= dy)
    {
      err += dy;
      x0 += sx;
    }
    if (e2 <= dx)
    {
      err += dx;
      y0 += sy;
    }
  }
}

/**
 * Draw a circle outline using midpoint circle algorithm with dithered grayscale
 * @param xc Center X coordinate
 * @param yc Center Y coordinate
 * @param r Radius
 * @param level Grayscale level (0-17)
 */
void drawGrayCircle(int xc, int yc, int r, uint8_t level)
{
  int x = 0, y = r;
  int d = 3 - 2 * r;
  while (y >= x)
  {
    drawGrayPixel(xc + x, yc + y, level);
    drawGrayPixel(xc - x, yc + y, level);
    drawGrayPixel(xc + x, yc - y, level);
    drawGrayPixel(xc - x, yc - y, level);
    drawGrayPixel(xc + y, yc + x, level);
    drawGrayPixel(xc - y, yc + x, level);
    drawGrayPixel(xc + y, yc - x, level);
    drawGrayPixel(xc - y, yc - x, level);
    x++;
    if (d > 0)
    {
      y--;
      d = d + 4 * (x - y) + 10;
    }
    else
    {
      d = d + 4 * x + 6;
    }
  }
}

/**
 * Fill a circle using midpoint circle algorithm with dithered grayscale
 * @param xc Center X coordinate
 * @param yc Center Y coordinate
 * @param r Radius
 * @param level Grayscale level (0-17)
 */
void fillGrayCircle(int xc, int yc, int r, uint8_t level)
{
  int x = 0, y = r;
  int d = 3 - 2 * r;
  while (y >= x)
  {
    for (int i = xc - x; i <= xc + x; i++)
    {
      drawGrayPixel(i, yc + y, level);
      drawGrayPixel(i, yc - y, level);
    }
    for (int i = xc - y; i <= xc + y; i++)
    {
      drawGrayPixel(i, yc + x, level);
      drawGrayPixel(i, yc - x, level);
    }
    x++;
    if (d > 0)
    {
      y--;
      d = d + 4 * (x - y) + 10;
    }
    else
    {
      d = d + 4 * x + 6;
    }
  }
}

/**
 * Draw a rounded rectangle outline with dithered grayscale
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param w Width
 * @param h Height
 * @param radius Corner radius
 * @param level Grayscale level (0-17)
 */
void drawGrayRoundRect(int x, int y, int w, int h, int radius, uint8_t level)
{
  drawGrayCircle(x + radius, y + radius, radius, level);
  drawGrayCircle(x + w - radius - 1, y + radius, radius, level);
  drawGrayCircle(x + radius, y + h - radius - 1, radius, level);
  drawGrayCircle(x + w - radius - 1, y + h - radius - 1, radius, level);
  drawGrayRect(x + radius, y, w - 2 * radius, 1, level);
  drawGrayRect(x + radius, y + h - 1, w - 2 * radius, 1, level);
  drawGrayRect(x, y + radius, 1, h - 2 * radius, level);
  drawGrayRect(x + w - 1, y + radius, 1, h - 2 * radius, level);
}

/**
 * Fill a rounded rectangle with dithered grayscale
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param w Width
 * @param h Height
 * @param radius Corner radius
 * @param level Grayscale level (0-17)
 */
void fillGrayRoundRect(int x, int y, int w, int h, int radius, uint8_t level)
{
  fillGrayRect(x + radius, y, w - 2 * radius, h, level);
  fillGrayRect(x, y + radius, w, h - 2 * radius, level);
  fillGrayCircle(x + radius, y + radius, radius, level);
  fillGrayCircle(x + w - radius - 1, y + radius, radius, level);
  fillGrayCircle(x + radius, y + h - radius - 1, radius, level);
  fillGrayCircle(x + w - radius - 1, y + h - radius - 1, radius, level);
}

float timeTillWakeUp = 3.6e9; // 1 hour in microseconds

/**
 * Put the ESP32 into deep sleep mode to save power
 * Wakes up after 1 hour to refresh the display
 */
void goDeepSleep()
{
  display.hibernate();
  // Go to deep sleep for 1 hour (3.6e9 microseconds = 3,600,000,000 µs)
  Serial.println(timeTillWakeUp / 1e6);
  esp_sleep_enable_timer_wakeup(timeTillWakeUp);
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
  display.setFont(&Roboto_Regular_11pt8b);
  do
  {
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(302, 142, 196, 196, GxEPD_BLACK);
    fillGrayRect(430, 142, 68, 98, 7);
    display.drawBitmap(302, 142, wifi_x_196x196, 196, 196, GxEPD_WHITE);

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds("WiFi Connection", 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(400 - (tbw / 2), 368); // 30 px top margin
    display.print("WiFi Connection");
    display.getTextBounds("failed", 0, 368 + tbh + 30, &tbx, &tby, &tbw, &tbh);
    display.setCursor(400 - (tbw / 2), tby);
    display.print("failed");
  } while (display.nextPage());
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
    return "No Connection";
  }
  else if (rssi >= -50)
  {
    return "Excellent";
  }
  else if (rssi >= -60)
  {
    return "Good";
  }
  else if (rssi >= -70)
  {
    return "Fair";
  }
  else
  { // rssi < -70
    return "Weak";
  }
}

/**
 * Initialize WiFi connection with timeout and error handling
 * Sets hostname and connects to configured network
 * Calls failedConnection() if unable to connect within 30 seconds
 */
void initWiFi()
{
  WiFi.setHostname("PixelPioneer GitHub ePaper Screen");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  float connectionBegin = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(200);
    if (millis() - connectionBegin >= 30000)
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
  WiFiStrength = WiFi.RSSI();
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

// Current day of week (0=Sunday, 6=Saturday)
int weekday = 0;

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

    const int n = weekday == 7 ? 0 : weekday;
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
  weekday = timeinfo.tm_wday;

  // Fetch GitHub profile data (followers, following, name)

  String profileJson = receiveData(String(String("https://api.github.com/users/") + GITHUB_USERNAME).c_str());
  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, profileJson);
  if (error)
  {
    Serial.print("Error occured while fetching forecast: ");
    Serial.println(error.c_str());
  }

  followers = doc["followers"].as<int>();
  following = doc["following"].as<int>();
  GITHUB_NAME = doc["name"].as<String>();

  // Array to store daily contribution counts for the past year (53 weeks * 7 days)
  uint8_t commits[372];
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
  char *wifiStrengthDesc = (char *)getWiFidesc(WiFiStrength);

  contributions = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["totalContributions"].as<int>();
  for (int i = 0; i <= 371; i++)
  {
    commits[i] = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][i / 7]["contributionDays"][i % 7]["contributionCount"].as<int>();

    // Calculate longest streak
    if (commits[i] > 0)
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
    if (commits[i] > maxContributions)
    {
      maxContributions = commits[i];
    }

    // Calculate days with contributions
    if (commits[i] > 0)
    {
      daysWithContributions++;
    }
  }

  // Calculate average contributions per day, rounded to 2 decimal places
  avgContributions = (float)contributions / (365 + weekday);
  avgContributions = roundf(avgContributions * 100) / 100;

  // Calculate current active streak (consecutive days from today backwards)
  int currentStreak = 0;
  for (int i = 371 - (7 - weekday); i >= 0; i--)
  {
    if (commits[i] > 0)
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

    // Display GitHub username and full name in footer
    display.setFont(&Roboto_Regular_6pt8b);
    display.fillRect(0, 464, 16, 16, GxEPD_BLACK);
    display.drawBitmap(0, 464, sy_github_16x16, 16, 16, GxEPD_WHITE);
    display.getTextBounds(String(GITHUB_USERNAME) + " (" + GITHUB_NAME + ")", 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(20, 480 - tbh * 0.25);
    display.print(String(GITHUB_USERNAME) + " (" + GITHUB_NAME + ")");

    // Display current date and time in footer
    display.getTextBounds(timeStr, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(795 - tbw, 480 - tbh * 0.33);
    display.print(timeStr);
    display.fillRect(770 - tbw, 464, 16, 16, GxEPD_BLACK);
    display.drawBitmap(770 - tbw, 464, wi_time_1_16x16, 16, 16, GxEPD_WHITE);

    // Display WiFi signal strength with appropriate icon
    display.getTextBounds(String(wifiStrengthDesc) + " (" + WiFiStrength + " dbm)", 770 - tbw, 480, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx - 10 - tbw, 480 - tbh * 0.33);
    display.print(String(wifiStrengthDesc) + " (" + WiFiStrength + " dBm)");
    display.fillRect(tbx - 31 - tbw, 464, 16, 16, GxEPD_BLACK);

    if (wifiStrengthDesc == "Excellent")
    display.drawBitmap(tbx - 31 - tbw, 464, wifi_16x16, 16, 16, GxEPD_WHITE);
    else if (wifiStrengthDesc == "Good")
      display.drawBitmap(tbx - 31 - tbw, 464, wifi_3_bar_16x16, 16, 16, GxEPD_WHITE);
    else if (wifiStrengthDesc == "Fair")
      display.drawBitmap(tbx - 31 - tbw, 464, wifi_2_bar_16x16, 16, 16, GxEPD_WHITE);
    else if (wifiStrengthDesc == "Weak")
      display.drawBitmap(tbx - 31 - tbw, 464, wifi_1_bar_16x16, 16, 16, GxEPD_WHITE);
    

    // Print total contributions
    fillGrayRoundRect(20, 20, 15, 176, 3, 4);
    display.setFont(&Roboto_Regular_48pt8b);
    display.getTextBounds(String(contributions), 50, 216, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby);
    display.print(contributions);

    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds("Contributions in the last year", tbx, 0.66 * tby + tbh, &tbx, &tby, &tbw, &tbh);
    display.setCursor(55, tby + tbh + 10);
    display.print("Contributions in the last year");

    // Print longest streak
    fillGrayRoundRect(350, 20, 15, 83, 3, 4);
    display.setFont(&Roboto_Regular_24pt8b);
    display.getTextBounds(String(longestStreak), 380, 55, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    display.setCursor(tbx, tby);
    display.print(longestStreak);
    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds("Longest Streak", tbx, tby, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby + 1.875 * tbh + 5);
    display.print("Longest Streak");

    // Print max contributions in a day
    fillGrayRoundRect(350, 113, 15, 83, 3, 4);
    display.setFont(&Roboto_Regular_24pt8b);
    display.getTextBounds(String(maxContributions), 380, 148, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    display.setCursor(tbx, tby);
    display.print(maxContributions);
    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds("Most in a Day", tbx, tby, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby + 1.875 * tbh + 5);
    display.print("Most in a Day");

    // Print current streak
    fillGrayRoundRect(555, 20, 15, 83, 3, 4);
    display.setFont(&Roboto_Regular_24pt8b);
    display.getTextBounds(String(currentStreak), 575, 55, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    display.setCursor(tbx, tby);
    display.print(currentStreak);
    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds("Current Streak", tbx, tby, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby + 1.875 * tbh + 5);
    display.print("Current Streak");

    // Print average contributions
    fillGrayRoundRect(555, 113, 15, 83, 3, 4);
    display.setFont(&Roboto_Regular_24pt8b);
    display.getTextBounds(String(avgContributions), 575, 148, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    display.setCursor(tbx, tby);
    display.print(avgContributions);
    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds("Average per Day", tbx, tby, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby + 1.875 * tbh + 5);
    display.print("Average per Day");

    // Render contribution heatmap (53 weeks x 7 days)
    for (int week = 0; week < 53; week++)
    {
      for (int day = 0; day < 7; day++)
      {
        if (week == 52 && day > weekday)
          break;
        int index = week * 7 + day;
        if (index >= 371)
          break;
        // Map contribution count to grayscale level (3=light, 16=dark)
        int color = map(commits[index], 0, maxContributions, 3, 16);
        fillGrayRoundRect(5 + week * 15, 220 + day * 33, 10, 27, 2, color);
      }
    }

  } while (display.nextPage());
  
  // Enter deep sleep to conserve power until next update
  goDeepSleep();
}

// Loop function - not used since device enters deep sleep after setup
void loop() {}
