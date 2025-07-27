/**
 * @file I2CBBConfig.h
 * @author Seb Madgwick
 * @brief I2C bit-bang driver.
 */

#ifndef I2CBB_CONFIG_H
#define I2CBB_CONFIG_H

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"

//------------------------------------------------------------------------------
// Definitions

#define I2CBB1_SCL_PIN              SCL2_PIN
#define I2CBB1_SDA_PIN              SDA2_PIN
#define I2CBB1_HALF_CLOCK_CYCLE     (5)

#define I2CBB2_SCL_PIN              SCL2_PIN
#define I2CBB2_SDA_PIN              SDA2_PIN
#define I2CBB2_HALF_CLOCK_CYCLE     (5)

#endif

//------------------------------------------------------------------------------
// End of file
