/**
 * @file InputCapture6.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_6_H
#define INPUT_CAPTURE_6_H

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Variable declarations

extern const InputCapture inputCapture6;

//------------------------------------------------------------------------------
// Function declarations

void InputCapture6Initialise(const InputCaptureSettings * const settings, void (*const captureEvent_) (const uint64_t ticks));
void InputCapture6Deinitialise(void);
void InputCapture6Trigger(void);
bool InputCapture6BufferOverrun(void);

#endif

//------------------------------------------------------------------------------
// End of file
