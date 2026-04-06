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
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const InputCapture inputCapture9;

//------------------------------------------------------------------------------
// Function declarations

void InputCapture9Initialise(const InputCaptureSettings * const settings, void (*const captureEvent_) (const uint64_t ticks));
void InputCapture9Deinitialise(void);
void InputCapture9Trigger(void);
bool InputCapture9BufferOverrun(void);

#endif

//------------------------------------------------------------------------------
// End of file
