/**
 * @file GetFatTime.c
 * @author Seb Madgwick
 * @brief Overrides the MPLAB Harmony function get_fattime in diskio.c.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "ff.h"
#include "RtcWeak/RtcWeak.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Returns the current time. See weak implementation in diskio.c for more
 * information.
 * @return Current time.
 */
DWORD get_fattime(void) {

    // Get RTC time
    RtcTime rtcTime;
    RtcGetTime(&rtcTime);

    // Parse time
    const SYS_FS_TIME fsTime = {
        .discreteTime.second = rtcTime.second / 2,
        .discreteTime.minute = rtcTime.minute,
        .discreteTime.hour = rtcTime.hour,
        .discreteTime.day = rtcTime.day,
        .discreteTime.month = rtcTime.month,
        .discreteTime.year = rtcTime.year - 1980,
    };
    return fsTime.packedTime;
}

//------------------------------------------------------------------------------
// End of file
