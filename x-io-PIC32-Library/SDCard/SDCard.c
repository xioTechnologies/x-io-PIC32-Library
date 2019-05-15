/**
 * @file SDCard.c
 * @author Seb Madgwick
 * @brief Application interface for SD card functionality using MPLAB Harmony.
 */

//------------------------------------------------------------------------------
// Includes

#include "SDCard.h"
#include <stdio.h> // printf, snprintf
#include <string.h> // memset, strlen, strncpy
#include "system_definitions.h"

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Mount name required by SYS_FS_Mount and SYS_FS_Unmount.
 */
#define MOUNT_NAME "/mnt/myDrive"

/**
 * @brief Comment out this definition to disable printing of file system errors.
 */
#define PRINT_FILE_SYSTEM_ERRORS

/**
 * @brief State.
 */
typedef enum {
    StateMounting,
    StateMounted,
    StateUnmounted,
} State;

//------------------------------------------------------------------------------
// Function prototypes

static void MountingTasks();
static void MountedTasks();
static void PrintFileSystemError(const char* functionName, const SYS_FS_ERROR sysFSError);

//------------------------------------------------------------------------------
// Variables

static State state = StateUnmounted;
static SYS_FS_HANDLE fileHandle = SYS_FS_HANDLE_INVALID;
static SYS_FS_HANDLE directoryHandle = SYS_FS_HANDLE_INVALID;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Module tasks.  This function should be called repeatedly within the
 * main program loop.
 */
void SDCardTasks() {
    switch (state) {
        case StateMounting:
            MountingTasks();
            break;
        case StateMounted:
            MountedTasks();
            break;
        case StateUnmounted:
            break;
    }
}

/**
 * @brief Mounting tasks.
 */
static void MountingTasks() {

    // Do nothing if SD card not attached
    if (DRV_SDCARD_IsAttached(DRV_SDCARD_INDEX_0) == false) {
        return;
    }

    // Mount SD card
    if (SYS_FS_Mount("/dev/mmcblka1", MOUNT_NAME, FAT, 0, NULL) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError("SYS_FS_Mount", SYS_FS_Error());
        return;
    }
    state = StateMounted;
}

/**
 * @brief Mounted tasks.
 */
static void MountedTasks() {
    if (DRV_SDCARD_IsAttached(DRV_SDCARD_INDEX_0) == false) {
        SDCardUnmount();
        SDCardMount();
    }
}

/**
 * @brief Mounts the SD card.
 */
void SDCardMount() {
    switch (state) {
        case StateMounting:
        case StateMounted:
            return;
        case StateUnmounted:
            state = StateMounting;
            break;
    }
}

/**
 * @brief Unmounts the SD card.
 */
void SDCardUnmount() {
    switch (state) {
        case StateMounting:
            state = StateUnmounted;
            return;
        case StateMounted:
            if (SYS_FS_Unmount(MOUNT_NAME) != SYS_FS_RES_SUCCESS) {
                PrintFileSystemError("SYS_FS_Mount", SYS_FS_Error());
            }
            state = StateUnmounted;
            break;
        case StateUnmounted:
            return;
    }
}

/**
 * @brief Returns true if the SD card is mounted.
 * @return True if the SD card is mounted.
 */
bool SDCardIsMounted() {
    return state == StateMounted;
}

/**
 * @brief Opens a file in either read or write mode.  Opening a file in write
 * mode will overwrite the file if it already exists.
 * @param fileName File name.
 * @param writeMode True to open a file in write mode.
 * @return SD card error.
 */
SDCardError SDCardFileOpen(const char* const fileName, const bool writeMode) {

    // Check if card full
    if (writeMode == true) {
        uint32_t totalSectors;
        uint32_t freeSectors;
        if (SYS_FS_DriveSectorGet(MOUNT_NAME, &totalSectors, &freeSectors) != SYS_FS_RES_SUCCESS) {
            PrintFileSystemError("SYS_FS_DriveSectorGet", SYS_FS_Error());
            return SDCardErrorFileSystemError;
        }
        if (freeSectors == 0) {
            return SDCardErrorFileOrSDCardFull;
        }
    }

    // Open file
    fileHandle = SYS_FS_FileOpen(fileName, (writeMode == true) ? SYS_FS_FILE_OPEN_WRITE : SYS_FS_FILE_OPEN_READ);
    if (fileHandle == SYS_FS_HANDLE_INVALID) {
        PrintFileSystemError("SYS_FS_FileOpen", SYS_FS_Error());
        return SDCardErrorFileSystemError;
    }
    return SDCardErrorOK;
}

/**
 * @brief Reads data from the file.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @return Number of bytes read.
 */
size_t SDCardFileRead(void* const destination, const size_t destinationSize) {
    const size_t numberOfBytesRead = SYS_FS_FileRead(fileHandle, destination, destinationSize);
    if (numberOfBytesRead == -1) {
        PrintFileSystemError("SYS_FS_FileRead", SYS_FS_FileError(fileHandle));
        return 0;
    }
    return numberOfBytesRead;
}

/**
 * @brief Reads a string from the file.  This function will read until a '\n' is
 * read or the end of file is reached.
 * @param destination Destination.
 * @param destinationSize Destination size.
 */
void SDCardFileReadString(void* const destination, const size_t destinationSize) {
    if (SYS_FS_FileStringGet(fileHandle, destination, destinationSize) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError("SYS_FS_FileStringGet", SYS_FS_FileError(fileHandle));
        strncpy(destination, "", destinationSize);
    }
}

/**
 * @brief Writes data to the file.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return SD card error.
 */
SDCardError SDCardFileWrite(const void* const data, const size_t numberOfBytes) {

    // Write data
    const size_t numberOfBytesWritten = SYS_FS_FileWrite(fileHandle, data, numberOfBytes);
    if (numberOfBytesWritten == -1) {
        PrintFileSystemError("SYS_FS_FileWrite", SYS_FS_FileError(fileHandle));
        return SDCardErrorFileSystemError;
    }

    // File or SD card full
    if (numberOfBytesWritten != numberOfBytes) {
        return SDCardErrorFileOrSDCardFull;
    }
    return SDCardErrorOK;
}

/**
 * @brief Writes a string to the file.  The terminating null character will not
 * be written.
 * @param sring String.
 * @return SD card error.
 */
SDCardError SDCardFileWriteString(const char* const string) {
    return SDCardFileWrite(string, strlen(string));
}

/**
 * @brief Closes the file.
 */
void SDCardFileClose() {
    if (SYS_FS_FileClose(fileHandle) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError("SYS_FS_FileClose", SYS_FS_FileError(fileHandle));
    }
}

/**
 * @brief Renames a file.  The file must be closed before it is renamed.
 * @param fileName File name.
 * @param newFileName New file name.
 */
void SDCardFileRename(const char* const fileName, const char* const newFileName) {
    if (SYS_FS_FileDirectoryRenameMove(fileName, newFileName) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError("SYS_FS_FileDirectoryRenameMove", SYS_FS_Error());
    }
}

/**
 * @brief Opens the root directory.
 */
void SDCardDirectoryOpen() {
    directoryHandle = SYS_FS_DirOpen(MOUNT_NAME);
    if (directoryHandle == SYS_FS_HANDLE_INVALID) {
        PrintFileSystemError("SYS_FS_DirOpen", SYS_FS_Error());
    }
}

/**
 * @brief Searches the root directory for files matching the file name.  The
 * file name can include wild cards (e.g. "*.*" to match all files).  If
 * multiple files match the file name then this function can be called in a loop
 * to retrieve the file details for each file.  If no files match the file name
 * then the file name provided in the file details will be an empty string.
 * The root directory must be closed and reopened for each new search.
 * @param fileName File name.
 * @param sdCardFileDetails SD card file details.
 */
void SDCardDirectorySearch(const char* const fileName, SDCardFileDetails * const sdCardFileDetails) {

    // Get search result
    SYS_FS_FSTAT sysFSFstat;
    char longFileName[SD_CARD_MAX_FILE_NAME_SIZE];
    sysFSFstat.lfname = longFileName;
    sysFSFstat.lfsize = sizeof (longFileName);
    if (SYS_FS_DirSearch(directoryHandle, fileName, SYS_FS_ATTR_MASK, &sysFSFstat) == SYS_FS_RES_FAILURE) {
        const SYS_FS_ERROR sysFSError = SYS_FS_Error();
        if (sysFSError != SYS_FS_ERROR_NO_FILE) {
            PrintFileSystemError("SYS_FS_DirSearch", sysFSError);
        }
        memset(sdCardFileDetails, 0, sizeof (SDCardFileDetails));
        return;
    }

    // Copy file size
    sdCardFileDetails->size = sysFSFstat.fsize;

    // Parse time
    SYS_FS_TIME sysFSTime;
    sysFSTime.timeDate.time = sysFSFstat.ftime;
    sysFSTime.timeDate.date = sysFSFstat.fdate;
    sdCardFileDetails->time.second = sysFSTime.discreteTime.second;
    sdCardFileDetails->time.minute = sysFSTime.discreteTime.minute;
    sdCardFileDetails->time.hour = sysFSTime.discreteTime.hour;
    sdCardFileDetails->time.day = sysFSTime.discreteTime.day;
    sdCardFileDetails->time.month = sysFSTime.discreteTime.month;
    sdCardFileDetails->time.year = 1980 + sysFSTime.discreteTime.year;

    // Copy file name
    if (strlen(sysFSFstat.lfname) == 0) {
        strncpy(sdCardFileDetails->name, sysFSFstat.fname, sizeof (sdCardFileDetails->name));
    } else {
        strncpy(sdCardFileDetails->name, sysFSFstat.lfname, sizeof (sdCardFileDetails->name));
    }
}

/**
 * @brief Returns true if any files matching the file name exist in the root
 * directory.  The file name can include wild cards (e.g. "*.txt" to match all
 * text files).  This function is optimised to be called in a loop to check the
 * exitance of multiple files that were written in the same order as being
 * checked by the loop.
 * @param fileName File name.
 * @return True if any files matching the file name exist.
 */
bool SDCardDirectoryExists(const char* const fileName) {

    // Search from unknown starting position in directory
    SDCardFileDetails sdCardFileDetails;
    SDCardDirectorySearch(fileName, &sdCardFileDetails);
    if (strlen(sdCardFileDetails.name) > 0) {
        return true;
    }

    // Repeat search from start of directory
    if (SYS_FS_DirRewind(directoryHandle) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError("SYS_FS_DirRewind", SYS_FS_FileError(directoryHandle));
        return false;
    }
    SDCardDirectorySearch(fileName, &sdCardFileDetails);
    if (strlen(sdCardFileDetails.name) > 0) {
        return true;
    }
    return false;
}

/**
 * @brief Closes the root directory.
 */
void SDCardDirectoryClose() {
    if (SYS_FS_DirClose(directoryHandle) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError("SYS_FS_DirClose", SYS_FS_FileError(directoryHandle));
    }
}

/**
 * @brief Prints file system error.
 * @param sysFSError File system error.
 */
static void PrintFileSystemError(const char* functionName, const SYS_FS_ERROR sysFSError) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
    char* error = (char*) &"";
    switch (sysFSError) {
        case SYS_FS_ERROR_OK:
            error = (char*) &"SYS_FS_ERROR_OK";
            break;
        case SYS_FS_ERROR_DISK_ERR:
            error = (char*) &"SYS_FS_ERROR_DISK_ERR";
            break;
        case SYS_FS_ERROR_INT_ERR:
            error = (char*) &"SYS_FS_ERROR_INT_ERR";
            break;
        case SYS_FS_ERROR_NOT_READY:
            error = (char*) &"SYS_FS_ERROR_NOT_READY";
            break;
        case SYS_FS_ERROR_NO_FILE:
            error = (char*) &"SYS_FS_ERROR_NO_FILE";
            break;
        case SYS_FS_ERROR_NO_PATH:
            error = (char*) &"SYS_FS_ERROR_NO_PATH";
            break;
        case SYS_FS_ERROR_INVALID_NAME:
            error = (char*) &"SYS_FS_ERROR_INVALID_NAME";
            break;
        case SYS_FS_ERROR_DENIED:
            error = (char*) &"SYS_FS_ERROR_DENIED";
            break;
        case SYS_FS_ERROR_EXIST:
            error = (char*) &"SYS_FS_ERROR_EXIST";
            break;
        case SYS_FS_ERROR_INVALID_OBJECT:
            error = (char*) &"SYS_FS_ERROR_INVALID_OBJECT";
            break;
        case SYS_FS_ERROR_WRITE_PROTECTED:
            error = (char*) &"SYS_FS_ERROR_WRITE_PROTECTED";
            break;
        case SYS_FS_ERROR_INVALID_DRIVE:
            error = (char*) &"SYS_FS_ERROR_INVALID_DRIVE";
            break;
        case SYS_FS_ERROR_NOT_ENABLED:
            error = (char*) &"SYS_FS_ERROR_NOT_ENABLED";
            break;
        case SYS_FS_ERROR_NO_FILESYSTEM:
            error = (char*) &"SYS_FS_ERROR_NO_FILESYSTEM";
            break;
        case SYS_FS_ERROR_FORMAT_ABORTED:
            error = (char*) &"SYS_FS_ERROR_FORMAT_ABORTED";
            break;
        case SYS_FS_ERROR_TIMEOUT:
            error = (char*) &"SYS_FS_ERROR_TIMEOUT";
            break;
        case SYS_FS_ERROR_LOCKED:
            error = (char*) &"SYS_FS_ERROR_LOCKED";
            break;
        case SYS_FS_ERROR_NOT_ENOUGH_CORE:
            error = (char*) &"SYS_FS_ERROR_NOT_ENOUGH_CORE";
            break;
        case SYS_FS_ERROR_TOO_MANY_OPEN_FILES:
            error = (char*) &"SYS_FS_ERROR_TOO_MANY_OPEN_FILES";
            break;
        case SYS_FS_ERROR_INVALID_PARAMETER:
            error = (char*) &"SYS_FS_ERROR_INVALID_PARAMETER";
            break;
        case SYS_FS_ERROR_NOT_ENOUGH_FREE_VOLUME:
            error = (char*) &"SYS_FS_ERROR_NOT_ENOUGH_FREE_VOLUME";
            break;
        case SYS_FS_ERROR_FS_NOT_SUPPORTED:
            error = (char*) &"SYS_FS_ERROR_FS_NOT_SUPPORTED";
            break;
        case SYS_FS_ERROR_FS_NOT_MATCH_WITH_VOLUME:
            error = (char*) &"SYS_FS_ERROR_FS_NOT_MATCH_WITH_VOLUME";
            break;
        case SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS:
            error = (char*) &"SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS";
            break;
    }
    printf("%s: %s\r\n", functionName, error);
#endif
}

//------------------------------------------------------------------------------
// End of file
