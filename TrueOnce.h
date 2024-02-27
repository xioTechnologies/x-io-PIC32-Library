/**
 * @file TrueOnce.h
 * @author Seb Madgwick
 * @brief Macro that will return true once.
 */

#ifndef TRUE_ONCE_H
#define TRUE_ONCE_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Macro that will return true once.
 */
#define TRUE_ONCE() ({ \
    static bool state = true; \
    TrueOnce(&state); \
})

//------------------------------------------------------------------------------
// Inline functions

/**
 * @brief Returns true once.
 * @param state State must be initialised to true.
 * @return True if state is true.
 */
static inline __attribute__((always_inline)) bool TrueOnce(bool * const state) {
    if (*state) {
        *state = false;
        return true;
    }
    return false;
}

#endif

//------------------------------------------------------------------------------
// End of file
