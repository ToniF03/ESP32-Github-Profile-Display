/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Rendering logic for the ESP32 GitHub profile dashboard and status screens
 */
#include "displayRenderer.h"

DisplayRenderer::DisplayRenderer(DisplayConfig::DisplayType &display)
    : display(display),
      dithering(display)
{
}

void DisplayRenderer::init(const int rotation,
                           const uint16_t textColor)
{
    display.init();
    display.setRotation(rotation);
    display.setTextColor(textColor);
    display.firstPage();
}

void DisplayRenderer::drawDashboard(const GitHubStats &stats,
                                    const GitHubProfile &profile,
                                    const DeviceInformation &deviceInformation)
{
    do
    {
        display.clearScreen();
        drawStatistics(stats);
        drawHeatmap(stats, deviceInformation);
        drawFooter(profile, deviceInformation);
    } while (display.nextPage());
}

void DisplayRenderer::drawConnectionError()
{
    display.setFont(&Roboto_Regular_11pt8b);
    do
    {
        display.clearScreen();
        display.fillScreen(GxEPD_WHITE);
        display.fillRect(302, 142, 196, 196, GxEPD_BLACK);
        dithering.fillGrayRect(430, 142, 68, 98, 7);
        display.drawBitmap(302, 142, wifi_x_196x196, 196, 196, GxEPD_WHITE);

        int16_t tbx, tby;
        uint16_t tbw, tbh;
        String splitString;
        // TODO: Split the WiFi Connection string by the middle of the number of spaces
        display.getTextBounds("WiFi Connection", 0, 0, &tbx, &tby, &tbw, &tbh);
        display.setCursor(400 - (tbw / 2), 368); // 30 px top margin
        display.print("WiFi Connection");

        display.getTextBounds("failed", 0, 368 + tbh + 30, &tbx, &tby, &tbw, &tbh);
        display.setCursor(400 - (tbw / 2), tby);
        display.print("failed");
    } while (display.nextPage());
}

void DisplayRenderer::drawStatistics(const GitHubStats &stats)
{
    int16_t tbx, tby;
    uint16_t tbw, tbh;

    // Print total contributions
    dithering.fillGrayRoundRect(Layout::StatisticsLeft, Layout::StatisticsTop, Layout::cardStrokeWidth, 176, 3, 4);
    display.setFont(&Roboto_Regular_48pt8b);
    display.getTextBounds(String(stats.contributions), 50, 216, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby);
    display.print(stats.contributions);

    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds(getStrings().contributions, tbx, 0.66 * tby + tbh, &tbx, &tby, &tbw, &tbh);
    display.setCursor(55, tby + tbh + 10);
    display.print(getStrings().contributions);

    // Print longest streak
    dithering.fillGrayRoundRect(Layout::LeftCardX, Layout::StatisticsTop, 15, 83, 3, 4);
    display.setFont(&Roboto_Regular_24pt8b);
    display.getTextBounds(String(stats.longestStreak), 380, 55, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    display.setCursor(tbx, tby);
    display.print(stats.longestStreak);

    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds(getStrings().longestStreak, tbx, tby, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby + 1.875 * tbh + 5);
    display.print(getStrings().longestStreak);

    // Print max contributions in a day
    dithering.fillGrayRoundRect(Layout::LeftCardX, 113, 15, 83, 3, 4);
    display.setFont(&Roboto_Regular_24pt8b);
    display.getTextBounds(String(stats.maxContributions), 380, 148, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    display.setCursor(tbx, tby);
    display.print(stats.maxContributions);

    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds(getStrings().mostInDay, tbx, tby, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby + 1.875 * tbh + 5);
    display.print(getStrings().mostInDay);

    // Print current streak
    dithering.fillGrayRoundRect(Layout::RightCardX, Layout::StatisticsTop, 15, 83, 3, 4);
    display.setFont(&Roboto_Regular_24pt8b);
    display.getTextBounds(String(stats.currentStreak), 575, 55, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    display.setCursor(tbx, tby);
    display.print(stats.currentStreak);

    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds(getStrings().currentStreak, tbx, tby, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby + 1.875 * tbh + 5);
    display.print(getStrings().currentStreak);

    // Print average contributions
    dithering.fillGrayRoundRect(Layout::RightCardX, 113, 15, 83, 3, 4);
    display.setFont(&Roboto_Regular_24pt8b);
    display.getTextBounds(String(stats.averageContributions), 575, 148, &tbx, &tby, &tbw, &tbh);
    tby += 1.5 * tbh;
    display.setCursor(tbx, tby);
    display.print(stats.averageContributions);

    display.setFont(&Roboto_Regular_8pt8b);
    display.getTextBounds(getStrings().averagePerDay, tbx, tby, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx, tby + 1.875 * tbh + 5);
    display.print(getStrings().averagePerDay);
}

void DisplayRenderer::drawFooter(const GitHubProfile &profile, const DeviceInformation &deviceInformation)
{
    int16_t tbx, tby;
    uint16_t tbw, tbh;

    // Display GitHub username and full name in footer
    display.setFont(&Roboto_Regular_6pt8b);
    display.fillRect(0, 464, 16, 16, GxEPD_BLACK);
    display.drawBitmap(0, 464, sy_github_16x16, 16, 16, GxEPD_WHITE);
    display.getTextBounds(String(profile.username) + " (" + profile.name + ")", 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(20, DisplayConfig::Width - tbh * 0.25);
    display.print(String(profile.username) + " (" + profile.name + ")");

    // Display current date and time in footer
    display.getTextBounds(deviceInformation.time_string, 0, 0, &tbx, &tby, &tbw, &tbh);
    display.setCursor(795 - tbw, DisplayConfig::Width - tbh * 0.33);
    display.print(deviceInformation.time_string);

    display.fillRect(770 - tbw, 464, 16, 16, GxEPD_BLACK);
    display.drawBitmap(770 - tbw, 464, wi_time_1_16x16, 16, 16, GxEPD_WHITE);

    // Display WiFi signal strength with appropriate icon
    display.getTextBounds(String(deviceInformation.WiFi_Description) + " (" + deviceInformation.WiFi_Strength + " dbm)", 770 - tbw, DisplayConfig::Width, &tbx, &tby, &tbw, &tbh);
    display.setCursor(tbx - 10 - tbw, DisplayConfig::Width - tbh * 0.33);
    display.print(String(deviceInformation.WiFi_Description) + " (" + deviceInformation.WiFi_Strength + " dBm)");
    display.fillRect(tbx - 31 - tbw, 464, 16, 16, GxEPD_BLACK);

    if (deviceInformation.WiFi_Description == getStrings().excellent)
        display.drawBitmap(tbx - 31 - tbw, 464, wifi_16x16, 16, 16, GxEPD_WHITE);
    else if (deviceInformation.WiFi_Description == getStrings().good)
        display.drawBitmap(tbx - 31 - tbw, 464, wifi_3_bar_16x16, 16, 16, GxEPD_WHITE);
    else if (deviceInformation.WiFi_Description == getStrings().fair)
        display.drawBitmap(tbx - 31 - tbw, 464, wifi_2_bar_16x16, 16, 16, GxEPD_WHITE);
    else if (deviceInformation.WiFi_Description == getStrings().weak)
        display.drawBitmap(tbx - 31 - tbw, 464, wifi_1_bar_16x16, 16, 16, GxEPD_WHITE);
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
            dithering.fillGrayRoundRect(Layout::HeatmapX + week * 15, Layout::HeatmapY + day * 33, 10, 27, 2, color);
        }
    }
}