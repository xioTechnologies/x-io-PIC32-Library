/**
 * @file Eeprom.c
 * @author Seb Madgwick
 * @brief Microchip 24xx32 I2C EEPROM driver.
 */

//------------------------------------------------------------------------------
// Includes

#include "Eeprom.h"
#include "I2C/I2CStartSequence.h"
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief I2C client address.
 */
#define I2C_CLIENT_ADDRESS (0x50)

/**
 * @brief EEPROM size.
 */
#define EEPROM_SIZE (0x1000)

/**
 * @brief Page size.
 */
#define PAGE_SIZE (32)

/**
 * @brief Print line length. Must be a multiple of the EEPROM size.
 */
#define PRINT_LINE_LENGTH (32)

//------------------------------------------------------------------------------
// Function declarations

static void StartSequence(const I2C * const i2c, const uint16_t address);
static void PrintLine(const uint16_t address, const uint8_t * const data);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Reads data.
 * @param i2c I2C interface.
 * @param address Address.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes.
 */
void EepromRead(const I2C * const i2c, const uint16_t address, void* const destination, const size_t numberOfBytes) {
    StartSequence(i2c, address);
    i2c->repeatedStart();
    i2c->sendAddressRead(I2C_CLIENT_ADDRESS);
    const size_t endIndex = numberOfBytes - 1;
    size_t destinationIndex = 0;
    while (destinationIndex < numberOfBytes) {
        const bool ack = destinationIndex < endIndex;
        ((uint8_t*) destination)[destinationIndex] = i2c->receive(ack);
        destinationIndex++;
    }
    i2c->stop();
}

/**
 * @brief Writes data.
 * @param i2c I2C interface.
 * @param address Address.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void EepromWrite(const I2C * const i2c, uint16_t address, const void* const data, const size_t numberOfBytes) {
    StartSequence(i2c, address);
    const uint16_t endAddress = address + numberOfBytes;
    uint8_t* dataByte = (uint8_t*) data;
    int currentPageIndex = address / PAGE_SIZE;
    while (address < endAddress) {
        i2c->send(*dataByte++);
        const int nextPageIndex = ++address / PAGE_SIZE;
        if (nextPageIndex != currentPageIndex) { // if crossing page boundary
            currentPageIndex = nextPageIndex;
            i2c->stop();
            StartSequence(i2c, address);
        }
    }
    i2c->stop();
}

/**
 * @brief Writes data only if the data being written is different to the data in
 * the EEPROM.
 * @param i2c I2C interface.
 * @param address Address.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 */
void EepromUpdate(const I2C * const i2c, uint16_t address, const void* const data, const size_t numberOfBytes) {
    const uint16_t endAddress = address + numberOfBytes;
    const uint8_t* dataByte = (uint8_t*) data;
    while (address < endAddress) {
        size_t chunkSize = PAGE_SIZE - (address % PAGE_SIZE); // number of bytes from address to end of page
        if ((address + chunkSize) > endAddress) {
            chunkSize = endAddress - address;
        }
        uint8_t pageData[PAGE_SIZE];
        EepromRead(i2c, address, pageData, chunkSize);
        if (memcmp(dataByte, pageData, chunkSize) != 0) {
            EepromWrite(i2c, address, dataByte, chunkSize);
        }
        address += chunkSize;
        dataByte += chunkSize;
    }
}

/**
 * @brief Start sequence common to read and write operations. Implements
 * acknowledge polling to minimise delay while device is engaged in write
 * cycle.
 * @param i2c I2C interface.
 * @param address Address.
 */
static void StartSequence(const I2C * const i2c, const uint16_t address) {
    I2CStartSequence(i2c, I2C_CLIENT_ADDRESS, 5); // 5 ms
    i2c->send(address >> 8);
    i2c->send(address & 0xFF);
}

/**
 * @brief Erases the EEPROM. All data bytes are set to 0xFF.
 */
void EepromErase(const I2C * const i2c) {
    const uint8_t blankPage[] = {[0 ... (PAGE_SIZE - 1)] = 0xFF};
    for (int index = 0; index < (EEPROM_SIZE / PAGE_SIZE); index++) {
        EepromWrite(i2c, index * PAGE_SIZE, blankPage, sizeof (blankPage));
    }
}

/**
 * @brief Returns true if the EEPROM is blank.
 * @param i2c I2C interface.
 * @return True if the EEPROM is blank.
 */
bool EepromBlank(const I2C * const i2c) {
    for (int index = 0; index < (EEPROM_SIZE / PAGE_SIZE); index++) {
        uint8_t pageData[PAGE_SIZE];
        EepromRead(i2c, index * PAGE_SIZE, pageData, sizeof (pageData));
        const uint8_t blankPage[] = {[0 ... (PAGE_SIZE - 1)] = 0xFF};
        if (memcmp(blankPage, pageData, sizeof (pageData)) != 0) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Prints the EEPROM.
 * @param i2c I2C interface.
 */
void EepromPrint(const I2C * const i2c) {
    bool printEllipses = true;
    for (uint16_t address = 0; address < EEPROM_SIZE; address += PRINT_LINE_LENGTH) {

        // Read data
        uint8_t data[PRINT_LINE_LENGTH];
        EepromRead(i2c, address, data, sizeof (data));

        // Print first and last line
        if ((address == 0) || (address == (EEPROM_SIZE - PRINT_LINE_LENGTH))) {
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
 * @param address Address. 0xFFFF for ellipses.
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

/**
 * @brief Performs self-test.
 * @param i2c I2C interface.
 * @return Test result.
 */
EepromTestResult EepromTest(const I2C * const i2c) {

    // Test client ACK
    const bool ack = I2CStartSequence(i2c, I2C_CLIENT_ADDRESS, 5); // 5 ms
    i2c->stop();
    if (ack == false) {
        return EepromTestResultAckFailed;
    }

    // Read data
    uint32_t readData;
    const uint16_t address = EEPROM_SIZE - sizeof (readData);
    EepromRead(i2c, address, &readData, sizeof (readData));

    // Write modified data
    const uint32_t writeData = readData + 1;
    EepromWrite(i2c, address, &writeData, sizeof (writeData));

    // Read modified data
    EepromRead(i2c, address, &readData, sizeof (readData));

    // Check that read data matches write data
    if (readData != writeData) {
        return EepromTestResultDataMismatch;
    }

    // Self-test passed
    return EepromTestResultPassed;
}

/**
 * @brief Returns the test result message.
 * @param result Test result.
 * @return Test result message.
 */
const char* EepromTestResultToString(const EepromTestResult result) {
    switch (result) {
        case EepromTestResultPassed:
            return "Passed";
        case EepromTestResultAckFailed:
            return "ACK failed";
        case EepromTestResultDataMismatch:
            return "Data mismatch";
    }
    return ""; // avoid compiler warning
}

//------------------------------------------------------------------------------
// End of file
