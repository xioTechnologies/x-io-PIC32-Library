/**
 * @file RtcWeak.c
 * @author Seb Madgwick
 * @brief RTC driver with weak implementations.
 */

//------------------------------------------------------------------------------
// Includes

#include "RtcWeak.h"
#include <stdio.h> // snprintf, sscanf

//------------------------------------------------------------------------------
// Variables

static RtcTime time;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Gets the time.  This function should be overridden by the application
 * RTC driver.
 * @param time Time.
 */
__attribute__((weak)) void RtcGetTime(RtcTime * const time_) {
    *time_ = time;
}

/**
 * @brief Gets the time.  This function should be overridden by the application
 * RTC driver.
 * @param time Time.
 */
__attribute__((weak)) void RtcSetTime(const RtcTime * const time_) {
    time = *time_;
}

/**
 * @brief Creates time from a string in the format "YYYY-MM-DD hh:mm:ss" as per
 * ISO 8601 and RFC 3339 page 8.
 * @param time Time.
 * @param string String.
 * @return 0 if successful.
 */
int RtcTimeFromString(RtcTime * const time, const char* const string) {
    if (sscanf(string, "%u %u %u %u %u %u", &time->year, &time->month, &time->day, &time->hour, &time->minute, &time->second) != 6) {
        return 1;
    }
    return 0;
}

/**
 * @brief Returns the time as a string in the format "YYYY-MM-DD hh:mm:ss" as
 * per ISO 8601 and RFC 3339 page 8.
 * @param time Time.
 * @return Time string.
 */
char* RtcTimeToString(const RtcTime * const time) {
    static char string[32] = "";
    snprintf(string, sizeof (string), "%04u-%02u-%02u %02u:%02u:%02u", time->year, time->month, time->day, time->hour, time->minute, time->second);
    return string;
}

//------------------------------------------------------------------------------
// End of file
