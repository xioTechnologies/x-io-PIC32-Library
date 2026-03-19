/**
 * @file RtcWeak.c
 * @author Seb Madgwick
 * @brief RTC driver with weak implementations.
 *
 * String representations use the format "YYYY-MM-DD hh:mm:ss", based on
 * ISO 8601 with a space separator as permitted by RFC 3339 (page 8).
 */

//------------------------------------------------------------------------------
// Includes

#include <ctype.h>
#include "RtcWeak.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
// Variables

static RtcTime currentTime = {
    .year = 2000,
    .month = 1,
    .day = 1,
    .hour = 0,
    .minute = 0,
    .second = 0,
};

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Gets the time. This function should be overridden by the RTC driver.
 * @param time Time.
 */
__attribute__((weak)) void RtcGetTime(RtcTime * const time) {
    *time = currentTime;
}

/**
 * @brief Sets the time. This function should be overridden by the RTC driver.
 * @param time Time.
 * @return Result.
 */
__attribute__((weak)) RtcResult RtcSetTime(const RtcTime * const time) {
    const RtcResult result = RtcValidate(time);
    if (result != RtcResultOk) {
        return result;
    }
    currentTime = *time;
    return RtcResultOk;
}

/**
 * @brief Gets the time as a string.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @return Result.
 */
RtcResult RtcGetTimeAsString(char* const destination, const size_t destinationSize) {
    RtcTime time;
    RtcGetTime(&time);
    return RtcTimeToString(&time, destination, destinationSize);
}

/**
 * @brief Sets the time from a string.
 * @param string String.
 * @return Result.
 */
RtcResult RtcSetTimeFromString(const char* const string) {
    RtcTime time;
    const RtcResult result = RtcTimeFromString(&time, string);
    if (result != RtcResultOk) {
        return result;
    }
    return RtcSetTime(&time);
}

/**
 * @brief Creates a string representation of the time. This function
 * intentionally does not validate the time so that invalid times can be printed
 * for debugging purposes.
 * @param time Time.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @return Result.
 */
RtcResult RtcTimeToString(const RtcTime * const time, char* const destination, const size_t destinationSize) {
    if (destinationSize < RTC_STRING_SIZE) {
        return RtcResultDestinationTooSmall;
    }
    snprintf(destination, destinationSize, "%04d-%02d-%02d %02d:%02d:%02d",
            time->year,
            time->month,
            time->day,
            time->hour,
            time->minute,
            time->second);
    return RtcResultOk;
}

/**
 * @brief Creates time from a string. The string is parsed by first replacing
 * all non-digit characters with space characters and then taking the first
 * six numbers as the time. This allows the string format to use any characters
 * as separators, and to include arbitrary trailing sequences such as a time
 * zone offset or milliseconds.
 * @param time Time.
 * @param string String.
 * @return Result.
 */
RtcResult RtcTimeFromString(RtcTime * const time, const char* const string) {

    // Create string of only digits and spaces
    char digits[RTC_STRING_SIZE];
    size_t digitsIndex = 0;
    size_t stringIndex = 0;
    while (true) {
        if (digitsIndex >= (sizeof (digits) - 1)) {
            digits[digitsIndex] = '\0';
            break;
        }
        if (string[stringIndex] == '\0') {
            digits[digitsIndex] = '\0';
            break;
        }
        if (isdigit((unsigned char) string[stringIndex]) != 0) {
            digits[digitsIndex++] = string[stringIndex++];
            continue;
        }
        if (digitsIndex == 0) {
            stringIndex++;
            continue;
        }
        if (digits[digitsIndex - 1] == ' ') {
            stringIndex++;
            continue;
        }
        digits[digitsIndex++] = ' ';
        stringIndex++;
    }

    // Parse values
    if (sscanf(digits, "%d %d %d %d %d %d",
        &time->year,
        &time->month,
        &time->day,
        &time->hour,
        &time->minute,
        &time->second) != 6) {
        return RtcResultInvalidStringFormat;
    }
    return RtcValidate(time);
}

/**
 * @brief Validates the time.
 * @param time Time.
 * @return Result.
 */
RtcResult RtcValidate(const RtcTime * const time) {
    if ((time->year < 0) || (time->year > 9999)) {
        return RtcResultInvalidYear;
    }
    if ((time->month < 1) || (time->month > 12)) {
        return RtcResultInvalidMonth;
    }
    if ((time->day < 1) || (time->day > 31)) {
        return RtcResultInvalidDay;
    }
    if ((time->hour < 0) || (time->hour > 23)) {
        return RtcResultInvalidHour;
    }
    if ((time->minute < 0) || (time->minute > 59)) {
        return RtcResultInvalidMinute;
    }
    if ((time->second < 0) || (time->second > 59)) {
        return RtcResultInvalidSecond;
    }
    return RtcResultOk;
}

/**
 * @brief Returns the result message.
 * @param result Result.
 * @return Result message.
 */
const char* RtcResultToString(const RtcResult result) {
    switch (result) {
        case RtcResultOk:
            return "Ok";
        case RtcResultInvalidYear:
            return "Year must be between 0 and 9999";
        case RtcResultInvalidMonth:
            return "Month must be between 1 and 12";
        case RtcResultInvalidDay:
            return "Day must be between 1 and 31";
        case RtcResultInvalidHour:
            return "Hour must be between 0 and 23";
        case RtcResultInvalidMinute:
            return "Minute must be between 0 and 59";
        case RtcResultInvalidSecond:
            return "Second must be between 0 and 59";
        case RtcResultDestinationTooSmall:
            return "Destination too small";
        case RtcResultInvalidStringFormat:
            return "String format must be YYYY-MM-DD hh:mm:ss";
    }
    return ""; // avoid compiler warning
}

//------------------------------------------------------------------------------
// End of file
