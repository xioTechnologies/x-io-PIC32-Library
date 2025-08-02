/**
 * @file InputCapture2.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_2_H
#define INPUT_CAPTURE_2_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void InputCapture2Initialise(const InputCaptureEdge edge, void (*const captureEvent_) (const uint64_t ticks));
void InputCapture2Deinitialise(void);
void InputCapture2Trigger(void);

#endif

//------------------------------------------------------------------------------
// End of file
