/**
 * @file Sync.h
 * @author Seb Madgwick
 * @brief Synchronisation input.
 */

#ifndef SYNC_H
#define SYNC_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Result.
 */
typedef enum {
    SyncResultOk,
    SyncResultError,
} SyncResult;

/**
 * @brief Data.
 */
typedef struct {
    uint64_t ticks;
    bool edge;
} SyncData;

//------------------------------------------------------------------------------
// Function declarations

void SyncInitialise(void);
void SyncDeinitialise(void);
void SyncTasks(void);
bool SyncCaptureError(void);
SyncResult SyncGetData(SyncData * const data);
uint32_t SyncBufferOverflow(void);

#endif

//------------------------------------------------------------------------------
// End of file
