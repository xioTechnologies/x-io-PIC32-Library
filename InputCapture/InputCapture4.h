/**
 * @file InputCapture4.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_4_H
#define INPUT_CAPTURE_4_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void InputCapture4Initialise(const InputCaptureEdge edge, void (*const captureEvent_) (const uint64_t ticks));
void InputCapture4Deinitialise(void);
void InputCapture4Trigger(void);

#endif

//------------------------------------------------------------------------------
// End of file
