/**
 * @file Sync.c
 * @author Seb Madgwick
 * @brief Synchronisation input.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "Fifo.h"
#include "InputCapture/InputCapture.h"
#include <stdio.h>
#include "Sync.h"
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to print capture errors.
 */
//#define PRINT_CAPTURE_ERRORS

/**
 * @brief Minimum supported period. This is the reciprocal of the maximum
 * supported frequency.
 */
#define MIN_PERIOD (TIMER_TICKS_PER_SECOND / 1100U)

/**
 * @brief Holdoff period to prevent excessive interrupts from spurious high-
 * frequency signals.
 */
#define HOLDOFF_PERIOD (TIMER_TICKS_PER_SECOND / 10U)

//------------------------------------------------------------------------------
// Function declarations

static void CaptureEvent(const uint64_t ticks);
static inline __attribute__((always_inline)) void Holdoff(const uint64_t now);
static inline __attribute__((always_inline)) void CaptureError(const char* const error);

//------------------------------------------------------------------------------
// Variables

static const InputCaptureSettings inputCaptureSettings = {.edge = InputCaptureEdgeEvery, .interrupt = InputCaptureInterruptEvery, .firstEdge = true};
static volatile bool edge = inputCaptureSettings.firstEdge;
static volatile uint64_t holdoffTimeout;
static volatile int captureError; // __sync_lock_test_and_set cannot be used with bool
static uint8_t fifoData[(128 * sizeof (SyncData)) + 1]; // FIFO capacity is 1 less than size
static Fifo fifo = {.data = fifoData, .dataSize = sizeof (fifoData)};
static volatile uint32_t bufferOverflow;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 */
void SyncInitialise(void) {
    SyncDeinitialise();
    SYNC_INPUT_CAPTURE.initialise(&inputCaptureSettings, CaptureEvent);
}

/**
 * @brief Deinitialises the module.
 */
void SyncDeinitialise(void) {
    SYNC_INPUT_CAPTURE.deinitialise();
    edge = inputCaptureSettings.firstEdge;
    holdoffTimeout = 0;
    captureError = 0;
    FifoClear(&fifo);
    bufferOverflow = 0;
}

/**
 * @brief Module tasks. This function should be called repeatedly within the
 * main program loop.
 */
void SyncTasks(void) {
    const bool state = EVIC_INT_Disable();
    const uint64_t timeout = holdoffTimeout;
    EVIC_INT_Restore(state);
    if (timeout == 0) {
        return;
    }
    if (TimerGetTicks64() < timeout) {
        return;
    }
    holdoffTimeout = 0;
    SYNC_INPUT_CAPTURE.initialise(&inputCaptureSettings, CaptureEvent);
}

/**
 * @brief Capture event callback.
 * @param ticks Ticks.
 */
static void CaptureEvent(const uint64_t ticks) {

    // Hardware buffer overrun
    if (SYNC_INPUT_CAPTURE.bufferOverrun()) {
        CaptureError("Hardware buffer overrun");
        Holdoff(ticks);
        return;
    }

    // High frequency
    static uint64_t previousTicks;
    if (edge == inputCaptureSettings.firstEdge) {
        if ((ticks - previousTicks) < MIN_PERIOD) {
            CaptureError("High frequency");
            Holdoff(ticks);
            return;
        }
        previousTicks = ticks;
    }

    // Write to FIFO
    const SyncData fifoPacket = {
        .ticks = ticks,
        .edge = edge,
    };
    if (FifoWrite(&fifo, &fifoPacket, sizeof (fifoPacket)) != FifoResultOk) {
        bufferOverflow++;
    }
    edge ^= true;
}

/**
 * @brief Disables the input capture for the holdoff period.
 * @param now Now.
 */
static inline __attribute__((always_inline)) void Holdoff(const uint64_t now) {
    SYNC_INPUT_CAPTURE.deinitialise();
    edge = inputCaptureSettings.firstEdge;
    holdoffTimeout = now + HOLDOFF_PERIOD;
}

/**
 * @brief Handles a capture error.
 * @param error Error.
 */
static inline __attribute__((always_inline)) void CaptureError(const char* const error) {
    captureError = 1;
#ifdef PRINT_CAPTURE_ERRORS
    printf("%s\n", error);
#endif
}

/**
 * @brief Returns true if a capture error occurred. Calling this function will
 * reset the flag.
 * @return True if a capture error occurred.
 */
bool SyncCaptureError(void) {
    return __sync_lock_test_and_set(&captureError, 0) != 0;
}

/**
 * @brief Gets data.
 * @param data Data.
 * @return Result.
 */
SyncResult SyncGetData(SyncData * const data) {
    if (FifoRead(&fifo, data, sizeof (SyncData)) == 0) {
        return SyncResultError;
    }
    return SyncResultOk;
}

/**
 * @brief Returns the number of edges lost due to buffer overflow. Calling
 * this function will reset the value.
 * @return Number of edges lost due to buffer overflow.
 */
uint32_t SyncBufferOverflow(void) {
    return __sync_lock_test_and_set(&bufferOverflow, 0);
}

//------------------------------------------------------------------------------
// End of file
