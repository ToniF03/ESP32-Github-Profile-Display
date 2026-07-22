/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Internationalization string selection for the ESP32 GitHub profile dashboard
 */

#pragma once

#include "../settings/settings.h"
#include "en.h"
#include "de.h"
#include "ru.h"

inline const Strings& getStrings()
{
    switch (Settings::language)
    {
        case Language::German:
            return German;

        case Language::Russian:
            return Russian;

        case Language::English:
        default:
            return English;
    }
}