/**
 * @file GetFatTime.c
 * @author Seb Madgwick
 * @brief Overrides the MPLAB Harmony function get_fattime in disk.io.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "ff.h"
#include "Rtc/Rtc.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Returns the current time for use by the file system.
 * @return Current time expressed as a single 32-bit value.  This value is equal
 * to the packedTime member of the SYS_FS_TIME structure.
 */
DWORD get_fattime(void) {

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
