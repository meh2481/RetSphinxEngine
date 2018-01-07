#pragma once
#include <stdint.h>
#include <string>

class SteelSeriesClient;

class SteelSeriesHaptic
{
private:
    SteelSeriesClient* ssClient;

    void bindEvent(const std::string& eventId, const std::string& eventType, uint32_t rumbleLen);

public:
    SteelSeriesHaptic(SteelSeriesClient* client);
    ~SteelSeriesHaptic();

    void init();
    bool isValid();

    //Strength in range 0-1, duration in msec, curTime current clock time in seconds
    void rumble(float strength, uint32_t duration, float curTime);
};