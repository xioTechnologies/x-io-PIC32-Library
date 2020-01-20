/**
 * @file StdioOverride.c
 * @author Seb Madgwick
 * @brief Overrides STDIO functions to redirect stdin and stdout.
 *
 * See section "2.13.2 Customizing STDIO" of "Standard C Libraries with Math
 * Functions".
 */

//------------------------------------------------------------------------------
// Includes

#include <stdint.h>
#include "Uart/Uart2.h"

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Writes a character to stdout.
 * @param c Character to be written.
 */
void _mon_putc(char c) {
    if (Uart2GetWriteAvailable() > 0) {
        Uart2WriteByte(c);
    }
}

/**
 * @brief Reads the next character from stdin.
 * @param canblock Non-zero to indicate that the function should block.
 * @return Returns the next character from the FILE associated with stdin. -1 is
 * returned to indicate end-of-file.
 */
int _mon_getc(int canblock) {
    do {
        if (Uart2GetReadAvailable() > 0) {
            const uint8_t byte = Uart2ReadByte();
            if (byte == '\r') {
                return -1;
            } else {
                return byte;
            }
        }
    } while (canblock != 0);
    return -1;
}

//------------------------------------------------------------------------------
// End of file
