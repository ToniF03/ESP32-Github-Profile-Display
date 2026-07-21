#pragma once

#include "../config/DisplayConfig.h"

class Dithering
{
public:
    explicit Dithering(DisplayConfig::DisplayType &display);

    void drawGrayPixel(int x, int y, uint8_t level);

    void drawGrayRect(int x, int y, int w, int h, uint8_t level);

    void fillGrayRect(int x, int y, int w, int h, uint8_t level);

    void drawGrayCircle(int x, int y, int radius, uint8_t level);

    void fillGrayCircle(int x, int y, int radius, uint8_t level);

    void drawGrayRoundRect(int x, int y, int w, int h, int radius, uint8_t level);

    void fillGrayRoundRect(int x, int y, int w, int h, int radius, uint8_t level);

    void drawGrayLine(int x0, int y0, int x1, int y1, uint8_t level);

private:
    DisplayConfig::DisplayType &display;

    bool isPixelBlack(uint16_t x, uint16_t y, uint8_t level);

    static constexpr uint8_t Bayer4x4[4][4] =
        {
            {0, 8, 2, 10},
            {12, 4, 14, 6},
            {3, 11, 1, 9},
            {15, 7, 13, 5}};
};