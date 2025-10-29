#include <ArduinoJson.h>
#include <GxEPD2_BW.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <time.h>

#include <resources/icons/icons.h>
#include <resources/fonts/fonts.h>
#include <resources/credentials.h>

#define BSY 4
#define RST 16
#define DC 17
#define CS 5

/*
 * Screen Connection
 * BSY  ->  4
 * RST  ->  16
 * DC   ->  17
 * CS   ->  5
 * SCK  ->  18
 * SDI  ->  23
 */

GxEPD2_BW<GxEPD2_750_GDEY075T7, GxEPD2_750_GDEY075T7::HEIGHT> display(GxEPD2_750_GDEY075T7(CS, DC, RST, BSY));

const uint8_t bayer4x4[4][4] = {
    {0, 8, 2, 10},
    {12, 4, 14, 6},
    {3, 11, 1, 9},
    {15, 7, 13, 5}};

// --- Helper to determine if pixel should be black ---
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

// --- Draw a pixel with gray ---
void drawGrayPixel(int x, int y, uint8_t level)
{
  if (isPixelBlack(x, y, level))
    display.drawPixel(x, y, GxEPD_BLACK);
  else
    display.drawPixel(x, y, GxEPD_WHITE);
}

// --- Draw rectangle (outline) ---
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

// --- Fill rectangle ---
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

// --- Draw line (Bresenham) ---
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

// --- Draw circle (outline) ---
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

// --- Fill circle ---
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

// --- Draw rounded rectangle ---
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

// --- Fill rounded rectangle ---
void fillGrayRoundRect(int x, int y, int w, int h, int radius, uint8_t level)
{
  fillGrayRect(x + radius, y, w - 2 * radius, h, level);
  fillGrayRect(x, y + radius, w, h - 2 * radius, level);
  fillGrayCircle(x + radius, y + radius, radius, level);
  fillGrayCircle(x + w - radius - 1, y + radius, radius, level);
  fillGrayCircle(x + radius, y + h - radius - 1, radius, level);
  fillGrayCircle(x + w - radius - 1, y + h - radius - 1, radius, level);
}

void goDeepSleep()
{
  display.hibernate();
  esp_sleep_enable_timer_wakeup(1800000000);
  // esp_sleep_enable_timer_wakeup(120000000);
  Serial.println("ESP goes to deep sleep now");
  Serial.flush();
  esp_deep_sleep_start();
}

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
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.println();
  Serial.println("--------------------------------");
  delay(10);
}

struct tm timeinfo;

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


int weekday = 0;

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

    String graphQLQuery = String("{\"query\":\"query { user(login: \\\"") + username + "\\\") { contributionsCollection(from: \\\"" + timeStr2 + "T00:00:00Z\\\", to: \\\"" + timeStr + "T23:59:59Z\\\") { contributionCalendar { totalContributions weeks { contributionDays { date contributionCount color } } } } } }\"}";

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

int hexToGray(const String &hexColor)
{
  // Expect format "#RRGGBB"
  if (hexColor.length() != 7 || hexColor[0] != '#')
    return 0;

  // Parse hex values
  int r = strtol(hexColor.substring(1, 3).c_str(), nullptr, 16);
  int g = strtol(hexColor.substring(3, 5).c_str(), nullptr, 16);
  int b = strtol(hexColor.substring(5, 7).c_str(), nullptr, 16);

  // Apply luminance formula
  int gray = int(0.299 * r + 0.587 * g + 0.114 * b);

  return gray; // 0–255 brightness
}

int contributions = 0;
String GITHUB_NAME;
int followers = 0;
int following = 0;

void setup()
{
  Serial.begin(115200);
  display.init();
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);
  display.firstPage();
  initWiFi();

  // Initialize SNTP using Arduino-style API (configTime) to avoid ESP-IDF-only symbols
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
    Serial.println(" Failed to obtain time");
    failedConnection();
  }
  Serial.println();

  // Print current time
  Serial.println(&timeinfo, "Current time: %A, %B %d %Y %H:%M:%S");
  char timeStr[64];
  sprintf(timeStr, "%02d/%02d/%04d %02d:%02d:%02d",
          timeinfo.tm_mday,
          timeinfo.tm_mon + 1,
          timeinfo.tm_year + 1900,
          timeinfo.tm_hour,
          timeinfo.tm_min,
          timeinfo.tm_sec);
  weekday = timeinfo.tm_wday;

  // Retrieve GitHub follower data
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

  uint8_t commits[372];
  error = deserializeJson(doc, receiveChartInformation(GITHUB_USERNAME));
  if (error)
  {
    Serial.print("Error occured while fetching forecast: ");
    Serial.println(error.c_str());
  }

  // TODO display stats like longest streak, max contributions in a day, etc.
  int longestStreak = 0;
  int streak = 0;
  int maxContributions = 0;
  int daysWithContributions = 0;


  contributions = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["totalContributions"].as<int>();
  for (int i = 0; i < 371; i++)
  {
    if (doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][i / 7]["contributionDays"][i % 7]["contributionCount"].as<int>() == 0)
      commits[i] = 209;
    else
      commits[i] = hexToGray(doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][i / 7]["contributionDays"][i % 7]["color"].as<String>());

    // Calculate longest streak
    if (doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][i / 7]["contributionDays"][i % 7]["contributionCount"].as<int>() > 0)
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
    if (doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][i / 7]["contributionDays"][i % 7]["contributionCount"].as<int>() > maxContributions)
    {
      maxContributions = doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][i / 7]["contributionDays"][i % 7]["contributionCount"].as<int>();
    }

    // Calculate days with contributions
    if (doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][i / 7]["contributionDays"][i % 7]["contributionCount"].as<int>() > 0)
    {
      daysWithContributions++;
    }
  }

  int currentStreak = 0;
  for (int i = 370 - (7 - weekday); i >= 0; i--)
  {
    if (doc["data"]["user"]["contributionsCollection"]["contributionCalendar"]["weeks"][i / 7]["contributionDays"][i % 7]["contributionCount"].as<int>() > 0)
    {
      currentStreak++;
    }
    else
    {
      break;
    }
  }


  do
  {
    display.clearScreen();

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    
    // Draw GitHub icon and username
    display.setFont(&Roboto_Regular_6pt8b);
    display.fillRect(0, 464, 16, 16, GxEPD_BLACK);
    display.drawBitmap(0, 464, sy_github_16x16, 16, 16, GxEPD_WHITE);
    display.getTextBounds(String(GITHUB_USERNAME) + " (" + GITHUB_NAME + ")", 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(20, 480 - tbh * 0.25);
    display.print(String(GITHUB_USERNAME) + " (" + GITHUB_NAME + ")");

    // Draw time icon and time
    display.getTextBounds(timeStr, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(795 - tbw, 480 - tbh * 0.33);
    display.print(timeStr);
    display.fillRect(770 - tbw, 464, 16, 16, GxEPD_BLACK);
    display.drawBitmap(770 - tbw, 464, wi_time_1_16x16, 16, 16, GxEPD_WHITE);

    // Print total contributions
    fillGrayRoundRect(20, 20, 15, 176, 3, 3);
    display.setFont(&Roboto_Regular_48pt8b);
    display.getTextBounds(String(contributions), 50, 216, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby);
    display.print(contributions);

    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds("Contributions in the last year", tbx, 0.75 * tby + tbh, &tbx, &tby, &tbw, &tbh);
    display.setCursor(55, tby + tbh + 10);
    display.print("Contributions in the last year");

    // Print longest streak
    fillGrayRoundRect(350, 20, 15, 61, 3, 3);
    display.setFont(&Roboto_Regular_20pt8b);
    display.getTextBounds(String(longestStreak), 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(380, 35 + 0.6 * tbh);
    display.print(longestStreak);
    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds("Longest Streak", 0, 35 + 0.6 * tbh, &tbx, &tby, &tbw, &tbh);
    display.setCursor(380, tby + 1.75 * tbh + 5);
    display.print("Longest Streak");

    // Print max contributions in a day
    fillGrayRoundRect(350, 92, 15, 61, 3, 3);
    display.setFont(&Roboto_Regular_20pt8b);
    display.getTextBounds(String(maxContributions), 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(380, 107 + 0.6 * tbh);
    display.print(maxContributions);
    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds("Most in a Day", 0, 107 + 0.6 * tbh, &tbx, &tby, &tbw, &tbh);
    display.setCursor(380, tby + 1.75*tbh + 5);
    display.print("Most in a Day");

    // Draw contribution graph
    for (int week = 0; week < 53; week++)
    {
      for (int day = 0; day < 7; day++)
      {
        if (week == 52 && day > weekday)
          break;
        int index = week * 7 + day;
        if (index >= 371)
          break;
        int color = map(commits[index], 0, 236, 17, 0);
        if (color == 1)
          color = 0;
        else
          color = map(color, 1, 17, 3, 17);
        fillGrayRoundRect(5 + week * 15, 220 + day * 33, 10, 27, 2, color);
      }
    }

  } while (display.nextPage());
  goDeepSleep();
}

void loop() {}
