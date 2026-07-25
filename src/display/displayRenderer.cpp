/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Rendering logic for the ESP32 GitHub profile dashboard and status screens
 */
#include "displayRenderer.h"

DisplayRenderer::DisplayRenderer()
    : _display(DisplayConfig::DisplayModel(Pins::BSY, Pins::BSY, Pins::BSY, Pins::BSY)),
      _dithering(_display)
{
}

void DisplayRenderer::init(const int rotation,
                           const uint16_t textColor)
{
    _display.init();
    _display.setRotation(rotation);
    _display.setTextColor(textColor);
    _display.firstPage();
}

void DisplayRenderer::drawDashboard(const GitHubStats &stats,
                                    const GitHubProfile &profile,
                                    const DeviceInformation &deviceInformation)
{
    do
    {
        _display.clearScreen();
        drawStatistics(stats);
        drawHeatmap(stats, deviceInformation);
        drawFooter(profile, deviceInformation);
    } while (_display.nextPage());
}

void DisplayRenderer::drawConnectionError()
{
    _display.setFont(&Roboto_Regular_11pt8b);
    do
    {
        _display.clearScreen();
        _display.fillScreen(GxEPD_WHITE);
        _display.fillRect(302, 142, 196, 196, GxEPD_BLACK);
        _dithering.fillGrayRect(430, 142, 68, 98, 7);
        _display.drawBitmap(302, 142, wifi_x_196x196, 196, 196, GxEPD_WHITE);

        int16_t tbx, tby;
        uint16_t tbw, tbh;
        String splitString;
        // TODO: Split the WiFi Connection string by the middle of the number of spaces
        _display.getTextBounds("WiFi Connection", 0, 0, &tbx, &tby, &tbw, &tbh);
        _display.setCursor(400 - (tbw / 2), 368); // 30 px top margin
        _display.print("WiFi Connection");

        _display.getTextBounds("failed", 0, 368 + tbh + 30, &tbx, &tby, &tbw, &tbh);
        _display.setCursor(400 - (tbw / 2), tby);
        _display.print("failed");
    } while (_display.nextPage());
}

void DisplayRenderer::drawStatistics(const GitHubStats &stats)
{
    int16_t tbx, tby;
    uint16_t tbw, tbh;

    // Print total contributions
    _dithering.fillGrayRoundRect(Layout::StatisticsLeft, Layout::StatisticsTop, Layout::cardStrokeWidth, 176, 3, 4);
    _display.setFont(&Roboto_Regular_48pt8b);
    _display.getTextBounds(String(stats.contributions), 50, 216, &tbx, &tby, &tbw, &tbh);
    _display.setCursor(tbx, tby);
    _display.print(stats.contributions);

    _display.setFont(&Roboto_Regular_8pt8b);
    _display.getTextBounds(getStrings().contributions, tbx, 0.66 * tby + tbh, &tbx, &tby, &tbw, &tbh);
    _display.setCursor(55, tby + tbh + 10);
    _display.print(getStrings().contributions);

    // Print longest streak
    _dithering.fillGrayRoundRect(Layout::LeftCardX, Layout::StatisticsTop, 15, 83, 3, 4);
    _display.setFont(&Roboto_Regular_24pt8b);
    _display.getTextBounds(String(stats.longestStreak), 380, 55, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    _display.setCursor(tbx, tby);
    _display.print(stats.longestStreak);

    _display.setFont(&Roboto_Regular_8pt8b);
    _display.getTextBounds(getStrings().longestStreak, tbx, tby, &tbx, &tby, &tbw, &tbh);
    _display.setCursor(tbx, tby + 1.875 * tbh + 5);
    _display.print(getStrings().longestStreak);

    // Print max contributions in a day
    _dithering.fillGrayRoundRect(Layout::LeftCardX, 113, 15, 83, 3, 4);
    _display.setFont(&Roboto_Regular_24pt8b);
    _display.getTextBounds(String(stats.maxContributions), 380, 148, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    _display.setCursor(tbx, tby);
    _display.print(stats.maxContributions);

    _display.setFont(&Roboto_Regular_8pt8b);
    _display.getTextBounds(getStrings().mostInDay, tbx, tby, &tbx, &tby, &tbw, &tbh);
    _display.setCursor(tbx, tby + 1.875 * tbh + 5);
    _display.print(getStrings().mostInDay);

    // Print current streak
    _dithering.fillGrayRoundRect(Layout::RightCardX, Layout::StatisticsTop, 15, 83, 3, 4);
    _display.setFont(&Roboto_Regular_24pt8b);
    _display.getTextBounds(String(stats.currentStreak), 575, 55, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    _display.setCursor(tbx, tby);
    _display.print(stats.currentStreak);

    _display.setFont(&Roboto_Regular_8pt8b);
    _display.getTextBounds(getStrings().currentStreak, tbx, tby, &tbx, &tby, &tbw, &tbh);
    _display.setCursor(tbx, tby + 1.875 * tbh + 5);
    _display.print(getStrings().currentStreak);

    // Print average contributions
    _dithering.fillGrayRoundRect(Layout::RightCardX, 113, 15, 83, 3, 4);
    _display.setFont(&Roboto_Regular_24pt8b);
    _display.getTextBounds(String(stats.averageContributions), 575, 148, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    _display.setCursor(tbx, tby);
    _display.print(stats.averageContributions);

    _display.setFont(&Roboto_Regular_8pt8b);
    _display.getTextBounds(getStrings().averagePerDay, tbx, tby, &tbx, &tby, &tbw, &tbh);
    _display.setCursor(tbx, tby + 1.875 * tbh + 5);
    _display.print(getStrings().averagePerDay);
}

void DisplayRenderer::drawFooter(const GitHubProfile &profile, const DeviceInformation &deviceInformation)
{
    int16_t tbx, tby;
    uint16_t tbw, tbh;

    // Display GitHub username and full name in footer
    _display.setFont(&Roboto_Regular_6pt8b);
    _display.fillRect(0, 464, 16, 16, GxEPD_BLACK);
    _display.drawBitmap(0, 464, sy_github_16x16, 16, 16, GxEPD_WHITE);
    _display.getTextBounds(String(profile.username) + " (" + profile.name + ")", 0, 0, &tbx, &tby, &tbw, &tbh);
    _display.setCursor(20, DisplayConfig::Width - tbh * 0.25);
    _display.print(String(profile.username) + " (" + profile.name + ")");

    // Display current date and time in footer
    _display.getTextBounds(deviceInformation.time_string, 0, 0, &tbx, &tby, &tbw, &tbh);
    _display.setCursor(795 - tbw, DisplayConfig::Width - tbh * 0.33);
    _display.print(deviceInformation.time_string);

    _display.fillRect(770 - tbw, 464, 16, 16, GxEPD_BLACK);
    _display.drawBitmap(770 - tbw, 464, wi_time_1_16x16, 16, 16, GxEPD_WHITE);

    // Display WiFi signal strength with appropriate icon
    _display.getTextBounds(String(deviceInformation.WiFi_Description) + " (" + deviceInformation.WiFi_Strength + " dbm)", 770 - tbw, DisplayConfig::Width, &tbx, &tby, &tbw, &tbh);
    _display.setCursor(tbx - 10 - tbw, DisplayConfig::Width - tbh * 0.33);
    _display.print(String(deviceInformation.WiFi_Description) + " (" + deviceInformation.WiFi_Strength + " dBm)");
    _display.fillRect(tbx - 31 - tbw, 464, 16, 16, GxEPD_BLACK);

    if (deviceInformation.WiFi_Description == getStrings().excellent)
        _display.drawBitmap(tbx - 31 - tbw, 464, wifi_16x16, 16, 16, GxEPD_WHITE);
    else if (deviceInformation.WiFi_Description == getStrings().good)
        _display.drawBitmap(tbx - 31 - tbw, 464, wifi_3_bar_16x16, 16, 16, GxEPD_WHITE);
    else if (deviceInformation.WiFi_Description == getStrings().fair)
        _display.drawBitmap(tbx - 31 - tbw, 464, wifi_2_bar_16x16, 16, 16, GxEPD_WHITE);
    else if (deviceInformation.WiFi_Description == getStrings().weak)
        _display.drawBitmap(tbx - 31 - tbw, 464, wifi_1_bar_16x16, 16, 16, GxEPD_WHITE);
}

void DisplayRenderer::drawHeatmap(const GitHubStats &stats, const DeviceInformation &deviceInformation)
{
    // Render contribution heatmap (53 weeks x 7 days)
    for (int week = 0; week < 53; week++)
    {
        for (int day = 0; day < 7; day++)
        {
            if (week == 52 && day > deviceInformation.weekday)
                break;
            int index = week * 7 + day;
            if (index >= 371)
                break;
            // Map contribution count to grayscale level (3=light, 16=dark)
            int color = map(stats.commits[index], 0, stats.maxContributions, 3, 16);
            _dithering.fillGrayRoundRect(Layout::HeatmapX + week * 15, Layout::HeatmapY + day * 33, 10, 27, 2, color);
        }
    }
}

inline void DisplayRenderer::hibernate() {
    _display.hibernate();
}