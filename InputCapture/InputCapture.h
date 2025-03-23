/**
 * @file InputCapture.h
 * @author Seb Madgwick
 * @brief Input capture driver for PIC32 devices.
 */

#ifndef INPUT_CAPTURE_H
#define INPUT_CAPTURE_H

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Input capture edge.
 */
typedef enum {
    InputCaptureEdgeFalling,
    InputCaptureEdgeRising,
} InputCaptureEdge;

#endif

//------------------------------------------------------------------------------
// End of file
