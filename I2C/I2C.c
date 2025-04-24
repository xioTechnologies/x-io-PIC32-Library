/**
 * @file I2C.c
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "I2C.h"
#include "PeripheralBusClockFrequency.h"
#include <stdio.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief I2C peripheral clock frequency.
 */
#if (defined __PIC32MX__) || (defined __PIC32MM__)
#define I2C_PERIPHERAL_CLOCK (PERIPHERAL_BUS_CLOCK_FREQUENCY)
#elif defined __PIC32MZ__
#define I2C_PERIPHERAL_CLOCK (PERIPHERAL_BUS_CLOCK_2_FREQUENCY)
#else
#error "Unsupported device."
#endif

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Calculates the I2CXBRG value for a target clock frequency.
 * See page 19 of Section 24. Inter-Integrated Circuit.
 * @param fsk Clock frequency in Hz.
 * @return I2CXBRG value.
 */
uint32_t I2CCalculateI2Cxbrg(const uint32_t fsk) {
    return (uint32_t) ((((1.0f / (2.0f * (float) fsk)) - 0.000000104f) * (float) I2C_PERIPHERAL_CLOCK - 2.0f) + 0.5f);
}

/**
 * @brief Appends the R/W bit on a 7-bit I2C client address to indicate a write.
 * @param address 7-bit client address.
 * @return 7-bit client address with appended R/W bit.
 */
uint8_t I2CAddressRead(const uint8_t address) {
    return (address << 1) | 1;
}

/**
 * @brief Appends the R/W bit on a 7-bit I2C client address to indicate a read.
 * @param address 7-bit client address.
 * @return 7-bit client address with appended R/W bit.
 */
uint8_t I2CAddressWrite(const uint8_t address) {
    return (address << 1) | 0;
}

/**
 * @brief Print start event.
 */
void I2CPrintStart(void) {
    printf("S ");
}

/**
 * @brief Print repeated start event.
 */
void I2CPrintRepeatedStart(void) {
    printf("R ");
}

/**
 * @brief Print stop event.
 */
void I2CPrintStop(void) {
    printf("P\n");
}

/**
 * @brief Print byte.
 * @param byte Byte.
 */
void I2CPrintByte(const uint8_t byte) {
    printf("%02X", byte);
}

/**
 * @brief Print 7-bit client address used to read.
 * @param address 7-bit client address.
 */
void I2CPrintReadAddress(const uint8_t address) {
    printf("r%02X", address);
}

/**
 * @brief Print 7-bit client address used to write.
 * @param address 7-bit client address.
 */
void I2CPrintWriteAddress(const uint8_t address) {
    printf("w%02X", address);
}

/**
 * @brief Print ACK or NACK.
 * @param ack True for ACK.
 */
void I2CPrintAckNack(const bool ack) {
    printf("%c ", ack ? '-' : '^');
}

//------------------------------------------------------------------------------
// End of file
