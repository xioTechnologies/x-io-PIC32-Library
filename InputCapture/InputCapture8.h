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
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const InputCapture inputCapture8;

//------------------------------------------------------------------------------
// Function declarations

void InputCapture8Initialise(const InputCaptureSettings * const settings, void (*const captureEvent_) (const uint64_t ticks));
void InputCapture8Deinitialise(void);
void InputCapture8Trigger(void);
bool InputCapture8BufferOverrun(void);

#endif

//------------------------------------------------------------------------------
// End of file
