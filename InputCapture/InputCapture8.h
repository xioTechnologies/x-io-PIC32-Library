/**
 * @file InputCapture8.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_8_H
#define INPUT_CAPTURE_8_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void InputCapture8Initialise(const InputCaptureEdge edge, void (*captureEvent_)(const uint64_t ticks));
void InputCapture8Deinitialise(void);
void InputCapture8Trigger(void);

#endif

//------------------------------------------------------------------------------
// End of file
