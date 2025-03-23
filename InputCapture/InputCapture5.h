/**
 * @file InputCapture5.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_5_H
#define INPUT_CAPTURE_5_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdint.h>

//------------------------------------------------------------------------------
// Function declarations

void InputCapture5Initialise(const InputCaptureEdge edge, void (*captureEvent_)(const uint64_t ticks));
void InputCapture5Deinitialise(void);
void InputCapture5Trigger(void);

#endif

//------------------------------------------------------------------------------
// End of file
