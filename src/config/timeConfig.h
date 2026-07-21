#pragma once

#include <Arduino.h>

namespace TimeConfig
{
    constexpr char Server[] = "pool.ntp.org";

    constexpr long GmtOffset = 3600;
    constexpr long DstOffset = 3600;

    constexpr uint64_t SleepTime = 3600ULL * 1000000ULL;
}