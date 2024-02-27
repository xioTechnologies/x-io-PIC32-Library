/**
 * @file Lambda.h
 * @author Seb Madgwick
 * @brief Macro for lambda functions.
 */

#ifndef LAMBDA_H
#define LAMBDA_H

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Macro for lambda functions.
 * @param returnType Return type.
 * @param ... Function parameter list and function body.
 *
 * Example:
 * @code
 * void (*callback)(const int x);
 *
 * callback = LAMBDA(void, (const int x) {
 *     const int xSquared = x * x;
 *     printf("%i", xSquared);
 * });
 *
 * callback(5);
 * @endcode
 */
#define LAMBDA(returnType, ...) ({ \
    returnType Function __VA_ARGS__ \
    Function; \
})

#endif

//------------------------------------------------------------------------------
// End of file
