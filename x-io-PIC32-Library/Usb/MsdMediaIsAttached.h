/**
 * @file MsdMediaIsAttached.h
 * @author Seb Madgwick
 * @brief Functions to allow an application to prevent the detection of an
 * attached SD card by MPLAB Harmony.
 */

#ifndef MSD_MEDIA_IS_ATTACHED_H
#define MSD_MEDIA_IS_ATTACHED_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>

//------------------------------------------------------------------------------
// Function prototypes

bool MsdMediaIsAttachedGet();
void MsdMediaIsAttachedSet(const bool isAttached);

#endif

//------------------------------------------------------------------------------
// End of file
