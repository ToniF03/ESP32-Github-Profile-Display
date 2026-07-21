#include <Arduino.h>
#include "dithering.h"
#include "../config/displayConfig.h"

Dithering::Dithering(DisplayConfig::DisplayType &display)
    : display(display)
{
}

/**
 * Determine if a pixel should be black using Bayer dithering
 * @param x X coordinate of the pixel
 * @param y Y coordinate of the pixel
 * @param level Grayscale level (0=white, 17=black, 1-16=dithered)
 * @return true if pixel should be black, false if white
 */
inline bool Dithering::isPixelBlack(uint16_t x, uint16_t y, uint8_t level)
{
    if (level == 0)
        return false; // totally white
    if (level == 17)
        return true; // totally black
    // map level 1-16 to threshold 0-15
    uint8_t threshold = level - 1;
    return Dithering::Bayer4x4[y % 4][x % 4] < threshold;
}

/**
 * Draw a single pixel with dithered grayscale
 * @param x X coordinate
 * @param y Y coordinate
 * @param level Grayscale level (0-17)
 */
void Dithering::drawGrayPixel(int x, int y, uint8_t level)
{
    if (isPixelBlack(x, y, level))
        display.drawPixel(x, y, 0x0000);
    else
        display.drawPixel(x, y, 0xFFFF);
}

/**
 * Draw a rectangle outline with dithered grayscale
 * @param x Top-left X coordinate
 * @param y Top-left Y coordinate
 * @param w Width
 * @param h Height
 * @param level Grayscale level (0-17)
 */
void Dithering::drawGrayRect(int x, int y, int w, int h, uint8_t level)
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
void Dithering::fillGrayRect(int x, int y, int w, int h, uint8_t level)
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
void Dithering::drawGrayLine(int x0, int y0, int x1, int y1, uint8_t level)
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
void Dithering::drawGrayCircle(int xc, int yc, int r, uint8_t level)
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
void Dithering::fillGrayCircle(int xc, int yc, int r, uint8_t level)
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
void Dithering::drawGrayRoundRect(int x, int y, int w, int h, int radius, uint8_t level)
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
void Dithering::fillGrayRoundRect(int x, int y, int w, int h, int radius, uint8_t level)
{
    fillGrayRect(x + radius, y, w - 2 * radius, h, level);
    fillGrayRect(x, y + radius, w, h - 2 * radius, level);
    fillGrayCircle(x + radius, y + radius, radius, level);
    fillGrayCircle(x + w - radius - 1, y + radius, radius, level);
    fillGrayCircle(x + radius, y + h - radius - 1, radius, level);
    fillGrayCircle(x + w - radius - 1, y + h - radius - 1, radius, level);
}