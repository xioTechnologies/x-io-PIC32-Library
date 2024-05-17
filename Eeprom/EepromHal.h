/**
 * @file EepromHal.h
 * @author Seb Madgwick
 * @brief EEPROM driver Hardware Abstraction Layer (HAL).
 */

#ifndef EEPROM_HAL_H
#define EEPROM_HAL_H

//------------------------------------------------------------------------------
// Includes

#include "I2C/I2C1.h"
#include <stdbool.h>
#include <stdint.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief EEPROM I2C slave address.
 */
#define EEPROM_HAL_I2C_ADDRESS (0x50)

/**
 * @brief EEPROM size.
 */
#define EEPROM_HAL_SIZE (0x1000)

/**
 * @brief EEPROM page size.
 */
#define EEPROM_HAL_PAGE_SIZE (32)

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Performs the start condition.
 */
static inline __attribute__((always_inline)) void EepromHalI2CStart(void) {
    I2C1Start();
}

/**
 * @brief Performs the start condition.
 */
static inline __attribute__((always_inline)) void EepromHalI2CRepeatedStart(void) {
    I2C1RepeatedStart();
}

/**
 * @brief Performs the stop condition.
 */
static inline __attribute__((always_inline)) void EepromHalI2CStop(void) {
    I2C1Stop();
}

/**
 * @brief Sends a byte and checks for ACK.
 * @param byte Byte.
 * @return True if an ACK was generated.
 */
static inline __attribute__((always_inline)) bool EepromHalI2CSend(const uint8_t byte) {
    return I2C1Send(byte);
}

/**
 * @brief Receives a byte and generates an ACK or NACK.
 * @param ack True for ACK.
 * @return Byte.
 */
static inline __attribute__((always_inline)) uint8_t EepromHalI2CReceive(const bool ack) {
    return I2C1Receive(ack);
}

#endif

//------------------------------------------------------------------------------
// End of file
