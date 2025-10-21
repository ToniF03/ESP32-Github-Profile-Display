#include <GxEPD2_BW.h>
#include <WiFi.h>
#include <resources/icons/icons.h>
#include <resources/fonts/fonts.h>

#define BSY 4
#define RST 16
#define DC 17
#define CS 5

#define WIFI_SSID "Vodafone-05FD"
#define WIFI_PASSWORD "12803ujioasud091op2k"

/*
 * BSY  ->  4
 * RST  ->  16
 * DC   ->  17
 * CS   ->  5
 * SCK  ->  18
 * SDI  ->  23
 */

GxEPD2_BW<GxEPD2_750_GDEY075T7, GxEPD2_750_GDEY075T7::HEIGHT> display(GxEPD2_750_GDEY075T7(CS, DC, RST, BSY));

const uint8_t bayer4x4[4][4] = {
  {  0,  8,  2, 10 },
  { 12,  4, 14,  6 },
  {  3, 11,  1,  9 },
  { 15,  7, 13,  5 }
};

// --- Helper to determine if pixel should be black ---
inline bool isPixelBlack(uint16_t x, uint16_t y, uint8_t level) {
  if (level == 0) return false;       // totally white
  if (level == 17) return true;       // totally black
  // map level 1-16 to threshold 0-15
  uint8_t threshold = level - 1; 
  return bayer4x4[y % 4][x % 4] < threshold;
}

// --- Draw a pixel with gray ---
void drawGrayPixel(int x, int y, uint8_t level) {
  if (isPixelBlack(x, y, level))
    display.drawPixel(x, y, GxEPD_BLACK);
  else
    display.drawPixel(x, y, GxEPD_WHITE);
}

// --- Draw rectangle (outline) ---
void drawGrayRect(int x, int y, int w, int h, uint8_t level) {
  for (int i = x; i < x + w; i++) {
    drawGrayPixel(i, y, level);
    drawGrayPixel(i, y + h - 1, level);
  }
  for (int j = y; j < y + h; j++) {
    drawGrayPixel(x, j, level);
    drawGrayPixel(x + w - 1, j, level);
  }
}

// --- Fill rectangle ---
void fillGrayRect(int x, int y, int w, int h, uint8_t level) {
  for (int j = y; j < y + h; j++) {
    for (int i = x; i < x + w; i++) {
      drawGrayPixel(i, j, level);
    }
  }
}

// --- Draw line (Bresenham) ---
void drawGrayLine(int x0, int y0, int x1, int y1, uint8_t level) {
  int dx = abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
  int dy = -abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
  int err = dx + dy, e2;
  while (true) {
    drawGrayPixel(x0, y0, level);
    if (x0 == x1 && y0 == y1) break;
    e2 = 2 * err;
    if (e2 >= dy) { err += dy; x0 += sx; }
    if (e2 <= dx) { err += dx; y0 += sy; }
  }
}

// --- Draw circle (outline) ---
void drawGrayCircle(int xc, int yc, int r, uint8_t level) {
  int x = 0, y = r;
  int d = 3 - 2 * r;
  while (y >= x) {
    drawGrayPixel(xc + x, yc + y, level);
    drawGrayPixel(xc - x, yc + y, level);
    drawGrayPixel(xc + x, yc - y, level);
    drawGrayPixel(xc - x, yc - y, level);
    drawGrayPixel(xc + y, yc + x, level);
    drawGrayPixel(xc - y, yc + x, level);
    drawGrayPixel(xc + y, yc - x, level);
    drawGrayPixel(xc - y, yc - x, level);
    x++;
    if (d > 0) { y--; d = d + 4 * (x - y) + 10; }
    else { d = d + 4 * x + 6; }
  }
}

// --- Fill circle ---
void fillGrayCircle(int xc, int yc, int r, uint8_t level) {
  int x = 0, y = r;
  int d = 3 - 2 * r;
  while (y >= x) {
    for (int i = xc - x; i <= xc + x; i++) {
      drawGrayPixel(i, yc + y, level);
      drawGrayPixel(i, yc - y, level);
    }
    for (int i = xc - y; i <= xc + y; i++) {
      drawGrayPixel(i, yc + x, level);
      drawGrayPixel(i, yc - x, level);
    }
    x++;
    if (d > 0) { y--; d = d + 4 * (x - y) + 10; }
    else { d = d + 4 * x + 6; }
  }
}

// --- Draw rounded rectangle ---
void drawGrayRoundRect(int x, int y, int w, int h, int radius, uint8_t level) {
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
void fillGrayRoundRect(int x, int y, int w, int h, int radius, uint8_t level) {
  fillGrayRect(x + radius, y, w - 2 * radius, h, level);
  fillGrayRect(x, y + radius, w, h - 2 * radius, level);
  fillGrayCircle(x + radius, y + radius, radius, level);
  fillGrayCircle(x + w - radius - 1, y + radius, radius, level);
  fillGrayCircle(x + radius, y + h - radius - 1, radius, level);
  fillGrayCircle(x + w - radius - 1, y + h - radius - 1, radius, level);
}

void goDeepSleep() {
  esp_sleep_enable_timer_wakeup(1800000000);
  //esp_sleep_enable_timer_wakeup(120000000);
  Serial.println("ESP goes to deep sleep now");
  Serial.flush();
  esp_deep_sleep_start();
}

void failedConnection() {
  display.setFont(&Roboto_Regular_11pt8b);
  do {
    display.fillScreen(GxEPD_WHITE);
    display.fillRect(302, 142, 196, 196, GxEPD_BLACK);
    display.drawBitmap(302, 142, wifi_x_196x196, 196, 196, GxEPD_WHITE);

    int16_t tbx, tby;
    uint16_t tbw, tbh;
    display.getTextBounds("WiFi Connection", 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(400 - (tbw / 2), 368);  // 30 px top margin
    display.print("WiFi Connection");
    display.getTextBounds("failed", 0, 368 + tbh + 30, &tbx, &tby, &tbw, &tbh);
    display.setCursor(400 - (tbw / 2), tby);
    display.print("failed");
  } while (display.nextPage());
  goDeepSleep();
}

void initWiFi() {
  WiFi.setHostname("PixelPioneer ePaper");

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  float connectionBegin = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(50);
    if (millis() - connectionBegin >= 60000) {
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

void setup() {
  Serial.begin(115200);
  display.init();
  display.setRotation(0);

  display.firstPage();

failedConnection();

  do {
    display.clearScreen();
  } while (display.nextPage());
}

void loop() {}
