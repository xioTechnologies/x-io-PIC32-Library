/**
 * @file InputCapture7.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_7_H
#define INPUT_CAPTURE_7_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void InputCapture7Initialise(const InputCaptureEdge edge, void (*captureEvent_)(const uint64_t ticks));
void InputCapture7Deinitialise(void);
void InputCapture7Trigger(void);

#endif

//------------------------------------------------------------------------------
// End of file
