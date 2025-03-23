/**
 * @file InputCapture9.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_9_H
#define INPUT_CAPTURE_9_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void InputCapture9Initialise(const InputCaptureEdge edge, void (*captureEvent_)(const uint64_t ticks));
void InputCapture9Deinitialise(void);
void InputCapture9Trigger(void);

#endif

//------------------------------------------------------------------------------
// End of file
