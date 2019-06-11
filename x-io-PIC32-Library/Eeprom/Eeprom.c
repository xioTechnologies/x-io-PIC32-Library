/**
 * @file Eeprom.c
 * @author Seb Madgwick
 * @brief Driver for Microchip 24xx32 to 24xx512 I2C EEPROM.
 */

//------------------------------------------------------------------------------
// Includes

#include "Eeprom.h"
#include "EepromHal.h"
#include <stdbool.h>
#include <stdio.h> // printf
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Function prototypes

static void StartSequence(const uint16_t address);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Reads data.
 * @param address Address.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 */
void EepromRead(const uint16_t address, void* const destination, const size_t numberOfBytes) {
    StartSequence(address);
    EepromHalI2CStop();
    EepromHalI2CStart();
    EepromHalI2CSend(I2CSlaveAddressRead(EEPROM_I2C_ADDRESS));
    const uint32_t endIndex = numberOfBytes - 1;
    uint32_t destinationIndex = 0;
    while (destinationIndex < numberOfBytes) {
        const bool ack = destinationIndex < endIndex;
        ((uint8_t*) destination)[destinationIndex] = EepromHalI2CReceive(ack);
        destinationIndex++;
    }
    EepromHalI2CStop();
}

/**
 * @brief Writes data.
 * @param address Address.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void EepromWrite(uint16_t address, const void* data, const size_t numberOfBytes) {
    StartSequence(address);
    const uint16_t endAddress = address + numberOfBytes;
    uint8_t* dataByte = (uint8_t*) data;
    uint32_t currentPageIndex = address / EEPROM_PAGE_SIZE;
    while (address < endAddress) {
        EepromHalI2CSend(*dataByte++);
        const uint32_t nextPageIndex = ++address / EEPROM_PAGE_SIZE;
        if (nextPageIndex != currentPageIndex) { // if crossing page boundary
            currentPageIndex = nextPageIndex;
            EepromHalI2CStop();
            StartSequence(address);
        }
    }
    EepromHalI2CStop();
}

/**
 * @brief Start sequence common to read and write functions.  Implements
 * acknowledge polling to minimise delay while device is engaged in write
 * cycle.
 * @param address Address.
 */
static void StartSequence(const uint16_t address) {
    const uint64_t startTicks = TimerGetTicks64();
    while (true) {
        EepromHalI2CStart();
        if (EepromHalI2CSend(I2CSlaveAddressWrite(EEPROM_I2C_ADDRESS)) == true) {
            break;
        }
        if ((TimerGetTicks64() - startTicks) > (TIMER_TICKS_PER_SECOND / 200)) { // 5 ms timeout
            break;
        }
    }
    EepromHalI2CSend(address >> 8);
    EepromHalI2CSend(address & 0xFF);
}

/**
 * @brief Erases the EEPROM.  All data bytes are set to 0xFF.
 */
void EepromEraseAll() {
    const uint8_t blankPage[EEPROM_PAGE_SIZE] = {[0 ... (EEPROM_PAGE_SIZE - 1)] = 0xFF};
    int pageIndex;
    for (pageIndex = 0; pageIndex < (EEPROM_SIZE / EEPROM_PAGE_SIZE); pageIndex++) {
        EepromWrite(pageIndex * EEPROM_PAGE_SIZE, (uint8_t*) blankPage, sizeof (blankPage));
    }
}

/**
 * @brief Prints the EEPROM.
 */
void EepromPrint() {
    int pageIndex;
    for (pageIndex = 0; pageIndex < (EEPROM_SIZE / EEPROM_PAGE_SIZE); pageIndex++) {
        int8_t pageData[EEPROM_PAGE_SIZE];
        EepromRead(pageIndex * EEPROM_PAGE_SIZE, pageData, sizeof (pageData));
        int index;
        for (index = 0; index < sizeof (pageData); index++) {

            // Print address
            const uint16_t address = (pageIndex * EEPROM_PAGE_SIZE) + index;
            if (address % 32 == 0) {
                if (address > 0) {
                    printf("\r\n");
                }
                printf("%04X | ", address);
            }

            // Print byte value
            if ((pageData[index] >= 0x20) && (pageData[index] <= 0x7E)) {
                printf(" %c ", (char) pageData[index]);
            } else {
                printf("%02X ", (uint8_t) pageData[index]);
            }
        }
    }
    printf("\r\n");
}

//------------------------------------------------------------------------------
// End of file
