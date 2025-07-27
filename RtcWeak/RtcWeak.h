/**
 * @file RtcWeak.h
 * @author Seb Madgwick
 * @brief RTC driver with weak implementations.
 */

#ifndef RTC_WEAK_H
#define RTC_WEAK_H

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

//------------------------------------------------------------------------------
// Function declarations

void RtcGetTime(RtcTime * const time);
void RtcSetTime(const RtcTime * const time);
int RtcTimeFromString(RtcTime * const time, const char* const string);
char* RtcTimeToString(const RtcTime * const time);

#endif

//------------------------------------------------------------------------------
// End of file
