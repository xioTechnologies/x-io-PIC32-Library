/**
 * @file SdCard.c
 * @author Seb Madgwick
 * @brief Application interface for SD card functionality using MPLAB Harmony.
 */

//------------------------------------------------------------------------------
// Includes

#include "definitions.h"
#include "SdCard.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Device name.
 */
#define DEV_NAME "/dev/mmcblka1"

/**
 * @brief Mount name.
 */
#define MOUNT_NAME "/mnt/myDrive"

/**
 * @brief Uncomment this line to enable printing of file system errors.
 */
//#define PRINT_FILE_SYSTEM_ERRORS

/**
 * @brief State.
 */
typedef enum {
    StateMounting,
    StateMounted,
    StateUnmounted,
} State;

//------------------------------------------------------------------------------
// Function declarations

static const char* ChangeDirectory(const char* const filePath, const bool createDirectory);
static void Concatenate(char* const destination, const size_t destinationSize, const char* const source);
#ifdef PRINT_FILE_SYSTEM_ERRORS
static void PrintFileSystemError(const char* functionName, const SYS_FS_ERROR sysFSError);
#endif

//------------------------------------------------------------------------------
// Variables

static State state = StateUnmounted;
static SYS_FS_HANDLE fileHandle = SYS_FS_HANDLE_INVALID;
static SYS_FS_HANDLE directoryHandle = SYS_FS_HANDLE_INVALID;

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Module tasks. This function should be called repeatedly within the
 * main program loop.
 */
void SdCardTasks(void) {
    switch (state) {
        case StateMounting:
            if (SYS_FS_MEDIA_MANAGER_MediaStatusGet(DEV_NAME) == false) {
                return;
            }
            if (SYS_FS_Mount(DEV_NAME, MOUNT_NAME, FAT, 0, NULL) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
                PrintFileSystemError("SYS_FS_Mount", SYS_FS_Error());
#endif
                return;
            }
            state = StateMounted;
            break;
        case StateMounted:
            if (SYS_FS_MEDIA_MANAGER_MediaStatusGet(DEV_NAME) == false) {
                SdCardUnmount();
                SdCardMount();
            }
            break;
        case StateUnmounted:
            break;
    }
}

/**
 * @brief Mounts the SD card.
 */
void SdCardMount(void) {
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
void SdCardUnmount(void) {
    switch (state) {
        case StateMounting:
            state = StateUnmounted;
            return;
        case StateMounted:
            if (SYS_FS_Unmount(MOUNT_NAME) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
                PrintFileSystemError("SYS_FS_Unmount", SYS_FS_Error());
#endif
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
bool SdCardMounted(void) {
    return state == StateMounted;
}

/**
 * @brief Formats the SD card.
 */
void SdCardFormat(void) {
    const SYS_FS_FORMAT_PARAM opt = (SYS_FS_FORMAT_PARAM){
        .fmt = SYS_FS_FORMAT_FAT32,
        .n_fat = 1,
        .align = 0,
        .n_root = 1,
        .au_size = 0
    };
    static uint8_t CACHE_ALIGN work[512];
    if (SYS_FS_DriveFormat(MOUNT_NAME, &opt, work, sizeof (work)) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_DriveFormat", SYS_FS_Error());
#endif
    }
}

/**
 * @brief Sets the volume label.
 * @param label Label.
 */
void SdCardSetVolumeLabel(const char* const label) {
    if (SYS_FS_DriveLabelSet(MOUNT_NAME, label) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_DriveLabelSet", SYS_FS_Error());
#endif
    }
}

/**
 * @brief Opens a file. Writing a file that already exists will overwrite the
 * existing file.
 * @param fileName File name.
 * @param write True to write a file.
 * @return Result.
 */
SdCardResult SdCardFileOpen(const char* const filePath, const bool write) {

    // Change directory
    const char* const fileName = ChangeDirectory(filePath, write);
    if (fileName == NULL) {
        return SdCardResultFileSystemError;
    }

    // Check if card full
    if (write) {
        uint32_t totalSectors;
        uint32_t freeSectors;
        if (SYS_FS_DriveSectorGet(MOUNT_NAME, &totalSectors, &freeSectors) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
            PrintFileSystemError("SYS_FS_DriveSectorGet", SYS_FS_Error());
#endif
            return SdCardResultFileSystemError;
        }
        if (freeSectors == 0) {
            return SdCardResultFileOrSdCardFull;
        }
    }

    // Open file
    fileHandle = SYS_FS_FileOpen(fileName, write ? SYS_FS_FILE_OPEN_WRITE : SYS_FS_FILE_OPEN_READ);
    if (fileHandle == SYS_FS_HANDLE_INVALID) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_FileOpen", SYS_FS_Error());
#endif
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Changes the directory to that of the file path.
 * @param filePath File path.
 * @param createDirectory True to create the directory.
 * @return File name. NULL if a file system error occurred.
 */
static const char* ChangeDirectory(const char* const filePath, const bool createDirectory) {

    // Change to root directory
    if (SYS_FS_DirectoryChange("/") != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_DirectoryChange", SYS_FS_Error());
#endif
        return NULL;
    }

    // Split file name and directory
    const char* const directory = SdCardPathSplitDirectory(filePath);
    const char* const fileName = SdCardPathSplitFileName(filePath);
    if (strlen(directory) == 0) {
        return fileName;
    }

    // Change directory
    if (createDirectory == false) {
        if (SYS_FS_DirectoryChange(directory) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
            PrintFileSystemError("SYS_FS_DirectoryChange", SYS_FS_Error());
#endif
            return NULL;
        }
        return fileName;
    }

    // Create directory
    char incrementingPath[SD_CARD_MAX_PATH_SIZE] = "";
    char tokenizedPath[SD_CARD_MAX_PATH_SIZE];
    snprintf(tokenizedPath, sizeof (tokenizedPath), "%s", directory);
    const char* directoryName = strtok(tokenizedPath, "/");
    while (directoryName != NULL) {
        if ((SYS_FS_DirectoryMake(directoryName) != SYS_FS_RES_SUCCESS) && (SYS_FS_Error() != SYS_FS_ERROR_EXIST)) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
            PrintFileSystemError("SYS_FS_DirectoryMake", SYS_FS_Error());
#endif
            return NULL;
        }
        snprintf(incrementingPath, sizeof (incrementingPath), "%s", SdCardPathJoin(2, incrementingPath, directoryName));
        if (SYS_FS_DirectoryChange(incrementingPath) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
            PrintFileSystemError("SYS_FS_DirectoryChange", SYS_FS_Error());
#endif
            return NULL;
        }
        directoryName = strtok(NULL, "/");
    }
    return fileName;
}

/**
 * @brief Reads data from the file.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @return Number of bytes read.
 */
size_t SdCardFileRead(void* const destination, const size_t destinationSize) {
    const size_t numberOfBytesRead = SYS_FS_FileRead(fileHandle, destination, destinationSize);
    if (numberOfBytesRead == -1) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_FileRead", SYS_FS_FileError(fileHandle));
#endif
        return 0;
    }
    return numberOfBytesRead;
}

/**
 * @brief Reads a string from the file. This function will read until a '\n' is
 * read or the end of file is reached.
 * @param destination Destination.
 * @param destinationSize Destination size.
 */
void SdCardFileReadString(void* const destination, const size_t destinationSize) {
    if (SYS_FS_FileStringGet(fileHandle, destination, destinationSize) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_FileStringGet", SYS_FS_FileError(fileHandle));
#endif
        strncpy(destination, "", destinationSize);
    }
}

/**
 * @brief Writes data to the file.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return Result.
 */
SdCardResult SdCardFileWrite(const void* const data, const size_t numberOfBytes) {

    // Write data
    const size_t numberOfBytesWritten = SYS_FS_FileWrite(fileHandle, data, numberOfBytes);
    if (numberOfBytesWritten == -1) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_FileWrite", SYS_FS_FileError(fileHandle));
#endif
        return SdCardResultFileSystemError;
    }

    // File or SD card full
    if (numberOfBytesWritten != numberOfBytes) {
        return SdCardResultFileOrSdCardFull;
    }
    return SdCardResultOk;
}

/**
 * @brief Writes a string to the file. The terminating null character will not
 * be written.
 * @param sring String.
 * @return Result.
 */
SdCardResult SdCardFileWriteString(const char* const string) {
    return SdCardFileWrite(string, strlen(string));
}

/**
 * @brief Gets the file size.
 * @return File size.
 */
size_t SdCardFileGetSize(void) {
    const int32_t fileSize = SYS_FS_FileSize(fileHandle);
    if (fileSize == -1) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_FileSize", SYS_FS_FileError(fileHandle));
#endif
        return 0;
    }
    return (size_t) fileSize;
}

/**
 * @brief Closes the file.
 */
void SdCardFileClose(void) {
    if (SYS_FS_FileClose(fileHandle) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_FileClose", SYS_FS_FileError(fileHandle));
#endif
    }
}

/**
 * @brief Opens a directory.
 * @param directory Directory. "" if root.
 */
void SdCardDirectoryOpen(const char* const directory) {
    if (SYS_FS_DirectoryChange("/") != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_DirectoryChange", SYS_FS_Error());
#endif
        return;
    }
    directoryHandle = SYS_FS_DirOpen(SdCardPathJoin(2, MOUNT_NAME, directory));
    if (directoryHandle == SYS_FS_HANDLE_INVALID) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_DirOpen", SYS_FS_Error());
#endif
    }
}

/**
 * @brief Searches the directory for files matching the file name. The file
 * name can include wild cards (e.g. "*" to match all files). If multiple files
 * match the file name then this function can be called in a loop to retrieve
 * the file details for each file. If no files match the file name then the
 * file name provided in the file details will be an empty string. The
 * directory must be closed and reopened for each new search.
 * @param fileName File name.
 * @param fileDetails File details.
 */
void SdCardDirectorySearch(const char* const fileName, SdCardFileDetails * const fileDetails) {

    // Get search result
    SYS_FS_FSTAT sysFSFstat;
    char longFileName[SD_CARD_MAX_PATH_SIZE];
    sysFSFstat.lfname = longFileName;
    sysFSFstat.lfsize = sizeof (longFileName);
    if (SYS_FS_DirSearch(directoryHandle, fileName, SYS_FS_ATTR_MASK, &sysFSFstat) == SYS_FS_RES_FAILURE) {
        const SYS_FS_ERROR sysFSError = SYS_FS_Error();
        if (sysFSError != SYS_FS_ERROR_NO_FILE) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
            PrintFileSystemError("SYS_FS_DirSearch", sysFSError);
#endif
        }
        memset(fileDetails, 0, sizeof (SdCardFileDetails));
        return;
    }

    // Copy file size
    fileDetails->size = sysFSFstat.fsize;

    // Parse time
    SYS_FS_TIME sysFSTime;
    sysFSTime.timeDate.time = sysFSFstat.ftime;
    sysFSTime.timeDate.date = sysFSFstat.fdate;
    fileDetails->time.second = sysFSTime.discreteTime.second;
    fileDetails->time.minute = sysFSTime.discreteTime.minute;
    fileDetails->time.hour = sysFSTime.discreteTime.hour;
    fileDetails->time.day = sysFSTime.discreteTime.day;
    fileDetails->time.month = sysFSTime.discreteTime.month;
    fileDetails->time.year = 1980 + sysFSTime.discreteTime.year;

    // Copy file name
    if (strlen(sysFSFstat.lfname) == 0) {
        snprintf(fileDetails->name, sizeof (fileDetails->name), "%s", sysFSFstat.fname);
    } else {
        snprintf(fileDetails->name, sizeof (fileDetails->name), "%s", sysFSFstat.lfname);
    }
}

/**
 * @brief Returns true if any files matching the file name exist in the
 * directory. The file name can include wild cards (e.g. "*.txt" to match all
 * text files). This function is optimised to be called in a loop to check the
 * exitance of multiple files that were written in the same order as being
 * checked by the loop.
 * @param fileName File name.
 * @return True if any files matching the file name exist.
 */
bool SdCardDirectoryExists(const char* const fileName) {

    // Search from unknown starting position in directory
    SdCardFileDetails fileDetails;
    SdCardDirectorySearch(fileName, &fileDetails);
    if (strlen(fileDetails.name) > 0) {
        return true;
    }

    // Repeat search from start of directory
    if (SYS_FS_DirRewind(directoryHandle) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_DirRewind", SYS_FS_FileError(directoryHandle));
#endif
        return false;
    }
    SdCardDirectorySearch(fileName, &fileDetails);
    if (strlen(fileDetails.name) > 0) {
        return true;
    }
    return false;
}

/**
 * @brief Closes the directory.
 */
void SdCardDirectoryClose(void) {
    if (SYS_FS_DirClose(directoryHandle) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_DirClose", SYS_FS_FileError(directoryHandle));
#endif
    }
}

/**
 * @brief Prints the contents of a directory.
 * @param directory Directory. "" if root.
 */
void SdCardPrintDirectory(const char* const directory) {
    SdCardDirectoryOpen(directory);
    SdCardFileDetails fileDetails;
    SdCardDirectorySearch("*", &fileDetails);
    while (strlen(fileDetails.name) > 0) {
        printf("%-*s %s    %s\n", 32, fileDetails.name, RtcTimeToString(&fileDetails.time), SdCardSizeToString(fileDetails.size));
        SdCardDirectorySearch("*", &fileDetails);
    }
    SdCardDirectoryClose();
}

/**
 * @brief Returns the size as a string in the format "1,234,456 KB" or "123
 * bytes" if the size is less than 1 KB where 1 KB = 1024 bytes.
 * @param size Size.
 * @return Size as a string.
 */
const char* SdCardSizeToString(const size_t size) {

    // Calculate number of KB
    static char string[16];
    unsigned int kb = size >> 10;
    if (kb == 0) {
        snprintf(string, sizeof (string), "%u bytes", (unsigned int) size);
        return string;
    }

    // Initialise empty string
    strcpy(string, "");

    // Create unseparated digits
    char digits[16];
    snprintf(digits, sizeof (digits), "%u", kb);

    // Build string one character at a time
    char* digit = digits;
    while (*digit != '\0') {

        // Add digit to string
        char digitString[] = {*digit, '\0'};
        Concatenate(string, sizeof (string), digitString);

        // Break if no more digits
        if (*++digit == '\0') {
            break;
        }

        // Add comma if number of remaining digits multiple of 3
        if ((strlen(digit) % 3) == 0) {
            Concatenate(string, sizeof (string), ",");
        }
    }

    // Add "KB"
    Concatenate(string, sizeof (string), " KB");
    return string;
}

/**
 * @brief Concatenates two strings.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @param source Source.
 */
void Concatenate(char* const destination, const size_t destinationSize, const char* const source) {
    strncat(destination, source, destinationSize - strlen(destination) - 1);
}

/**
 * @brief Renames a file or directory.
 * @param path File or directory path.
 * @param newPath New file or directory path.
 */
void SdCardRename(const char* const path, const char* const newPath) {
    if (SYS_FS_DirectoryChange("/") != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_DirectoryChange", SYS_FS_Error());
#endif
        return;
    }
    if (SYS_FS_FileDirectoryRenameMove(path, newPath) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_FileDirectoryRenameMove", SYS_FS_Error());
#endif
    }
}

/**
 * @brief Deletes a file or directory.
 * @param path File or directory path.
 */
void SdCardDelete(const char* const path) {
    if (SYS_FS_DirectoryChange("/") != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_DirectoryChange", SYS_FS_Error());
#endif
        return;
    }
    if (SYS_FS_FileDirectoryRemove(path) != SYS_FS_RES_SUCCESS) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
        PrintFileSystemError("SYS_FS_FileDirectoryRemove", SYS_FS_Error());
#endif
    }
}

/**
 * @brief Splits the file path to return the file name.
 * @param filePath File path.
 * @return File name.
 */
const char* SdCardPathSplitFileName(const char* const filePath) {
    const char* const lastSeparator = strrchr(filePath, '/');
    if (lastSeparator == NULL) {
        return filePath;
    }
    return lastSeparator + 1;
}

/**
 * @brief Splits the file path to return the directory.
 * @param filePath File path.
 * @return Directory.
 */
const char* SdCardPathSplitDirectory(const char* const filePath) {

    // Copy file path
    static char directory[SD_CARD_MAX_PATH_SIZE];
    snprintf(directory, sizeof (directory), "%s", filePath);

    // Truncate to directory
    char* lastSeparator = strrchr(directory, '/');
    if (lastSeparator == NULL) {
        *directory = '\0';
    } else {
        *lastSeparator = '\0';
    }
    return directory;
}

/**
 * @brief Joins parts and returns the resultant path.
 * @param numberOfParts Number of parts.
 * @param ... Parts.
 * @return Path.
 */
const char* SdCardPathJoin(const int numberOfParts, ...) {

    // Initialise empty string
    static char path[SD_CARD_MAX_PATH_SIZE];
    strcpy(path, "");

    // Loop through each part
    va_list parts;
    va_start(parts, numberOfParts);
    for (int index = 0; index < numberOfParts; index++) {

        // Copy part for tokenisation
        char part[SD_CARD_MAX_PATH_SIZE];
        snprintf(part, sizeof (part), "%s", va_arg(parts, char*));

        // Append each sub part to path
        const char* subPart = strtok(part, "/");
        while (subPart != NULL) {
            Concatenate(path, sizeof (path), "/");
            Concatenate(path, sizeof (path), subPart);
            subPart = strtok(NULL, "/");
        }
    }
    va_end(parts);
    return path;
}

#ifdef PRINT_FILE_SYSTEM_ERRORS

/**
 * @brief Prints file system error.
 * @param sysFsError File system error.
 */
static void PrintFileSystemError(const char* functionName, const SYS_FS_ERROR sysFsError) {
    char* error = "";
    switch (sysFsError) {
        case SYS_FS_ERROR_OK:
            error = "SYS_FS_ERROR_OK";
            break;
        case SYS_FS_ERROR_DISK_ERR:
            error = "SYS_FS_ERROR_DISK_ERR";
            break;
        case SYS_FS_ERROR_INT_ERR:
            error = "SYS_FS_ERROR_INT_ERR";
            break;
        case SYS_FS_ERROR_NOT_READY:
            error = "SYS_FS_ERROR_NOT_READY";
            break;
        case SYS_FS_ERROR_NO_FILE:
            error = "SYS_FS_ERROR_NO_FILE";
            break;
        case SYS_FS_ERROR_NO_PATH:
            error = "SYS_FS_ERROR_NO_PATH";
            break;
        case SYS_FS_ERROR_INVALID_NAME:
            error = "SYS_FS_ERROR_INVALID_NAME";
            break;
        case SYS_FS_ERROR_DENIED:
            error = "SYS_FS_ERROR_DENIED";
            break;
        case SYS_FS_ERROR_EXIST:
            error = "SYS_FS_ERROR_EXIST";
            break;
        case SYS_FS_ERROR_INVALID_OBJECT:
            error = "SYS_FS_ERROR_INVALID_OBJECT";
            break;
        case SYS_FS_ERROR_WRITE_PROTECTED:
            error = "SYS_FS_ERROR_WRITE_PROTECTED";
            break;
        case SYS_FS_ERROR_INVALID_DRIVE:
            error = "SYS_FS_ERROR_INVALID_DRIVE";
            break;
        case SYS_FS_ERROR_NOT_ENABLED:
            error = "SYS_FS_ERROR_NOT_ENABLED";
            break;
        case SYS_FS_ERROR_NO_FILESYSTEM:
            error = "SYS_FS_ERROR_NO_FILESYSTEM";
            break;
        case SYS_FS_ERROR_FORMAT_ABORTED:
            error = "SYS_FS_ERROR_FORMAT_ABORTED";
            break;
        case SYS_FS_ERROR_TIMEOUT:
            error = "SYS_FS_ERROR_TIMEOUT";
            break;
        case SYS_FS_ERROR_LOCKED:
            error = "SYS_FS_ERROR_LOCKED";
            break;
        case SYS_FS_ERROR_NOT_ENOUGH_CORE:
            error = "SYS_FS_ERROR_NOT_ENOUGH_CORE";
            break;
        case SYS_FS_ERROR_TOO_MANY_OPEN_FILES:
            error = "SYS_FS_ERROR_TOO_MANY_OPEN_FILES";
            break;
        case SYS_FS_ERROR_INVALID_PARAMETER:
            error = "SYS_FS_ERROR_INVALID_PARAMETER";
            break;
        case SYS_FS_ERROR_NOT_ENOUGH_FREE_VOLUME:
            error = "SYS_FS_ERROR_NOT_ENOUGH_FREE_VOLUME";
            break;
        case SYS_FS_ERROR_FS_NOT_SUPPORTED:
            error = "SYS_FS_ERROR_FS_NOT_SUPPORTED";
            break;
        case SYS_FS_ERROR_FS_NOT_MATCH_WITH_VOLUME:
            error = "SYS_FS_ERROR_FS_NOT_MATCH_WITH_VOLUME";
            break;
        case SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS:
            error = "SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS";
            break;
    }
    printf("%s failed. %s\n", functionName, error);
}

#endif

//------------------------------------------------------------------------------
// End of file
