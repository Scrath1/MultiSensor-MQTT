#ifndef FILESYSTEM_H
#define FILESYSTEM_H
#include "global.h"
#ifdef ARDUINO
    #include <Arduino.h>
#else
    #include <cstdint>
#endif  // ARDUINO

void filesystemSetup();

class Filesystem {
   public:
    typedef enum { READ_ONLY, WRITE_TRUNCATE, WRITE_APPEND } OpenMode_t;
    /**
     * @brief Constructer
     */
    Filesystem() = default;

    /**
     * @brief Opens a file with the given filename
     *
     * @param filename [IN] Name of the file to open
     * @param openMode [IN] Mode in which to open file. Defaults to READ_ONLY.
     * @return RC_t RC_SUCCESS on success,
     *          RC_ERROR_OPEN if the file couldn't be opened
     *          RC_ERROR_BUSY if a file is already open
     */
    virtual RC_t openFile(const char filename[], OpenMode_t openMode = READ_ONLY) = 0;

    /**
     * @brief Returns whether the filesystem currently has a file that is open
     *
     * @return true
     * @return false
     */
    virtual bool hasOpenFile() = 0;

    /**
     * @brief Closes the currently open file
     *
     * @return RC_t RC_SUCCESS on success
     */
    virtual RC_t closeFile() = 0;

    /**
     * @brief Writes n bytes pointed to by data to an open file.
     *
     * @param data [IN] Pointer to start position of data array with at least n bytes
     * @param n [IN] Amount of bytes to write
     * @return RC_t RC_SUCCESS on success,
     *          RC_ERROR_OPEN if no file is open.
     */
    virtual RC_t write(const uint8_t* data, uint32_t n) = 0;

    /**
     * @brief Flushes write buffer
     *
     * @return RC_t RC_SUCCESS on success,
     *          RC_ERROR_OPEN if no file is open,
     *          else RC_ERROR
     */
    virtual RC_t flush() = 0;

    /**
     * @brief Reads at most n bytes of data into the array pointed to
     * by data parameter
     *
     * @param data [OUT] Array for storing read data.
     * @param n [IN] Maximum amount of data to read in bytes / Size of data
     * @return RC_t RC_SUCCESS on success,
     *          RC_ERROR_OPEN if no file is open,
     *          RC_ERROR_READ_FAILS if the read operation has failed without reaching end-of-file
     */
    virtual RC_t read(uint8_t* data, uint32_t n) = 0;

    /**
     * @brief Reads at most n bytes of data or until the value of end parameter is found
     * into the array pointed to by data parameter
     *
     * @param data [OUT] Array for storing read data
     * @param n [IN] Maximum amount of data to read in bytes / Size of data
     * @param end [IN] Value upon which to stop reading, even if less than n bytes have been read. This value is NOT
     * included in the output data.
     * @return RC_t RC_SUCCESS on success,
     *          RC_ERROR_OPEN if no file is open
     *          RC_ERROR_READ_FAILS if the read operation has failed without reaching end-of-file
     */
    virtual RC_t readUntil(uint8_t* data, uint32_t n, uint8_t end) = 0;

    /**
     * @brief Returns true if the end of file has been reached
     * or no file is open
     *
     * @return true eof was reached
     * @return false eof was not reached
     */
    virtual bool eof() = 0;

    /**
     * @brief Tests if a file with the given filename exists
     *
     * @param filename [IN] Name of the file to test
     * @return true
     * @return false
     */
    virtual bool fileExists(const char filename[]) = 0;

    /**
     * @brief Deletes a file with the given filename from the system
     *
     * @param filename [IN] Name of the file to delete
     * @return RC_t RC_SUCCESS on success
     */
    virtual RC_t deleteFile(const char filename[]) = 0;

    /**
     * @brief Returns whether the filesystem was successfully initialized
     *
     * @return true
     * @return false
     */
    inline bool isInitialized() const { return successfullyMounted; }

   protected:
    /**
     * @brief Variable which can be set in the constructor to indicate whether the
     * filesystem was successfully initialized.
     */
    bool successfullyMounted = false;
};

#endif  // FILESYSTEM_H