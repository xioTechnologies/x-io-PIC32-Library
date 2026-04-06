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
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const InputCapture inputCapture5;

//------------------------------------------------------------------------------
// Function declarations

void InputCapture5Initialise(const InputCaptureSettings * const settings, void (*const captureEvent_) (const uint64_t ticks));
void InputCapture5Deinitialise(void);
void InputCapture5Trigger(void);
bool InputCapture5BufferOverrun(void);

#endif

//------------------------------------------------------------------------------
// End of file
