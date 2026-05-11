/**
 * @file Ltc.c
 * @author Seb Madgwick
 * @brief Linear Timecode (LTC) receiver.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "Fifo.h"
#include "InputCapture/InputCapture.h"
#include "Ltc.h"
#include <stdio.h>
#include <string.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to print decode errors.
 */
//#define PRINT_DECODE_ERRORS

/**
 * @brief Minimum valid pulse width. The minimum expected pulse width is half
 * the bit period at 30 FPS, 208.3 us.
 */
#define MIN_PULSE_WIDTH (100U * TIMER_TICKS_PER_MICROSECOND)

/**
 * @brief Maximum valid pulse width. The maximum expected pulse width is the
 * bit period at 24 FPS, 520.8 us.
 */
#define MAX_PULSE_WIDTH (600U * TIMER_TICKS_PER_MICROSECOND)

/**
 * @brief Threshold to determine if a pulse width is half a bit period or a
 * whole bit period. This value is applicable to all frame rates. The longest
 * half bit period is 260.4 us at 24 FPS, and the shortest whole bit period is
 * 416.7 us at 30 FPS.
 */
#define PULSE_WIDTH_THRESHOLD (350U * TIMER_TICKS_PER_MICROSECOND)

/**
 * @brief Holdoff period to prevent excessive interrupts from spurious high-
 * frequency signals. One frame is approximately 35 ms.
 */
#define HOLDOFF_PERIOD (35U * TIMER_TICKS_PER_MILLISECOND)

/**
 * @brief Timeout before no-signal is asserted.
 */
#define NO_SIGNAL_TIMEOUT (TIMER_TICKS_PER_SECOND / 2U)

/**
 * @brief Frame data.
 */
typedef struct {
    unsigned frameNumberUnits : 4; // must be 0-9
    unsigned userBitsField1 : 4;
    unsigned frameNumberTens : 2; // must be 0-2
    unsigned dropFrameFlag : 1;
    unsigned colorFrameFlag : 1;
    unsigned userBitsField2 : 4;
    unsigned secondsUnits : 4; // must be 0-9
    unsigned userBitsField3 : 4;
    unsigned secondsTens : 3; // must be 0-5
    unsigned : 1;
    unsigned userBitsField4 : 4;
    unsigned minutesUnits : 4; // must be 0-9
    unsigned userBitsField5 : 4;
    unsigned minutesTens : 3; // must be 0-5
    unsigned : 1;
    unsigned userBitsField6 : 4;
    unsigned hoursUnits : 4; // must be 0-9
    unsigned userBitsField7 : 4;
    unsigned hoursTens : 2; // must be 0-2
    unsigned bgf1 : 1;
    unsigned : 1;
    unsigned userBitsField8 : 4;
} __attribute__((__packed__)) FrameData;

/**
 * @brief Frame.
 */
typedef struct {

    union {
        FrameData data;
        uint64_t dataWord;
    } __attribute__((__packed__));
    uint16_t syncWord; // must be 0xBFFC
} __attribute__((__packed__)) Frame;

/**
 * @brief Receive result.
 */
typedef enum {
    ReceiveResultOk,
    ReceiveResultInProgress,
    ReceiveResultLowBitRate,
    ReceiveResultHighBitRate,
    ReceiveResultFrameOverrun,
    ReceiveResultInvalidFrame,
} ReceiveResult;

/**
 * @brief FIFO packet.
 */
typedef struct {
    uint64_t ticks;
    FrameData data;
} __attribute__((__packed__)) FifoPacket;

//------------------------------------------------------------------------------
// Function declarations

static void CaptureEvent(const uint64_t ticks);
static inline __attribute__((always_inline)) void Holdoff(const uint64_t now);
static inline __attribute__((always_inline)) ReceiveResult Receive(const uint64_t pulseWidth);
static inline __attribute__((always_inline)) bool DetectSyncWord(const bool bit);
static inline __attribute__((always_inline)) bool WriteFrameBuffer(const bool bit);
static inline __attribute__((always_inline)) void ClearFrameBuffer(void);
static inline __attribute__((always_inline)) void DecodeError(const char* const error);

//------------------------------------------------------------------------------
// Variables

static const InputCaptureSettings inputCaptureSettings = {.edge = InputCaptureEdgeEvery, .interrupt = InputCaptureInterruptFourth, .firstEdge = false};
static Frame frameBuffer;
static int frameBufferIndex;
static bool secondHalfBitPeriodPending;
static volatile uint64_t holdoffTimeout;
static volatile int decodeError; // __sync_lock_test_and_set cannot be used with bool
static volatile uint64_t noSignalTimeout;
static uint8_t fifoData[(128 * sizeof (FifoPacket)) + 1]; // FIFO capacity is 1 less than size
static Fifo fifo = {.data = fifoData, .dataSize = sizeof (fifoData)};
static volatile uint32_t bufferOverflow;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 */
void LtcInitialise(void) {
    LtcDeinitialise();
    LTC_INPUT_CAPTURE.initialise(&inputCaptureSettings, CaptureEvent);
    noSignalTimeout = TimerGetTicks64() + NO_SIGNAL_TIMEOUT;
}

/**
 * @brief Deinitialises the module.
 */
void LtcDeinitialise(void) {
    LTC_INPUT_CAPTURE.deinitialise();
    ClearFrameBuffer();
    holdoffTimeout = 0;
    decodeError = 0;
    noSignalTimeout = 0;
    FifoClear(&fifo);
    bufferOverflow = 0;
}

/**
 * @brief Module tasks. This function should be called repeatedly within the
 * main program loop.
 */
void LtcTasks(void) {
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
    LTC_INPUT_CAPTURE.initialise(&inputCaptureSettings, CaptureEvent);
}

/**
 * @brief Capture event callback.
 * @param ticks Ticks.
 */
static void CaptureEvent(const uint64_t ticks) {

    // Hardware buffer overrun
    if (LTC_INPUT_CAPTURE.bufferOverrun()) {
        DecodeError("Hardware buffer overrun");
        Holdoff(ticks);
        ClearFrameBuffer();
        return;
    }

    // Calculate pulse width
    static uint64_t previous;
    const uint64_t pulseWidth = ticks - previous;
    previous = ticks;

    // Receive pulse width
    switch (Receive(pulseWidth)) {
        case ReceiveResultOk:
            noSignalTimeout = ticks + NO_SIGNAL_TIMEOUT;
            const FifoPacket fifoPacket = {
                .ticks = ticks,
                .data = frameBuffer.data,
            };
            if (FifoWrite(&fifo, &fifoPacket, sizeof (fifoPacket)) != FifoResultOk) {
                bufferOverflow++;
            }
            break;
        case ReceiveResultInProgress:
            return;
        case ReceiveResultLowBitRate:
            DecodeError("Low bit rate");
            break;
        case ReceiveResultHighBitRate:
            DecodeError("High bit rate");
            Holdoff(ticks);
            break;
        case ReceiveResultFrameOverrun:
            DecodeError("Frame overrun");
            break;
        case ReceiveResultInvalidFrame:
            DecodeError("Invalid frame");
            break;
    }
    ClearFrameBuffer();
}

/**
 * @brief Disables the input capture for the holdoff period.
 * @param now Now.
 */
static inline __attribute__((always_inline)) void Holdoff(const uint64_t now) {
    LTC_INPUT_CAPTURE.deinitialise();
    holdoffTimeout = now + HOLDOFF_PERIOD;
}

/**
 * @brief Receives each pulse width to decode the LTC waveform. This function
 * must be called for the pulse width between every consecutive edge.
 * @param pulseWidth Pulse width.
 * @return Result.
 */
static inline __attribute__((always_inline)) ReceiveResult Receive(const uint64_t pulseWidth) {

    // Validate pulse width
    if (pulseWidth > MAX_PULSE_WIDTH) {
        return ReceiveResultLowBitRate;
    }
    if (pulseWidth < MIN_PULSE_WIDTH) {
        return ReceiveResultHighBitRate;
    }

    // Whole bit period (0 bit)
    if (pulseWidth > PULSE_WIDTH_THRESHOLD) {
        secondHalfBitPeriodPending = false;
        DetectSyncWord(false);
        if (WriteFrameBuffer(false)) {
            return ReceiveResultFrameOverrun;
        }
        return ReceiveResultInProgress;
    }

    // Half bit period (1 bit)
    if (secondHalfBitPeriodPending == false) {
        secondHalfBitPeriodPending = true;
        return ReceiveResultInProgress;
    }
    secondHalfBitPeriodPending = false;
    if (WriteFrameBuffer(true)) {
        return ReceiveResultFrameOverrun;
    }

    // Final bit of sync word
    if (DetectSyncWord(true) == false) {
        return ReceiveResultInProgress;
    }

    // Validate frame buffer
    if ((frameBufferIndex != 80)
        || (frameBuffer.data.frameNumberUnits > 9)
        || (frameBuffer.data.frameNumberTens > 2)
        || (frameBuffer.data.secondsUnits > 9)
        || (frameBuffer.data.secondsTens > 5)
        || (frameBuffer.data.minutesUnits > 9)
        || (frameBuffer.data.minutesTens > 5)
        || (frameBuffer.data.hoursUnits > 9)
        || (frameBuffer.data.hoursTens > 2)
        || (frameBuffer.syncWord != 0xBFFC)) {
        return ReceiveResultInvalidFrame;
    }
    return ReceiveResultOk;
}

/**
 * @brief Detects the sync word. This function must be called for every bit and
 * will return true if the bit is the final bit of the sync word. The sync word
 * ends with a 1 bit, so this function will never return true if the bit is
 * false.
 * @param bit Bit.
 * @return True if the bit is the final bit of the sync word.
 */
static inline __attribute__((always_inline)) bool DetectSyncWord(const bool bit) {

    typedef enum {
        StateTwelveOnes,
        StateFinalZero,
        StateFinalOne,
    } State;

    static State state = StateTwelveOnes;
    switch (state) {
        case StateTwelveOnes:
        {
            static int count;
            if (bit == false) {
                count = 0;
                break;
            }
            if (++count >= 12) {
                count = 0;
                state = StateFinalZero;
            }
            break;
        }
        case StateFinalZero:
            if (bit) {
                state = StateTwelveOnes;
            } else {
                state = StateFinalOne;
            }
            break;
        case StateFinalOne:
            state = StateTwelveOnes;
            if (bit) {
                return true;
            }
            break;
    }
    return false;
}

/**
 * @brief Writes a bit to the frame buffer and returns true if a frame overrun
 * error occurred.
 * @param bit Bit.
 * @return True if a frame overrun error occurred.
 */
static inline __attribute__((always_inline)) bool WriteFrameBuffer(const bool bit) {
    if (frameBufferIndex < 64) {
        frameBuffer.dataWord |= (uint64_t) (bit ? 1 : 0) << frameBufferIndex++;
        return false;
    }
    if (frameBufferIndex < 80) {
        frameBuffer.syncWord |= (uint16_t) (bit ? 1 : 0) << (frameBufferIndex++ -64);
        return false;
    }
    return true;
}

/**
 * @brief Clears the frame buffer.
 */
static inline __attribute__((always_inline)) void ClearFrameBuffer(void) {
    memset(&frameBuffer, 0, sizeof (frameBuffer));
    frameBufferIndex = 0;
    secondHalfBitPeriodPending = false;
}

/**
 * @brief Handles a decode error.
 * @param error Error.
 */
static inline __attribute__((always_inline)) void DecodeError(const char* const error) {
    decodeError = 1;
#ifdef PRINT_DECODE_ERRORS
    printf("%s\n", error);
#endif
}

/**
 * @brief Returns true if a decode error occurred. Calling this function will
 * reset the flag.
 * @return True if a decode error occurred.
 */
bool LtcDecodeError(void) {
    return __sync_lock_test_and_set(&decodeError, 0) != 0;
}

/**
 * @brief Returns true if there is no signal.
 * @return True if there is no signal.
 */
bool LtcNoSignal(void) {
    const bool state = EVIC_INT_Disable();
    const uint64_t timeout = noSignalTimeout;
    EVIC_INT_Restore(state);
    if (timeout == 0) {
        return false;
    }
    return TimerGetTicks64() >= timeout;
}

/**
 * @brief Gets data.
 * @param data Data.
 * @return Result.
 */
LtcResult LtcGetData(LtcData * const data) {
    FifoPacket fifoPacket;
    if (FifoRead(&fifo, &fifoPacket, sizeof (fifoPacket)) == 0) {
        return LtcResultError;
    }
    data->ticks = fifoPacket.ticks;
    const char timecode[LTC_TIMECODE_SIZE] = {
        '0' + fifoPacket.data.hoursTens,
        '0' + fifoPacket.data.hoursUnits,
        ':',
        '0' + fifoPacket.data.minutesTens,
        '0' + fifoPacket.data.minutesUnits,
        ':',
        '0' + fifoPacket.data.secondsTens,
        '0' + fifoPacket.data.secondsUnits,
        fifoPacket.data.dropFrameFlag ? ';' : ':',
        '0' + fifoPacket.data.frameNumberTens,
        '0' + fifoPacket.data.frameNumberUnits,
        '\0',
    };
    memcpy(data->timecode, timecode, sizeof (timecode));
    return LtcResultOk;
}

/**
 * @brief Returns the number of frames lost due to buffer overflow. Calling
 * this function will reset the value.
 * @return Number of frames lost due to buffer overflow.
 */
uint32_t LtcBufferOverflow(void) {
    return __sync_lock_test_and_set(&bufferOverflow, 0);
}

//------------------------------------------------------------------------------
// End of file
