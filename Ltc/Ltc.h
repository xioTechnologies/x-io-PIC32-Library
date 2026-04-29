/**
 * @file Ltc.h
 * @author Seb Madgwick
 * @brief Linear Timecode (LTC) receiver.
 */

#ifndef LTC_H
#define LTC_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Result.
 */
typedef enum {
    LtcResultOk,
    LtcResultError,
} LtcResult;

/**
 * @brief Timecode size.
 */
#define LTC_TIMECODE_SIZE (sizeof ("hh:mm:ss:ff"))

/**
 * @brief Data.
 */
typedef struct {
    uint64_t ticks;
    char timecode[LTC_TIMECODE_SIZE];
} LtcData;

//------------------------------------------------------------------------------
// Function declarations

void LtcInitialise(void);
void LtcDeinitialise(void);
void LtcTasks(void);
bool LtcDecodeError(void);
bool LtcNoSignal(void);
LtcResult LtcGetData(LtcData * const data);
uint32_t LtcBufferOverflow(void);

#endif

//------------------------------------------------------------------------------
// End of file
