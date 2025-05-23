/**
 * @file InputCapture6.c
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "InputCapture6.h"
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Variables

static void (*captureEvent)(const uint64_t ticks);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param edge Edge.
 * @param captureEvent_ Capture event callback.
 */
void InputCapture6Initialise(const InputCaptureEdge edge, void (*captureEvent_)(const uint64_t ticks)) {

    // Ensure default register states
    InputCapture6Deinitialise();

    // Configure input capture
    switch (edge) {
        case InputCaptureEdgeFalling:
            IC6CONbits.ICM = 0b010; // simple Capture Event mode - every falling edge
            break;
        case InputCaptureEdgeRising:
            IC6CONbits.ICM = 0b011; // simple Capture Event mode - every rising edge
            break;
    }
    IC6CONbits.C32 = 1;

    // Enable interrupt
    captureEvent = captureEvent_;
    EVIC_SourceEnable(INT_SOURCE_INPUT_CAPTURE_6);

    // Enable
    IC6CONbits.ON = 1;
}

/**
 * @brief Deinitialises the module.
 */
void InputCapture6Deinitialise(void) {
    IC6CON = 0;
    EVIC_SourceDisable(INT_SOURCE_INPUT_CAPTURE_6);
    EVIC_SourceStatusClear(INT_SOURCE_INPUT_CAPTURE_6);
}

/**
 * @brief Triggers a capture vent.
 */
void InputCapture6Trigger(void) {
    EVIC_SourceStatusSet(INT_SOURCE_INPUT_CAPTURE_6);
}

/**
 * @brief External interrupt handler. This function should be called by the ISR
 * implementation generated by MPLAB Harmony.
 */
void InputCapture6InterruptHandler(void) {
    uint64_t ticks = TimerGetTicks64();
    const uint32_t latency = (uint32_t) ticks - (uint32_t) IC6BUF;
    captureEvent(ticks - (uint64_t) latency);
    EVIC_SourceStatusClear(INT_SOURCE_INPUT_CAPTURE_6);
}

//------------------------------------------------------------------------------
// End of file
