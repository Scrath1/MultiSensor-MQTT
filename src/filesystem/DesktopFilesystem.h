#ifndef DESKTOP_FILESYSTEM_H
#define DESKTOP_FILESYSTEM_H
#include <cstdint>
#include <fstream>

#include "Filesystem.h"

class DesktopFilesystem : public Filesystem {
   public:
    /**
     * @brief Constructor
     */
    DesktopFilesystem();

    /**
     * @brief Opens a file with the given filename
     *
     * @param filename [IN] Name of the file to open
     * @param openMode [IN] Mode in which to open file. Defaults to READ_ONLY.
     * @return RC_t RC_SUCCESS on success,
     *          RC_ERROR_OPEN if the file couldn't be opened
     *          RC_ERROR_BUSY if a file is already open
     */
    RC_t openFile(const char filename[], OpenMode_t openMode = READ_ONLY);

    /**
     * @brief Returns whether the filesystem currently has a file that is open
     *
     * @return true
     * @return false
     */
    bool hasOpenFile();

    /**
     * @brief Closes the currently open file
     *
     * @return RC_t
     */
    RC_t closeFile();

    /**
     * @brief Writes n bytes pointed to by data to an open file.
     *
     * @param data [IN] Pointer to start position of data array
     * @param n [IN] Amount of bytes to write
     * @return RC_t RC_SUCCESS on success,
     *          RC_ERROR_OPEN if no file is open.
     *          RC_ERROR_WRITE_FAILS if the file is opened in read only mode
     */
    RC_t write(const uint8_t* data, uint32_t n);

    /**
     * @brief Flushes write buffer
     *
     * @return RC_t RC_SUCCESS on success, else RC_ERROR
     */
    RC_t flush();

    /**
     * @brief Reads at most n bytes of data into the array pointed to
     * by data parameter
     *
     * @param data [OUT] Array for storing read data
     * @param n [IN] Maximum amount of data to read in bytes / Size of data
     * @return RC_t RC_SUCCESS on success,
     *          RC_ERROR_OPEN if no file is open
     *          RC_ERROR_READ_FAILS if the read operation has failed without reaching end-of-file
     */
    RC_t read(uint8_t* data, uint32_t n);

    /**
     * @brief Reads at most n bytes of data or until the value of end parameter is found
     * into the array pointed to by data parameter
     *
     * @param data [OUT] Array for storing read data
     * @param n [IN] Maximum amount of data to read in bytes / Size of data
     * @param end [IN] Value upon which to stop reading, even if less than n bytes have been read. This value is NOT included in the output data.
     * @return RC_t RC_SUCCESS on success,
     *          RC_ERROR_OPEN if no file is open,
     *          RC_ERROR_READ_FAILS if the read operation has failed without reaching end-of-file
     */
    RC_t readUntil(uint8_t* data, uint32_t n, uint8_t end);

    /**
     * @brief Returns true if the end of file has been reached
     * or no file is open
     *
     * @return true eof was reached
     * @return false eof was not reached
     */
    bool eof();

    /**
     * @brief Tests if a file with the given filename exists
     *
     * @param filename [IN] Name of the file to test
     * @return true
     * @return false
     */
    bool fileExists(const char filename[]);

    /**
     * @brief Deletes a file with the given filename from the system
     *
     * @param filename [IN] Name of the file to delete
     * @return RC_t RC_SUCCESS on success,
     *          RC_ERROR on error
     */
    RC_t deleteFile(const char filename[]);

   private:
    /**
     * @brief Keeps track of in which mode the file was opened
     */
    OpenMode_t fileMode;
    /**
     * @brief File object
     */
    std::fstream file;
};

#endif  // DESKTOP_FILESYSTEM_H