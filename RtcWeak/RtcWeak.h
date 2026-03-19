/**
 * @file RtcWeak.h
 * @author Seb Madgwick
 * @brief RTC driver with weak implementations.
 */

#ifndef RTC_WEAK_H
#define RTC_WEAK_H

//------------------------------------------------------------------------------
// Includes

#include <stddef.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Time.
 */
typedef struct {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
} RtcTime;

/**
 * @brief Result.
 */
typedef enum {
    RtcResultOk,
    RtcResultInvalidYear,
    RtcResultInvalidMonth,
    RtcResultInvalidDay,
    RtcResultInvalidHour,
    RtcResultInvalidMinute,
    RtcResultInvalidSecond,
    RtcResultDestinationTooSmall,
    RtcResultInvalidStringFormat,
} RtcResult;

/**
 * @brief String size.
 */
#define RTC_STRING_SIZE (sizeof("YYYY-MM-DD hh:mm:ss"))

//------------------------------------------------------------------------------
// Function declarations

void RtcGetTime(RtcTime * const time);
RtcResult RtcSetTime(const RtcTime * const time);
RtcResult RtcGetTimeAsString(char* const destination, const size_t destinationSize);
RtcResult RtcSetTimeFromString(const char* const string);
RtcResult RtcTimeToString(char* const destination, const size_t destinationSize, const RtcTime * const time);
RtcResult RtcTimeFromString(RtcTime * const time, const char* const string);
RtcResult RtcValidate(const RtcTime * const time);
const char* RtcResultToString(const RtcResult result);

#endif

//------------------------------------------------------------------------------
// End of file
