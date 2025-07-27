/**
 * @file I2C.h
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

#ifndef I2C_H
#define I2C_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stdint.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Clock frequency.
 */
typedef enum {
    I2CClockFrequency100kHz = 100000,
    I2CClockFrequency400kHz = 400000,
    I2CClockFrequency1MHz = 1000000,
} I2CClockFrequency;

/**
 * @brief Timeout in timer ticks. Equal to 10 clock cycles for the slowest clock
 * frequency.
 */
#define I2C_TIMEOUT (TIMER_TICKS_PER_SECOND / (I2CClockFrequency100kHz / 10))

/**
 * @brief I2C interface.
 */
typedef struct {
    void (*start)(void);
    void (*repeatedStart)(void);
    void (*stop)(void);
    bool (*send)(const uint8_t byte);
    bool (*sendAddressRead)(const uint8_t address);
    bool (*sendAddressWrite)(const uint8_t address);
    uint8_t(*receive)(const bool ack);
} I2C;

//------------------------------------------------------------------------------
// Function declarations

uint32_t I2CCalculateI2Cxbrg(const uint32_t fsk);
uint8_t I2CAddressRead(const uint8_t address);
uint8_t I2CAddressWrite(const uint8_t address);
void I2CPrintStart(void);
void I2CPrintRepeatedStart(void);
void I2CPrintStop(void);
void I2CPrintByte(const uint8_t byte);
void I2CPrintReadAddress(const uint8_t address);
void I2CPrintWriteAddress(const uint8_t address);
void I2CPrintAckNack(const bool ack);

#endif

//------------------------------------------------------------------------------
// End of file
