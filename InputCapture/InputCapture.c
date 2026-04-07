/**
 * @file InputCapture.c
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "InputCapture.h"

//------------------------------------------------------------------------------
// Variables

const InputCaptureSettings inputCaptureSettingsDefault = {
    .edge = InputCaptureEdgeRising,
    .interrupt = InputCaptureInterruptEvery,
    .firstEdge = false,
};

//------------------------------------------------------------------------------
// End of file
