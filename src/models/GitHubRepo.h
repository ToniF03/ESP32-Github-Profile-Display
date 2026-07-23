/*
 * Created on: 2026-07-22
 * Author(s): Toni Fey
 * License: MIT
 * Description: Data model for a GitHub repository
 */

#pragma once

#include <Arduino.h>

struct GitHubRepo
{
    String name;
    String description;
    String language;
    String license;
    int stargazers;
    int watchers;
};