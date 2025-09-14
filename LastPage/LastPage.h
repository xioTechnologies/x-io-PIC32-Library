/**
 * @file LastPage.h
 * @author Seb Madgwick
 * @brief Provides access to the last page of flash as NVM for application data.
 */

#ifndef LAST_PAGE_H
#define LAST_PAGE_H

//------------------------------------------------------------------------------
// Includes

#include <stddef.h>

//------------------------------------------------------------------------------
// Function declarations

void LastPageRead(void* const destination, const size_t numberOfBytes);
void LastPageWrite(const void* const data, const size_t numberOfBytes);
void LastPageUpdate(const void* const data, const size_t numberOfBytes);
void LastPageErase(void);
void LastPagePrint(void);

#endif

//------------------------------------------------------------------------------
// End of file
