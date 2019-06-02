/**
 * @file SDCard.h
 * @author Seb Madgwick
 * @brief Application interface for SD card functionality using MPLAB Harmony.
 */

#ifndef SD_CARD_H
#define SD_CARD_H

//------------------------------------------------------------------------------
// Includes

#include <stdbool.h>
#include <stddef.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief SD card maximum file name size.
 */
#define SD_CARD_MAX_FILE_NAME_SIZE (128)

/**
 * @brief SD card error.
 */
typedef enum {
    SDCardErrorOK,
    SDCardErrorFileSystemError,
    SDCardErrorFileOrSDCardFull,
} SDCardError;

/**
 * @brief SD card file time.
 */
typedef struct {
    int second;
    int minute;
    int hour;
    int day;
    int month;
    int year;
} SDCardFileTime;

/**
 * @brief SD card file details.
 */
typedef struct {
    size_t size;
    SDCardFileTime time;
    char name[SD_CARD_MAX_FILE_NAME_SIZE];
} SDCardFileDetails;

//------------------------------------------------------------------------------
// Function prototypes

void SDCardTasks();
void SDCardMount();
void SDCardUnmount();
bool SDCardIsMounted();
SDCardError SDCardFileOpen(const char* const fileName, const bool writeMode);
size_t SDCardFileRead(void* const destination, const size_t destinationSize);
void SDCardFileReadString(void* const destination, const size_t destinationSize);
SDCardError SDCardFileWrite(const void* const data, const size_t numberOfBytes);
SDCardError SDCardFileWriteString(const char* const string);
size_t SDCardFileGetSize();
void SDCardFileClose();
void SDCardFileRename(const char* const fileName, const char* const newFileName);
void SDCardDirectoryOpen();
void SDCardDirectorySearch(const char* const fileName, SDCardFileDetails * const fileDetails);
bool SDCardDirectoryExists(const char* const fileName);
void SDCardDirectoryClose();

#endif

//------------------------------------------------------------------------------
// End of file
