#include "screen_ink.h"
#include "weather.h"
#include <Arduino.h>

static int g_screen_status = -1;
static int g_calendar_status = -1;

int si_calendar_status() {
    return g_calendar_status;
}

void si_calendar() {
    time_t now = 0;
    time(&now);
    struct tm tmInfo = {0};
    localtime_r(&now, &tmInfo);
    g_calendar_status = (tmInfo.tm_year + 1900 >= 2025) ? 1 : 2;
}

int si_screen_status() {
    return g_screen_status;
}

void si_screen() {
    si_calendar();
    if (g_calendar_status == 2) {
        Serial.println("[screen_ink_stub] calendar/time not ready");
        g_screen_status = 2;
        return;
    }
    g_screen_status = 1;
}

void print_status() {
    Serial.printf("Weather: %d\n", weather_status());
    Serial.printf("Calendar: %d\n", si_calendar_status());
    Serial.printf("Screen: %d\n", si_screen_status());
}
