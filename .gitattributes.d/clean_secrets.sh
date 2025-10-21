#!/bin/sh
sed -E 's/(#define WIFI_SSID )".*"/\1"YOUR_WIFI_SSID"/' | \
sed -E 's/(#define WIFI_PASSWORD )".*"/\1"YOUR_WIFI_PASSWORD"/'