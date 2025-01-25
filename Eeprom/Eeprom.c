/**
 * @file Eeprom.c
 * @author Seb Madgwick
 * @brief Driver for Microchip 24xx32 to 24xx512 I2C EEPROM.
 */

//------------------------------------------------------------------------------
// Includes

#include "Eeprom.h"
#include "EepromHal.h"
#include "I2C/I2CSlaveAddress.h"
#include "I2C/I2CStartSequence.h"
#include <stdbool.h>
#include <stdio.h> // printf
#include <string.h> // memcmp

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Print line length.  Must be a multiple of the EEPROM size.
 */
#define PRINT_LINE_LENGTH (32)

//------------------------------------------------------------------------------
// Function declarations

static void StartSequence(const uint16_t address);
static void PrintLine(const uint16_t address, const uint8_t * const data);

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
    EepromHalI2CRepeatedStart();
    EepromHalI2CSend(I2CSlaveAddressRead(EEPROM_HAL_I2C_ADDRESS));
    const size_t endIndex = numberOfBytes - 1;
    size_t destinationIndex = 0;
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
void EepromWrite(uint16_t address, const void* const data, const size_t numberOfBytes) {
    StartSequence(address);
    const uint16_t endAddress = address + numberOfBytes;
    uint8_t* dataByte = (uint8_t*) data;
    int currentPageIndex = address / EEPROM_HAL_PAGE_SIZE;
    while (address < endAddress) {
        EepromHalI2CSend(*dataByte++);
        const int nextPageIndex = ++address / EEPROM_HAL_PAGE_SIZE;
        if (nextPageIndex != currentPageIndex) { // if crossing page boundary
            currentPageIndex = nextPageIndex;
            EepromHalI2CStop();
            StartSequence(address);
        }
    }
    EepromHalI2CStop();
}

/**
 * @brief Writes data only if the data being written is different to the data
 * on the EEPROM.
 * @param address Address.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void EepromUpdate(uint16_t address, const void* const data, const size_t numberOfBytes) {
    const uint16_t endAddress = address + numberOfBytes;
    const uint8_t* dataByte = (uint8_t*) data;
    while (address < endAddress) {
        size_t chunkSize = EEPROM_HAL_PAGE_SIZE - (address % EEPROM_HAL_PAGE_SIZE); // number of bytes from address to end of page
        if ((address + chunkSize) > endAddress) {
            chunkSize = endAddress - address;
        }
        uint8_t pageData[EEPROM_HAL_PAGE_SIZE];
        EepromRead(address, pageData, chunkSize);
        if (memcmp(dataByte, pageData, chunkSize) != 0) {
            EepromWrite(address, dataByte, chunkSize);
        }
        address += chunkSize;
        dataByte += chunkSize;
    }
}

/**
 * @brief Start sequence common to read and write operations.  Implements
 * acknowledge polling to minimise delay while device is engaged in write
 * cycle.
 * @param address Address.
 */
static void StartSequence(const uint16_t address) {
    I2CStartSequence(EepromHalI2CStart, EepromHalI2CSend, EEPROM_HAL_I2C_ADDRESS, 5); // 5 ms
    EepromHalI2CSend(address >> 8);
    EepromHalI2CSend(address & 0xFF);
}

/**
 * @brief Erases the EEPROM.  All data bytes are set to 0xFF.
 */
void EepromErase(void) {
    const uint8_t blankPage[] = {[0 ... (EEPROM_HAL_PAGE_SIZE - 1)] = 0xFF};
    for (int index = 0; index < (EEPROM_HAL_SIZE / EEPROM_HAL_PAGE_SIZE); index++) {
        EepromWrite(index * EEPROM_HAL_PAGE_SIZE, blankPage, sizeof (blankPage));
    }
}

/**
 * @brief Returns true if the EEPROM is blank.
 * @return True if the EEPROM is blank.
 */
bool EepromBlank(void) {
    for (int index = 0; index < (EEPROM_HAL_SIZE / EEPROM_HAL_PAGE_SIZE); index++) {
        uint8_t pageData[EEPROM_HAL_PAGE_SIZE];
        EepromRead(index * EEPROM_HAL_PAGE_SIZE, pageData, sizeof (pageData));
        const uint8_t blankPage[] = {[0 ... (EEPROM_HAL_PAGE_SIZE - 1)] = 0xFF};
        if (memcmp(blankPage, pageData, sizeof (pageData)) != 0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Prints the EEPROM.
 */
void EepromPrint(void) {
    bool printEllipses = true;
    for (uint16_t address = 0; address < EEPROM_HAL_SIZE; address += PRINT_LINE_LENGTH) {

        // Read data
        uint8_t data[PRINT_LINE_LENGTH];
        EepromRead(address, data, sizeof (data));

        // Print first and last line
        if ((address == 0) || (address == (EEPROM_HAL_SIZE - PRINT_LINE_LENGTH))) {
            PrintLine(address, data);
            continue;
        }

        // Print line if data not blank
        const uint8_t blankData[] = {[0 ... (PRINT_LINE_LENGTH - 1)] = 0xFF};
        if (memcmp(blankData, data, sizeof (data)) != 0) {
            PrintLine(address, data);
            printEllipses = true;
            continue;
        }

        // Print ellipses
        if (printEllipses) {
            PrintLine(0xFFFF, data);
            printEllipses = false;
        }
    }
}

/**
 * @brief Prints line.
 * @param address Address.  0xFFFF for ellipses.
 * @param data Data.
 */
static void PrintLine(const uint16_t address, const uint8_t * const data) {

    // Print address
    if (address == 0xFFFF) {
        printf("...  | ");
    } else {
        printf("%04X | ", address);
    }

    // Print data
    for (size_t index = 0; index < PRINT_LINE_LENGTH; index++) {
        if ((data[index] >= 0x20) && (data[index] <= 0x7E)) { // if printable character
            printf(" %c ", (char) data[index]);
        } else {
            printf("%02X ", data[index]);
        }
    }
    printf("\n");
}

//------------------------------------------------------------------------------
// End of file
