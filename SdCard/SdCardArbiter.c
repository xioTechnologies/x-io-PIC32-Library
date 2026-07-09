/**
 * @file SdCardArbiter.c
 * @author Seb Madgwick
 * @brief Manages access to the SD card.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "SdCard/SdCard.h"
#include "SdCardArbiter.h"
#include <stddef.h>
#include <stdio.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to print access.
 */
//#define PRINT_ACCESS

//------------------------------------------------------------------------------
// Variables

static const SdCardArbiterClient* clients[SD_CARD_ARBITER_MAX_NUMBER_OF_CLIENTS];
static const SdCardArbiterClient* activeClient;
#ifdef PRINT_ACCESS
static const SdCardArbiterClient* previousClient;
#endif

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Adds a client.
 * @param client Client.
 */
void SdCardArbiterAdd(const SdCardArbiterClient * const client) {
    for (int index = 0; index < SD_CARD_ARBITER_MAX_NUMBER_OF_CLIENTS; index++) {
        if (clients[index] == NULL) {
            clients[index] = client;
            return;
        }
    }
#ifdef PRINT_ACCESS
    printf("Too many clients. %s discarded.\n", client->name);
#endif
}

/**
 * @brief Module tasks. This function should be called repeatedly within the
 * main program loop.
 */
void SdCardArbiterTasks(void) {
    const SdCardArbiterClient* requestingClient = NULL;

    // Check for requests
    for (int index = 0; index < SD_CARD_ARBITER_MAX_NUMBER_OF_CLIENTS; index++) {
        const SdCardArbiterClient * const client = clients[index];

        // End of list
        if (client == NULL) {
            break;
        }

        // No request
        if (client->request() == false) {
            continue;
        }

        // Deny request if priority not greater than active client
        if ((activeClient != NULL) && (client->priority <= activeClient->priority)) {
            continue;
        }

        // Deny request if priority not greater than existing request
        if ((requestingClient != NULL) && (client->priority <= requestingClient->priority)) {
            continue;
        }

        // Tentatively accept request
        requestingClient = client;
        SdCardArbiterEnd();
        SdCardMount();
    }

    // Deny request if SD card not mounted
    if (SdCardMounted() == false) {
        requestingClient = NULL;
    }

    // Accept request
    if (requestingClient != NULL) {
        activeClient = requestingClient;
#ifdef PRINT_ACCESS
        printf("%s prologue\n", activeClient->name);
#endif
        if (activeClient->prologue != NULL) {
            activeClient->prologue();
        }
    }

    // Do nothing else if no active client
    if (activeClient == NULL) {
        return;
    }

    // Active client tasks
#ifdef PRINT_ACCESS
    if (activeClient != previousClient) {
        printf("%s tasks\n", activeClient->name);
        previousClient = activeClient;
    }
#endif
    if (activeClient->tasks != NULL) {
        activeClient->tasks();
    }
}

/**
 * @brief Ends active client access. This function must not be called from a
 * request callback.
 */
void SdCardArbiterEnd(void) {
    if (activeClient == NULL) {
        return;
    }
#ifdef PRINT_ACCESS
    printf("%s epilogue\n", activeClient->name);
    previousClient = NULL;
#endif
    if (activeClient->epilogue != NULL) {
        activeClient->epilogue();
    }
    activeClient = NULL;
}

//------------------------------------------------------------------------------
// End of file
