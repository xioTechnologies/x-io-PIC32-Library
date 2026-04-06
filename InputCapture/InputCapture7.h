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
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const InputCapture inputCapture7;

//------------------------------------------------------------------------------
// Function declarations

void InputCapture7Initialise(const InputCaptureSettings * const settings, void (*const captureEvent_) (const uint64_t ticks));
void InputCapture7Deinitialise(void);
void InputCapture7Trigger(void);
bool InputCapture7BufferOverrun(void);

#endif

//------------------------------------------------------------------------------
// End of file
