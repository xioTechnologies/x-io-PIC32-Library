/**
 * @file InputCapture3.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_3_H
#define INPUT_CAPTURE_3_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const InputCapture inputCapture3;

//------------------------------------------------------------------------------
// Function declarations

void InputCapture3Initialise(const InputCaptureSettings * const settings, void (*const captureEvent_) (const uint64_t ticks));
void InputCapture3Deinitialise(void);
void InputCapture3Trigger(void);
bool InputCapture3BufferOverrun(void);

#endif

//------------------------------------------------------------------------------
// End of file
