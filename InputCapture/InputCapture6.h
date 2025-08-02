/**
 * @file InputCapture6.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_6_H
#define INPUT_CAPTURE_6_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void InputCapture6Initialise(const InputCaptureEdge edge, void (*const captureEvent_) (const uint64_t ticks));
void InputCapture6Deinitialise(void);
void InputCapture6Trigger(void);

#endif

//------------------------------------------------------------------------------
// End of file
