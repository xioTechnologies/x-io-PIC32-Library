/**
 * @file LastPage.c
 * @author Seb Madgwick
 * @brief Provides access to the last page of flash as NVM for application data.
 *
 * WARNING: Flash endurance is typically only 10,000 erase/write cycles.
 *
 * The linker script must be modified to reserve the last page of flash as shown
 * in the example below. In this example, the kseg0_program_mem region LENGTH
 * has been reduced and the reserved_last_page region has been added with a
 * LENGTH of 0x4000 (16 KB) as defined by NVM_FLASH_PAGESIZE.
 *
 * MEMORY
 * {
 *   kseg0_program_mem     (rx)  : ORIGIN = 0x9D000000, LENGTH = 0x7C000
 *   reserved_last_page    (rw)  : ORIGIN = 0x9D07C000, LENGTH = 0x4000
 *   ...
 * }
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "LastPage.h"
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Last page address.
 */
#define LAST_PAGE_ADDRESS (NVM_FLASH_START_ADDRESS + NVM_FLASH_SIZE - NVM_FLASH_PAGESIZE)

/**
 * @brief Print line length. Must be a multiple of NVM_FLASH_PAGESIZE.
 */
#define PRINT_LINE_LENGTH (32)

//------------------------------------------------------------------------------
// Variables

static uint8_t __attribute__((coherent)) buffer[NVM_FLASH_PAGESIZE];

//------------------------------------------------------------------------------
// Function declarations

static void PrintData(const uint8_t * const data);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Reads from last page of flash.
 * @param destination Destination.
 * @param numberOfBytes Number of bytes. Must not exceed NVM_FLASH_PAGESIZE.
 */
void LastPageRead(void* const destination, const size_t numberOfBytes) {
    NVM_Read(destination, numberOfBytes, LAST_PAGE_ADDRESS);
}

/**
 * @brief Writes to last page of flash.
 * @param data Data.
 * @param numberOfBytes Number of bytes. Must not exceed NVM_FLASH_PAGESIZE.
 */
void LastPageWrite(const void* const data, const size_t numberOfBytes) {
    LastPageErase();
    memset(buffer, 0xFF, sizeof (buffer));
    memcpy(buffer, data, numberOfBytes);
    for (size_t address = 0; address < NVM_FLASH_PAGESIZE; address += NVM_FLASH_ROWSIZE) {
        NVM_RowWrite((uint32_t *) (void*) &(buffer[address]), LAST_PAGE_ADDRESS + address);
        while (NVM_IsBusy() == true);
    }
}

/**
 * @brief Writes to the last page of flash only if the data being written is
 * different.
 * @param data Data.
 * @param numberOfBytes Number of bytes. Must not exceed NVM_FLASH_PAGESIZE.
 */
void LastPageUpdate(const void* const data, const size_t numberOfBytes) {
    LastPageRead(buffer, sizeof (buffer));
    if (memcmp(data, buffer, numberOfBytes) != 0) {
        LastPageWrite(data, numberOfBytes);
    }
}

/**
 * @brief Erases the last page of flash.
 */
void LastPageErase(void) {
    NVM_PageErase(LAST_PAGE_ADDRESS);
    while (NVM_IsBusy() == true);
}

/**
 * @brief Prints the last page of flash.
 */
void LastPagePrint(void) {
    LastPageRead(buffer, sizeof (buffer));
    bool printEllipses = true;
    for (uint32_t address = 0; address < NVM_FLASH_PAGESIZE; address += PRINT_LINE_LENGTH) {
        const bool isFirstLine = address == 0;
        const bool isLastLine = address == (NVM_FLASH_PAGESIZE - PRINT_LINE_LENGTH);
        const uint8_t blank[] = {[0 ... (PRINT_LINE_LENGTH - 1)] = 0xFF};
        const bool isNotBlank = memcmp(blank, &buffer[address], sizeof (blank)) != 0;

        // Print line with address
        if (isFirstLine || isLastLine || isNotBlank) {
            printf("%08X | ", LAST_PAGE_ADDRESS + address);
            PrintData(&buffer[address]);
            printEllipses = true;
            continue;
        }

        // Print line with ellipses
        if (printEllipses) {
            printf("...      | ");
            PrintData(&buffer[address]);
            printEllipses = false;
        }
    }
}

/**
 * @brief Prints data.
 * @param data Data.
 */
static void PrintData(const uint8_t * const data) {
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
