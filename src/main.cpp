#include <Arduino.h>
#include <GxEPD2.h>
#include <GxEPD2_3C.h>
#include <WiFi.h>

GxEPD2_3C<GxEPD2_750c_Z08, GxEPD2_750c_Z08::HEIGHT / 2> display(GxEPD2_750c_Z08(13, 22, 21, 14));

const char *ssid = "PP WeatherStation";
const char *password = "YOUR_STATION_PASSWORD";

void setupWiFi();

void setupWiFi() {
  WiFi.setHostname("PixelPioneer ePaper");
  WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
  float connectionBegin = millis();
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    if (millis() - connectionBegin >= 60000) {
      //failedConnection();
      //goDeepSleep();
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
  delay(10);

  display.init(115200);
  display.setRotation(0);
  display.setTextColor(GxEPD_BLACK);

  display.firstPage();

  delay(100);


}

void loop() {
    do {
    display.fillRect(0, 0, display.width(), display.height(), GxEPD_WHITE);
  } while (display.nextPage());
  Serial.println("Setup WiFi");
  delay(500000000);
}