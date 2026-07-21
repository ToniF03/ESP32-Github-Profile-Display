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