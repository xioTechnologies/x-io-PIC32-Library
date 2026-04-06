/**
 * @file InputCapture.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_H
#define INPUT_CAPTURE_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Edge.
 */
typedef enum {
    InputCaptureEdgeFalling,
    InputCaptureEdgeRising,
    InputCaptureEdgeEvery,
} InputCaptureEdge;

/**
 * @brief Interrupt.
 */
typedef enum {
    InputCaptureInterruptEvery,
    InputCaptureInterruptSecond,
    InputCaptureInterruptThird,
    InputCaptureInterruptFourth,
} InputCaptureInterrupt;

/**
 * @brief Settings.
 */
typedef struct {
    InputCaptureEdge edge;
    InputCaptureInterrupt interrupt;
} InputCaptureSettings;

/**
 * @brief Input capture interface.
 */
typedef struct {
    void (*const initialise) (const InputCaptureSettings * const settings, void (*const captureEvent_) (const uint64_t ticks));
    void (*const deinitialise) (void);
    bool (*const bufferOverrun) (void);
} InputCapture;

//------------------------------------------------------------------------------
// Variable declarations

extern const InputCaptureSettings inputCaptureSettingsDefault;

#endif

//------------------------------------------------------------------------------
// End of file
