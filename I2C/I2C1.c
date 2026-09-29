/**
 * @file I2C1.c
 * @author Seb Madgwick
 * @brief I2C driver for PIC32 devices.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "I2C1.h"
#include <stddef.h>
#include "Timer/Timer.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Uncomment this line to print messages.
 */
//#define PRINT_MESSAGES

//------------------------------------------------------------------------------
// Function declarations

static inline __attribute__((always_inline)) bool Transmit(const uint8_t byte);
static inline __attribute__((always_inline)) uint8_t Receive(void);
static void WaitForInterruptOrTimeout(void);

//------------------------------------------------------------------------------
// Variables

const I2C i2c1 = {
    .start = I2C1Start,
    .repeatedStart = I2C1RepeatedStart,
    .stop = I2C1Stop,
    .transmit = I2C1Transmit,
    .transmitAddressRead = I2C1TransmitAddressRead,
    .transmitAddressWrite = I2C1TransmitAddressWrite,
    .receiveAck = I2C1ReceiveAck,
    .receiveNack = I2C1ReceiveNack,
};

static int lock;
static const I2CInstruction* instructions;
static int numberOfInstructions;
static int instructionsIndex;
static void (*transferComplete)(void);
static uint64_t transferTimeout;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the module.
 * @param clockFrequency Clock frequency.
 */
void I2C1Initialise(const I2CClockFrequency clockFrequency) {

    // Ensure default register states
    I2C1Deinitialise();

    // Configure I2C
    I2C1BRG = I2CCalculateI2Cxbrg(clockFrequency);
    if (clockFrequency != I2CClockFrequency400kHz) {
        I2C1CONbits.DISSLW = 1; // slew rate control disabled
    }
    I2C1CONbits.I2CEN = 1;
}

/**
 * @brief Disables the module.
 */
void I2C1Deinitialise(void) {

    // Disable I2C and restore default register states
    I2C1CON = 0;
    I2C1STAT = 0;

    // Disable interrupt
    EVIC_SourceDisable(INT_SOURCE_I2C1_MASTER);
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
}

/**
 * @brief Generates a start condition.
 */
void I2C1Start(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.SEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintStart();
#endif
}

/**
 * @brief Generates a repeated start condition.
 */
void I2C1RepeatedStart(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.RSEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintRepeatedStart();
#endif
}

/**
 * @brief Generates a stop condition.
 */
void I2C1Stop(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.PEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintStop();
#endif
}

/**
 * @brief Transmits a byte and checks for an ACK.
 * @param byte Byte.
 * @return True if an ACK was received.
 */
bool I2C1Transmit(const uint8_t byte) {
    const bool ack = Transmit(byte);
#ifdef PRINT_MESSAGES
    I2CPrintByte(byte);
    if (ack) {
        I2CPrintAck();
    } else {
        I2CPrintNack();
    }
#endif
    return ack;
}

/**
 * @brief Transmits a 7-bit client address with appended R/W bit to indicate a
 * read.
 * @param address 7-bit client address.
 * @return True if an ACK was received.
 */
bool I2C1TransmitAddressRead(const uint8_t address) {
    const bool ack = Transmit(I2CAddressRead(address));
#ifdef PRINT_MESSAGES
    I2CPrintReadAddress(address);
    if (ack) {
        I2CPrintAck();
    } else {
        I2CPrintNack();
    }
#endif
    return ack;
}

/**
 * @brief Transmits a 7-bit client address with appended R/W bit to indicate a
 * write.
 * @param address 7-bit client address.
 * @return True if an ACK was received.
 */
bool I2C1TransmitAddressWrite(const uint8_t address) {
    const bool ack = Transmit(I2CAddressWrite(address));
#ifdef PRINT_MESSAGES
    I2CPrintWriteAddress(address);
    if (ack) {
        I2CPrintAck();
    } else {
        I2CPrintNack();
    }
#endif
    return ack;
}

/**
 * @brief Transmits a byte and checks for an ACK.
 * @param byte Byte.
 * @return True if an ACK was received.
 */
static inline __attribute__((always_inline)) bool Transmit(const uint8_t byte) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1TRN = byte;
    WaitForInterruptOrTimeout();
    return I2C1STATbits.ACKSTAT == 0;
}

/**
 * @brief Receives a byte and generates an ACK.
 * @return Byte.
 */
uint8_t I2C1ReceiveAck(void) {
    const uint8_t byte = Receive();
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.ACKDT = 0;
    I2C1CONbits.ACKEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintByte(byte);
    I2CPrintAck();
#endif
    return byte;
}

/**
 * @brief Receives a byte and generates a NACK.
 * @return Byte.
 */
uint8_t I2C1ReceiveNack(void) {
    const uint8_t byte = Receive();
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.ACKDT = 1;
    I2C1CONbits.ACKEN = 1;
    WaitForInterruptOrTimeout();
#ifdef PRINT_MESSAGES
    I2CPrintByte(byte);
    I2CPrintNack();
#endif
    return byte;
}

/**
 * @brief Receives a byte.
 * @return Byte.
 */
static inline __attribute__((always_inline)) uint8_t Receive(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    I2C1CONbits.RCEN = 1;
    WaitForInterruptOrTimeout();
    return I2C1RCV;
}

/**
 * @brief Waits for the interrupt or timeout.
 */
static void WaitForInterruptOrTimeout(void) {
    const uint64_t timeout = TimerGetTicks64() + I2C_TIMEOUT;
    while (true) {
        if (EVIC_SourceStatusGet(INT_SOURCE_I2C1_MASTER)) {
            break;
        }
        if (TimerGetTicks64() > timeout) {
            break;
        }
    }
}

/**
 * @brief Acquires the lock and transfers data. The transfer will only begin
 * if the lock was acquired. The lock will be released when the transfer
 * completes or a timeout occurs. The transfer complete callback will be called
 * from within an interrupt once the transfer is complete.
 * @param instructions_ Instructions.
 * @param numberOfInstructions_ Number of instructions.
 * @param transferComplete_ Transfer complete callback. NULL if unused.
 * @return True if the lock was acquired.
 */
bool I2C1LockThenTransfer(const I2CInstruction * const instructions_, const int numberOfInstructions_, void (*const transferComplete_) (void)) {

    // Attempt to acquire lock
    if (I2C1Lock() == false) {
        return false;
    }

    // Configure transfer
    instructions = instructions_;
    numberOfInstructions = numberOfInstructions_;
    instructionsIndex = 0;
    transferComplete = transferComplete_;
    transferTimeout = TimerGetTicks64() + (uint64_t) (numberOfInstructions * I2C_TIMEOUT);

    // Trigger first interrupt
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);
    EVIC_SourceEnable(INT_SOURCE_I2C1_MASTER);
    EVIC_SourceStatusSet(INT_SOURCE_I2C1_MASTER);
    return true;
}

/**
 * @brief I2C master interrupt handler. This function should be called by the
 * ISR implementation generated by MPLAB Harmony.
 */
void I2C1MasterInterruptHandler(void) {
    EVIC_SourceStatusClear(INT_SOURCE_I2C1_MASTER);

    // Print received ACK or NACK
#ifdef PRINT_MESSAGES
    if (instructionsIndex > 0) {
        switch (instructions[instructionsIndex - 1].operation) {
            case I2COperationTransmit:
            case I2COperationTransmitAddressRead:
            case I2COperationTransmitAddressWrite:
                if (I2C1STATbits.ACKSTAT == 0) {
                    I2CPrintAck();
                } else {
                    I2CPrintNack();
                }
                break;
            default:
                break;
        }
    }
#endif

    // End of transfer
    if (instructionsIndex >= numberOfInstructions) {
        EVIC_SourceDisable(INT_SOURCE_I2C1_MASTER);
        I2C1Unlock();
        if (transferComplete != NULL) {
            transferComplete();
        }
        return;
    }

    // Initiate operation
    switch (instructions[instructionsIndex].operation) {
        case I2COperationStart:
            I2C1CONbits.SEN = 1;
            break;
        case I2COperationRepeatedStart:
            I2C1CONbits.RSEN = 1;
            break;
        case I2COperationStop:
            I2C1CONbits.PEN = 1;
            break;
        case I2COperationTransmit:
            I2C1TRN = instructions[instructionsIndex].data;
            break;
        case I2COperationTransmitAddressRead:
            I2C1TRN = I2CAddressRead(instructions[instructionsIndex].address);
            break;
        case I2COperationTransmitAddressWrite:
            I2C1TRN = I2CAddressWrite(instructions[instructionsIndex].address);
            break;
        case I2COperationReceive:
            I2C1CONbits.RCEN = 1;
            break;
        case I2COperationAck:
            *instructions[instructionsIndex].destination = I2C1RCV;
            I2C1CONbits.ACKDT = 0;
            I2C1CONbits.ACKEN = 1;
            break;
        case I2COperationNack:
            *instructions[instructionsIndex].destination = I2C1RCV;
            I2C1CONbits.ACKDT = 1;
            I2C1CONbits.ACKEN = 1;
            break;
    }

    // Print operation
#ifdef PRINT_MESSAGES
    switch (instructions[instructionsIndex].operation) {
        case I2COperationStart:
            I2CPrintStart();
            break;
        case I2COperationRepeatedStart:
            I2CPrintRepeatedStart();
            break;
        case I2COperationStop:
            I2CPrintStop();
            break;
        case I2COperationTransmit:
            I2CPrintByte(instructions[instructionsIndex].data);
            break;
        case I2COperationTransmitAddressRead:
            I2CPrintReadAddress(instructions[instructionsIndex].address);
            break;
        case I2COperationTransmitAddressWrite:
            I2CPrintWriteAddress(instructions[instructionsIndex].address);
            break;
        case I2COperationReceive:
            break;
        case I2COperationAck:
            I2CPrintByte(*instructions[instructionsIndex].destination);
            I2CPrintAck();
            break;
        case I2COperationNack:
            I2CPrintByte(*instructions[instructionsIndex].destination);
            I2CPrintNack();
            break;
    }
#endif

    // Increment index
    instructionsIndex++;
}

/**
 * @brief Attempts to acquire the lock.
 * @return True if the lock was acquired.
 */
bool I2C1Lock(void) {
    if (EVIC_SourceIsEnabled(INT_SOURCE_I2C1_MASTER) && (TimerGetTicks64() > transferTimeout)) {
        EVIC_SourceDisable(INT_SOURCE_I2C1_MASTER);
        I2C1Unlock();
    }
    return __sync_lock_test_and_set(&lock, 1) == 0;
}

/**
 * @brief Releases the lock.
 */
void I2C1Unlock(void) {
    __sync_lock_release(&lock);
}

/**
 * @brief Returns true if locked.
 * @return True if locked.
 */
bool I2C1Locked(void) {
    if (I2C1Lock() == false) {
        return true;
    }
    I2C1Unlock();
    return false;
}

//------------------------------------------------------------------------------
// End of file
