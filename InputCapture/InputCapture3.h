/**
 * @file InputCapture3.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_3_H
#define INPUT_CAPTURE_3_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void InputCapture3Initialise(const InputCaptureEdge edge, void (*const captureEvent_) (const uint64_t ticks));
void InputCapture3Deinitialise(void);
void InputCapture3Trigger(void);

#endif

//------------------------------------------------------------------------------
// End of file
