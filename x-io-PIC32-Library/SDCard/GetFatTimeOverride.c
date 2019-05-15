/**
 * @file GetFatTimeOverride.c
 * @author Seb Madgwick
 * @brief Overrides the file system get time function to use a RTC.
 *
 * Line 215 of diskio.c in MPLAB Harmony must be modified to:
 * uint32_t __attribute__((weak)) get_fattime(void)
 */

//------------------------------------------------------------------------------
// Includes

#include "Rtc/Rtc.h"
#include <stdint.h>
#include "system_definitions.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @breif Returns the current time for use by the file system.
 * @return Current time expressed as a single 32-bit value.  This value is equal
 * to the packedTime member of the SYS_FS_TIME structure.
 */
uint32_t get_fattime(void) {

    // Read RTC
    RtcTime rtcTime;
    RtcReadTime(&rtcTime);

    // Parse time
    const SYS_FS_TIME sysFSTime = {
        .discreteTime.second = rtcTime.second / 2,
        .discreteTime.minute = rtcTime.minute,
        .discreteTime.hour = rtcTime.hour,
        .discreteTime.day = rtcTime.day,
        .discreteTime.month = rtcTime.month,
        .discreteTime.year = rtcTime.year - 1980,
    };
    return sysFSTime.packedTime;
}

//------------------------------------------------------------------------------
// End of file
