/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Application settings and language configuration for the ESP32 GitHub profile display
 */

#pragma once

enum class Language
{
    English,
    German,
    Russian
};

class Settings
{
public:
    static Language language;
};