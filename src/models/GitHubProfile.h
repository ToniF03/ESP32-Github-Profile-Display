#pragma once

#include <Arduino.h>

struct GitHubProfile
{
    String username;
    String name;
    int followers;
    int following;
};