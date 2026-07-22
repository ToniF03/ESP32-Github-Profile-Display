/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Data model for a GitHub profile
 */

#pragma once

#include <Arduino.h>

struct GitHubProfile
{
    String username;
    String name;
    int followers;
    int following;
};