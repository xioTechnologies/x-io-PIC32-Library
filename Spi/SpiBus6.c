/**
 * @file SpiBus6.c
 * @author Seb Madgwick
 * @brief SPI bus.
 */

//------------------------------------------------------------------------------
// Includes

#include "Config.h"
#include "SpiBus6.h"

//------------------------------------------------------------------------------
// Function declarations

static void BeginTrasnfer(SpiBusClient * const client);
static void TransferComplete(void);

//------------------------------------------------------------------------------
// Variables

const SpiBus spiBus6 = {
    .addClient = SpiBus6AddClient,
    .transfer = SpiBus6Transfer,
    .transferInProgress = SpiBus6TransferInProgress,
};
static int numberOfClients;
static SpiBusClient clients[SPI_BUS_6_MAX_NUMBER_OF_CLIENTS];
static SpiBusClient * volatile activeClient;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Adds a client to the SPI bus.
 * @param csPin CS pin.
 * @return Client.
 */
SpiBusClient * const SpiBus6AddClient(const GPIO_PIN csPin) {
    if (numberOfClients >= SPI_BUS_6_MAX_NUMBER_OF_CLIENTS) {
        return NULL;
    }
    SpiBusClient * const client = &clients[numberOfClients++];
    client->csPin = csPin;
    return client;
}

/**
 * @brief Transfers data. The data will be overwritten with the received data.
 * The data must be declared __attribute__((coherent)) for PIC32MZ devices.
 * This function must not be called while a transfer is in progress. The
 * transfer complete callback will be called from within an interrupt once the
 * transfer is complete.
 * @param client Client.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @param transferComplete Transfer complete callback.
 */
void SpiBus6Transfer(SpiBusClient * const client, volatile void* const data, const size_t numberOfBytes, void (*const transferComplete) (void)) {
    if (client == NULL) {
        return;
    }
    client->data = data;
    client->numberOfBytes = numberOfBytes;
    client->transferComplete = transferComplete;
    client->inProgress = true; // set this member last else interrupt may use invalid structure
    BeginTrasnfer(client);
}

/**
 * @brief Returns true while the transfer is in progress.
 * @param client Client.
 * @return True while the transfer is in progress.
 */
bool SpiBus6TransferInProgress(const SpiBusClient * const client) {
    if (client == NULL) {
        return false;
    }
    return client->inProgress;
}

/**
 * @brief Begins transfer.
 * @param client Client.
 */
static void BeginTrasnfer(SpiBusClient * const client) {
    static int lock;
    if (__sync_lock_test_and_set(&lock, 1) == 1) {
        return;
    }
    if (activeClient == NULL) {
        activeClient = client;
        SPI_BUS_6_SPI.transfer(activeClient->csPin, activeClient->data, activeClient->numberOfBytes, TransferComplete);
    }
    __sync_lock_release(&lock);
}

/**
 * @brief Transfer complete callback.
 */
static void TransferComplete(void) {

    // End current transfer
    activeClient->inProgress = false;
    if (activeClient->transferComplete != NULL) {
        activeClient->transferComplete();
    }
    activeClient = NULL;

    // Begin next transfer
    for (int index = 0; index < SPI_BUS_6_MAX_NUMBER_OF_CLIENTS; index++) {
        if (clients[index].inProgress) {
            BeginTrasnfer(&clients[index]);
            return;
        }
    }
}

//------------------------------------------------------------------------------
// End of file
