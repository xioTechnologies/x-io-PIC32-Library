/**
 * @file SdCardArbiter.h
 * @author Seb Madgwick
 * @brief Manages access to the SD card.
 */

#ifndef SD_CARD_ARBITER_H
#define SD_CARD_ARBITER_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Client.
 */
typedef struct {
    const char* name;
    int priority; // may be changed at any time
    bool (*const request) (void); // return true to request access
    void (*const prologue) (void); // NULL if unused
    void (*const tasks) (void); // NULL if unused
    void (*const epilogue) (void); // NULL if unused
} SdCardArbiterClient;

//------------------------------------------------------------------------------
// Function declarations

void SdCardArbiterAdd(const SdCardArbiterClient * const client);
void SdCardArbiterTasks(void);
void SdCardArbiterEnd(void);

#endif

//------------------------------------------------------------------------------
// End of file
