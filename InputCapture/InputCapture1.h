/**
 * @file InputCapture1.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_1_H
#define INPUT_CAPTURE_1_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void InputCapture1Initialise(const InputCaptureEdge edge, void (*captureEvent_)(const uint64_t ticks));
void InputCapture1Deinitialise(void);
void InputCapture1Trigger(void);

#endif

//------------------------------------------------------------------------------
// End of file
