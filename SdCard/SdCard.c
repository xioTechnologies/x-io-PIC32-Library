/**
 * @file SdCard.c
 * @author Seb Madgwick
 * @brief Wrapper for the MPLAB Harmony SD card driver.
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
 * @brief Uncomment this line to print file system errors.
 */
//#define PRINT_FILE_SYSTEM_ERRORS

/**
 * @brief Device name.
 */
#define DEV_NAME "/dev/mmcblka1"

/**
 * @brief Mount name.
 */
#define MOUNT_NAME "/mnt/myDrive"

/**
 * @brief Sector size.
 */
#define SECTOR_SIZE (FF_MAX_SS != FF_MIN_SS ? GET_SECTOR_SIZE : FF_MAX_SS)

/**
 * @brief State.
 */
typedef enum {
    StateUnmounted,
    StateMounting,
    StateMounted,
} State;

//------------------------------------------------------------------------------
// Function declarations

static const char* ChangeDirectory(const char* const filePath, const bool create);
static void Concatenate(char* const destination, const size_t destinationSize, const char* const source);
static void PrintFileSystemError(void);

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
        case StateUnmounted:
            break;
        case StateMounting:
            if (SYS_FS_MEDIA_MANAGER_MediaStatusGet(DEV_NAME) == false) {
                return;
            }
            if (SYS_FS_Mount(DEV_NAME, MOUNT_NAME, FAT, 0, NULL) != SYS_FS_RES_SUCCESS) {
                PrintFileSystemError();
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
    }
}

/**
 * @brief Mounts the SD card.
 */
void SdCardMount(void) {
    switch (state) {
        case StateUnmounted:
            state = StateMounting;
            break;
        case StateMounting:
        case StateMounted:
            return;
    }
}

/**
 * @brief Unmounts the SD card.
 */
void SdCardUnmount(void) {
    switch (state) {
        case StateUnmounted:
            return;
        case StateMounting:
            state = StateUnmounted;
            return;
        case StateMounted:
            if (SYS_FS_Unmount(MOUNT_NAME) != SYS_FS_RES_SUCCESS) {
                PrintFileSystemError();
            }
            state = StateUnmounted;
            break;
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
 * @return Result.
 */
SdCardResult SdCardFormat(void) {
    const SYS_FS_FORMAT_PARAM options = {
        .fmt = SYS_FS_FORMAT_FAT32,
        .n_fat = 1,
        .align = 0,
        .n_root = 1,
        .au_size = 0
    };
    static uint8_t CACHE_ALIGN work[512];
    if (SYS_FS_DriveFormat(MOUNT_NAME, &options, work, sizeof (work)) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Gets capacity.
 * @param capacity Capacity.
 * @return Result.
 */
SdCardResult SdCardGetCapacity(SdCardCapacity * const capacity) {
    uint32_t totalSectors;
    uint32_t freeSectors;
    if (SYS_FS_DriveSectorGet(MOUNT_NAME, &totalSectors, &freeSectors) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    capacity->capacity = (uint64_t) totalSectors * (uint64_t) SECTOR_SIZE;
    capacity->used = (uint64_t) (totalSectors - freeSectors) * (uint64_t) SECTOR_SIZE;
    capacity->available = (uint64_t) freeSectors * (uint64_t) SECTOR_SIZE;
    return SdCardResultOk;
}

/**
 * @brief Sets the volume label.
 * @param label Label.
 * @return Result.
 */
SdCardResult SdCardSetVolumeLabel(const char* const label) {
    if (SYS_FS_DriveLabelSet(MOUNT_NAME, label) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Opens a file for reading.
 * @param filePath File path.
 * @return Result.
 */
SdCardResult SdCardFileOpen(const char* const filePath) {

    // Change directory
    const char* const fileName = ChangeDirectory(filePath, false);
    if (fileName == NULL) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }

    // Open file
    fileHandle = SYS_FS_FileOpen(fileName, SYS_FS_FILE_OPEN_READ);
    if (fileHandle == SYS_FS_HANDLE_INVALID) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Creates a file for writing. Creating a file that already exists will
 * overwrite the existing file.
 * @param filePath File path.
 * @return Result.
 */
SdCardResult SdCardFileCreate(const char* const filePath) {

    // Change directory
    const char* const fileName = ChangeDirectory(filePath, true);
    if (fileName == NULL) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }

    // Create file
    fileHandle = SYS_FS_FileOpen(fileName, SYS_FS_FILE_OPEN_WRITE);
    if (fileHandle == SYS_FS_HANDLE_INVALID) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Changes the directory to that of the file path.
 * @param filePath File path.
 * @param create True to create the directory.
 * @return File name. NULL if a file system error occurred.
 */
static const char* ChangeDirectory(const char* const filePath, const bool create) {

    // Change to root directory
    if (SYS_FS_DirectoryChange("/") != SYS_FS_RES_SUCCESS) {
        return NULL;
    }

    // Split file name and directory
    char directory[SD_CARD_MAX_PATH_SIZE];
    SdCardPathDirectory(directory, sizeof (directory), filePath);
    const char* const fileName = SdCardPathFileName(filePath);
    if (strlen(directory) == 0) {
        return fileName;
    }

    // Change directory
    if (create == false) {
        if (SYS_FS_DirectoryChange(directory) != SYS_FS_RES_SUCCESS) {
            return NULL;
        }
        return fileName;
    }

    // Create directory
    char incrementingPath[SD_CARD_MAX_PATH_SIZE] = "";
    char* tokenPosition;
    const char* part = strtok_r(directory, "/", &tokenPosition);
    while (part != NULL) {
        if ((SYS_FS_DirectoryMake(part) != SYS_FS_RES_SUCCESS) && (SYS_FS_Error() != SYS_FS_ERROR_EXIST)) {
            return NULL;
        }
        SdCardPathAppend(incrementingPath, sizeof (incrementingPath), part);
        if (SYS_FS_DirectoryChange(incrementingPath) != SYS_FS_RES_SUCCESS) {
            return NULL;
        }
        part = strtok_r(NULL, "/", &tokenPosition);
    }
    return fileName;
}

/**
 * @brief Reads data from the file.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @param numberOfBytes Number of bytes read.
 * @return Result.
 */
SdCardResult SdCardFileRead(void* const destination, const size_t destinationSize, size_t * const numberOfBytes) {
    *numberOfBytes = SYS_FS_FileRead(fileHandle, destination, destinationSize);
    if (*numberOfBytes == (size_t) - 1) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Reads a string from the file. This function will read until a '\n' is
 * read or the end of file is reached.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @return Result.
 */
SdCardResult SdCardFileReadString(void* const destination, const size_t destinationSize) {
    if (SYS_FS_FileStringGet(fileHandle, destination, destinationSize) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Writes data to the file.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return Result.
 */
SdCardResult SdCardFileWrite(const void* const data, const size_t numberOfBytes) {
    const size_t numberOfBytesWritten = SYS_FS_FileWrite(fileHandle, data, numberOfBytes);
    if (numberOfBytesWritten == (size_t) - 1) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    if (numberOfBytesWritten < numberOfBytes) {
        return SdCardResultWriteIncomplete;
    }
    return SdCardResultOk;
}

/**
 * @brief Writes a string to the file. The terminating null character will not
 * be written.
 * @param string String.
 * @return Result.
 */
SdCardResult SdCardFileWriteString(const char* const string) {
    return SdCardFileWrite(string, strlen(string));
}

/**
 * @brief Gets the file size.
 * @param size Size.
 * @return Result.
 */
SdCardResult SdCardFileGetSize(size_t * const size) {
    const int32_t fileSize = SYS_FS_FileSize(fileHandle);
    if (fileSize == -1) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    *size = (size_t) fileSize;
    return SdCardResultOk;
}

/**
 * @brief Flushes the file.
 * @return Result.
 */
SdCardResult SdCardFileFlush(void) {
    if (SYS_FS_FileSync(fileHandle) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Closes the file.
 * @return Result.
 */
SdCardResult SdCardFileClose(void) {
    if (SYS_FS_FileClose(fileHandle) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Wraps SdCardFileRead in a file open/close sequence.
 * @param filePath File path.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @param numberOfBytes Number of bytes read.
 * @return Result.
 */
SdCardResult SdCardFileQuickRead(const char* const filePath, void* const destination, const size_t destinationSize, size_t * const numberOfBytes) {
    SdCardResult result = SdCardFileOpen(filePath);
    if (result != SdCardResultOk) {
        return result;
    }
    result = SdCardFileRead(destination, destinationSize, numberOfBytes);
    if (result != SdCardResultOk) {
        SdCardFileClose();
        return result;
    }
    return SdCardFileClose();
}

/**
 * @brief Wraps SdCardFileReadString in a file open/close sequence.
 * @param filePath File path.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @return Result.
 */
SdCardResult SdCardFileQuickReadString(const char* const filePath, void* const destination, const size_t destinationSize) {
    SdCardResult result = SdCardFileOpen(filePath);
    if (result != SdCardResultOk) {
        return result;
    }
    result = SdCardFileReadString(destination, destinationSize);
    if (result != SdCardResultOk) {
        SdCardFileClose();
        return result;
    }
    return SdCardFileClose();
}

/**
 * @brief Wraps SdCardFileWrite in a file create/close sequence.
 * @param filePath File path.
 * @param data Data.
 * @param numberOfBytes Number of bytes.
 * @return Result.
 */
SdCardResult SdCardFileQuickWrite(const char* const filePath, const void* const data, const size_t numberOfBytes) {
    SdCardResult result = SdCardFileCreate(filePath);
    if (result != SdCardResultOk) {
        return result;
    }
    result = SdCardFileWrite(data, numberOfBytes);
    if (result != SdCardResultOk) {
        SdCardFileClose();
        return result;
    }
    return SdCardFileClose();
}

/**
 * @brief Wraps SdCardFileWriteString in a file create/close sequence.
 * @param filePath File path.
 * @param string String.
 * @return Result.
 */
SdCardResult SdCardFileQuickWriteString(const char* const filePath, const char* const string) {
    SdCardResult result = SdCardFileCreate(filePath);
    if (result != SdCardResultOk) {
        return result;
    }
    result = SdCardFileWriteString(string);
    if (result != SdCardResultOk) {
        SdCardFileClose();
        return result;
    }
    return SdCardFileClose();
}

/**
 * @brief Opens a directory.
 * @param directory Directory. "" if root.
 * @return Result.
 */
SdCardResult SdCardDirectoryOpen(const char* const directory) {
    char path[SD_CARD_MAX_PATH_SIZE];
    SdCardPathJoin(path, sizeof (path), 2, MOUNT_NAME, directory);
    directoryHandle = SYS_FS_DirOpen(path);
    if (directoryHandle == SYS_FS_HANDLE_INVALID) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Searches the directory for files matching the file name. The file
 * name can include wild cards (e.g. "*" to match all files). If multiple files
 * match the file name then this function can be called in a loop to retrieve
 * the file status for each file. If no files match the file name then the
 * file name provided in the file status will be an empty string. The directory
 * must be closed and reopened for each new search.
 * @param fileName File name.
 * @param fileStatus File status.
 * @return Result.
 */
SdCardResult SdCardDirectorySearch(const char* const fileName, SdCardFileStatus * const fileStatus) {

    // Get search result
    SYS_FS_FSTAT fsStatus;
    char longFileName[SD_CARD_MAX_PATH_SIZE];
    fsStatus.lfname = longFileName;
    fsStatus.lfsize = sizeof (longFileName);
    if (SYS_FS_DirSearch(directoryHandle, fileName, SYS_FS_ATTR_MASK, &fsStatus) == SYS_FS_RES_FAILURE) {
        if (SYS_FS_Error() != SYS_FS_ERROR_NO_FILE) {
            PrintFileSystemError();
            return SdCardResultFileSystemError;
        }
        memset(fileStatus, 0, sizeof (SdCardFileStatus));
        return SdCardResultOk;
    }

    // File size
    fileStatus->size = fsStatus.fsize;

    // Time
    SYS_FS_TIME time;
    time.timeDate.time = fsStatus.ftime;
    time.timeDate.date = fsStatus.fdate;
    fileStatus->time.second = time.discreteTime.second;
    fileStatus->time.minute = time.discreteTime.minute;
    fileStatus->time.hour = time.discreteTime.hour;
    fileStatus->time.day = time.discreteTime.day;
    fileStatus->time.month = time.discreteTime.month;
    fileStatus->time.year = 1980 + time.discreteTime.year;

    // Attributes
    fileStatus->isDirectory = (fsStatus.fattrib & SYS_FS_ATTR_DIR) != 0;

    // File name
    snprintf(fileStatus->name, sizeof (fileStatus->name), "%s", strlen(fsStatus.lfname) > 0 ? fsStatus.lfname : fsStatus.fname);
    return SdCardResultOk;
}

/**
 * @brief Returns true if any files matching the file name exist in the
 * directory. The file name can include wild cards (e.g. "*.txt" to match all
 * text files). This function is optimised to be called in a loop to check the
 * existence of multiple files that were written in the same order as being
 * checked by the loop.
 * @param fileName File name.
 * @return True if any files matching the file name exist.
 */
bool SdCardDirectoryExists(const char* const fileName) {

    // Search from unknown starting position in directory
    SdCardFileStatus fileStatus;
    if (SdCardDirectorySearch(fileName, &fileStatus) != SdCardResultOk) {
        return false;
    }
    if (strlen(fileStatus.name) > 0) {
        return true;
    }

    // Repeat search from start of directory
    if (SYS_FS_DirRewind(directoryHandle) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return false;
    }
    if (SdCardDirectorySearch(fileName, &fileStatus) != SdCardResultOk) {
        return false;
    }
    return strlen(fileStatus.name) > 0;
}

/**
 * @brief Closes the directory.
 * @return Result.
 */
SdCardResult SdCardDirectoryClose(void) {
    if (SYS_FS_DirClose(directoryHandle) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Wraps SdCardDirectorySearch inside a directory open/close sequence.
 * @param directory Directory. "" if root.
 * @param fileName File name.
 * @param fileStatus File status.
 * @return Result.
 */
SdCardResult SdCardDirectoryQuickSearch(const char* const directory, const char* const fileName, SdCardFileStatus * const fileStatus) {
    SdCardResult result = SdCardDirectoryOpen(directory);
    if (result != SdCardResultOk) {
        return result;
    }
    result = SdCardDirectorySearch(fileName, fileStatus);
    if (result != SdCardResultOk) {
        SdCardDirectoryClose();
        return result;
    }
    return SdCardDirectoryClose();
}

/**
 * @brief Wraps SdCardDirectoryExists inside a directory open/close sequence.
 * @param directory Directory. "" if root.
 * @param fileName File name.
 * @return True if any files matching the file name exist.
 */
bool SdCardDirectoryQuickExists(const char* const directory, const char* const fileName) {
    if (SdCardDirectoryOpen(directory) != SdCardResultOk) {
        return false;
    }
    const bool exists = SdCardDirectoryExists(fileName);
    SdCardDirectoryClose();
    return exists;
}

/**
 * @brief Renames a file or directory.
 * @param oldPath Old file or directory path.
 * @param newPath New file or directory path.
 * @return Result.
 */
SdCardResult SdCardRename(const char* const oldPath, const char* const newPath) {
    if (SYS_FS_DirectoryChange("/") != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    if (SYS_FS_FileDirectoryRenameMove(oldPath, newPath) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Deletes a file or directory.
 * @param path File or directory path.
 * @return Result.
 */
SdCardResult SdCardDelete(const char* const path) {
    if (SYS_FS_DirectoryChange("/") != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    if (SYS_FS_FileDirectoryRemove(path) != SYS_FS_RES_SUCCESS) {
        PrintFileSystemError();
        return SdCardResultFileSystemError;
    }
    return SdCardResultOk;
}

/**
 * @brief Provides the file name part of the file path. The return value is a
 * pointer to start of the file name in the original file path.
 * @param filePath File path.
 * @return File name part of the file path.
 */
const char* SdCardPathFileName(const char* const filePath) {
    const char* const lastSeparator = strrchr(filePath, '/');
    if (lastSeparator == NULL) {
        return filePath;
    }
    return lastSeparator + 1;
}

/**
 * @brief Provides the extension part of the file path. The return value is a
 * pointer to start of the extension in the original file path.
 * @param filePath File path.
 * @return Extension part of the file path.
 */
const char* SdCardPathExtension(const char* const filePath) {
    const char* const fileName = SdCardPathFileName(filePath);
    const char* const lastDot = strrchr(fileName, '.');
    if (lastDot == NULL) {
        return filePath + strlen(filePath);
    }
    return lastDot;
}

/**
 * @brief Provides the stem part of the file path.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @param filePath File path.
 */
void SdCardPathStem(char* const destination, const size_t destinationSize, const char* const filePath) {
    const char* const fileName = SdCardPathFileName(filePath);
    const char* const extension = SdCardPathExtension(fileName);
    snprintf(destination, destinationSize, "%.*s", extension - fileName, fileName);
}

/**
 * @brief Provides the directory part of the file path.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @param filePath File path.
 */
void SdCardPathDirectory(char* const destination, const size_t destinationSize, const char* const filePath) {

    // Copy file path
    snprintf(destination, destinationSize, "%s", filePath);

    // Truncate to directory
    char* lastSeparator = strrchr(destination, '/');
    if (lastSeparator != NULL) {
        *lastSeparator = '\0';
    } else {
        *destination = '\0';
    }
}

/**
 * @brief Joins parts and returns the resultant path.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @param numberOfParts Number of parts.
 * @param ... Parts.
 */
void SdCardPathJoin(char* const destination, const size_t destinationSize, const int numberOfParts, ...) {

    // Initialise empty string
    strcpy(destination, "");

    // Loop through each part
    va_list parts;
    va_start(parts, numberOfParts);
    for (int index = 0; index < numberOfParts; index++) {

        // Copy part for tokenisation
        char part[SD_CARD_MAX_PATH_SIZE];
        snprintf(part, sizeof (part), "%s", va_arg(parts, char*));

        // Append each sub part to path
        char* tokenPosition;
        const char* subPart = strtok_r(part, "/", &tokenPosition);
        while (subPart != NULL) {
            Concatenate(destination, destinationSize, "/");
            Concatenate(destination, destinationSize, subPart);
            subPart = strtok_r(NULL, "/", &tokenPosition);
        }
    }
    va_end(parts);
}

/**
 * @brief Appends a part to a path.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @param part Part.
 */
void SdCardPathAppend(char* const destination, const size_t destinationSize, const char* const part) {
    char destinationCopy[SD_CARD_MAX_PATH_SIZE];
    snprintf(destinationCopy, sizeof (destinationCopy), "%s", destination);
    SdCardPathJoin(destination, destinationSize, 2, destinationCopy, part);
}

/**
 * @brief Sets the extension.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @param extension Extension.
 */
void SdCardPathSetExtension(char* const destination, const size_t destinationSize, const char* const extension) {
    *(char*) SdCardPathExtension(destination) = '\0';
    Concatenate(destination, destinationSize, extension);
}

/**
 * @brief Creates a string in the format "1,234,567 KB", or "123 bytes" if the
 * size is less than 1 KB. 1 KB = 1024 bytes.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @param size Size.
 */
void SdCardSizeToString(char* const destination, const size_t destinationSize, const uint64_t size) {

    // Calculate number of KB
    unsigned int kb = (unsigned int) (size >> 10);
    if (kb == 0) {
        snprintf(destination, destinationSize, "%u bytes", (unsigned int) size);
        return;
    }

    // Initialise empty string
    strcpy(destination, "");

    // Create unseparated digits
    char digits[16];
    snprintf(digits, sizeof (digits), "%u", kb);

    // Build string one character at a time
    char* digit = digits;
    while (true) {
        char digitString[] = {*digit, '\0'};
        Concatenate(destination, destinationSize, digitString);
        if (*++digit == '\0') {
            break;
        }
        if ((strlen(digit) % 3) == 0) {
            Concatenate(destination, destinationSize, ",");
        }
    }

    // Add "KB"
    Concatenate(destination, destinationSize, " KB");
}

/**
 * @brief Concatenates two strings.
 * @param destination Destination.
 * @param destinationSize Destination size.
 * @param source Source.
 */
static void Concatenate(char* const destination, const size_t destinationSize, const char* const source) {
    strncat(destination, source, destinationSize - strlen(destination) - 1);
}

/**
 * @brief Prints the capacity.
 */
void SdCardPrintCapacity(void) {
    SdCardCapacity capacity;
    if (SdCardGetCapacity(&capacity) != SdCardResultOk) {
        return;
    }
    char size[16];
    SdCardSizeToString(size, sizeof (size), capacity.capacity);
    printf("Capacity  %12s\n", size);
    SdCardSizeToString(size, sizeof (size), capacity.used);
    printf("Used      %12s\n", size);
    SdCardSizeToString(size, sizeof (size), capacity.available);
    printf("Available %12s\n", size);
}

/**
 * @brief Prints the contents of a directory.
 * @param directory Directory. "" if root.
 */
void SdCardPrintDirectory(const char* const directory) {
    if (SdCardDirectoryOpen(directory) != SdCardResultOk) {
        return;
    }
    while (true) {
        SdCardFileStatus fileStatus;
        if (SdCardDirectorySearch("*", &fileStatus) != SdCardResultOk) {
            break;
        }
        if (strlen(fileStatus.name) == 0) {
            break;
        }
        char time[RTC_STRING_SIZE];
        RtcTimeToString(time, sizeof (time), &fileStatus.time);
        char size[16] = "";
        if (fileStatus.isDirectory == false) {
            SdCardSizeToString(size, sizeof (size), fileStatus.size);
        }
        printf("%-32s %s %12s\n", fileStatus.name, time, size);
    }
    SdCardDirectoryClose();
}

/**
 * @brief Prints the file system error.
 */
static void PrintFileSystemError(void) {
#ifdef PRINT_FILE_SYSTEM_ERRORS
    printf("%s\n", SdCardGetFileSystemError());
#endif
}

/**
 * @brief Returns a string representation of the result.
 * @param result Result.
 * @return String representation of the result.
 */
const char* SdCardResultToString(const SdCardResult result) {
    switch (result) {
        case SdCardResultOk:
            return "Ok";
        case SdCardResultWriteIncomplete:
            return "Write incomplete";
        case SdCardResultFileSystemError:
            return "File system error";
    }
    return ""; // avoid compiler warning
}

/**
 * @brief Returns the file system error.
 * @return File system error message.
 */
const char* SdCardGetFileSystemError(void) {
    switch (SYS_FS_Error()) {
        case SYS_FS_ERROR_OK:
            return "SYS_FS_ERROR_OK";
        case SYS_FS_ERROR_DISK_ERR:
            return "SYS_FS_ERROR_DISK_ERR";
        case SYS_FS_ERROR_INT_ERR:
            return "SYS_FS_ERROR_INT_ERR";
        case SYS_FS_ERROR_NOT_READY:
            return "SYS_FS_ERROR_NOT_READY";
        case SYS_FS_ERROR_NO_FILE:
            return "SYS_FS_ERROR_NO_FILE";
        case SYS_FS_ERROR_NO_PATH:
            return "SYS_FS_ERROR_NO_PATH";
        case SYS_FS_ERROR_INVALID_NAME:
            return "SYS_FS_ERROR_INVALID_NAME";
        case SYS_FS_ERROR_DENIED:
            return "SYS_FS_ERROR_DENIED";
        case SYS_FS_ERROR_EXIST:
            return "SYS_FS_ERROR_EXIST";
        case SYS_FS_ERROR_INVALID_OBJECT:
            return "SYS_FS_ERROR_INVALID_OBJECT";
        case SYS_FS_ERROR_WRITE_PROTECTED:
            return "SYS_FS_ERROR_WRITE_PROTECTED";
        case SYS_FS_ERROR_INVALID_DRIVE:
            return "SYS_FS_ERROR_INVALID_DRIVE";
        case SYS_FS_ERROR_NOT_ENABLED:
            return "SYS_FS_ERROR_NOT_ENABLED";
        case SYS_FS_ERROR_NO_FILESYSTEM:
            return "SYS_FS_ERROR_NO_FILESYSTEM";
        case SYS_FS_ERROR_FORMAT_ABORTED:
            return "SYS_FS_ERROR_FORMAT_ABORTED";
        case SYS_FS_ERROR_TIMEOUT:
            return "SYS_FS_ERROR_TIMEOUT";
        case SYS_FS_ERROR_LOCKED:
            return "SYS_FS_ERROR_LOCKED";
        case SYS_FS_ERROR_NOT_ENOUGH_CORE:
            return "SYS_FS_ERROR_NOT_ENOUGH_CORE";
        case SYS_FS_ERROR_TOO_MANY_OPEN_FILES:
            return "SYS_FS_ERROR_TOO_MANY_OPEN_FILES";
        case SYS_FS_ERROR_INVALID_PARAMETER:
            return "SYS_FS_ERROR_INVALID_PARAMETER";
        case SYS_FS_ERROR_NOT_ENOUGH_FREE_VOLUME:
            return "SYS_FS_ERROR_NOT_ENOUGH_FREE_VOLUME";
        case SYS_FS_ERROR_FS_NOT_SUPPORTED:
            return "SYS_FS_ERROR_FS_NOT_SUPPORTED";
        case SYS_FS_ERROR_FS_NOT_MATCH_WITH_VOLUME:
            return "SYS_FS_ERROR_FS_NOT_MATCH_WITH_VOLUME";
        case SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS:
            return "SYS_FS_ERROR_NOT_SUPPORTED_IN_NATIVE_FS";
    }
    return "";
}

//------------------------------------------------------------------------------
// End of file
