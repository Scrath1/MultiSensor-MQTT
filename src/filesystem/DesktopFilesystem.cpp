#include "DesktopFilesystem.h"

#include <cstdio>
#include <fstream>
#include <iostream>

DesktopFilesystem::DesktopFilesystem() {
    successfullyMounted = true;
}

RC_t DesktopFilesystem::openFile(const char filename[], OpenMode_t openMode) {
    std::ios::openmode mode;
    switch (openMode) {
        default:
        case READ_ONLY:
            mode = std::ios::in;
            break;
        case WRITE_TRUNCATE:
            mode = std::ios::in | std::ios::out | std::ios::trunc;
            break;
        case WRITE_APPEND:
            mode = std::ios::in | std::ios::out | std::ios::app;
            break;
    }
    // if a file was previously opened, close it first
    if (hasOpenFile()) return RC_ERROR_BUSY;

    fileMode = openMode;
    file.open(filename, mode);
    if (hasOpenFile())
        return RC_SUCCESS;
    else
        return RC_ERROR_OPEN;
}

bool DesktopFilesystem::hasOpenFile() {
    return file.is_open();
}

RC_t DesktopFilesystem::closeFile() {
    if (hasOpenFile()) file.close();
    return RC_SUCCESS;
}

RC_t DesktopFilesystem::write(const uint8_t* data, uint32_t n) {
    if (!hasOpenFile()) return RC_ERROR_OPEN;
    file.write((const char*)data, n);
    return RC_SUCCESS;
}

RC_t DesktopFilesystem::flush() {
    if (!hasOpenFile()) return RC_ERROR_OPEN;
    file.flush();
    if (file.fail()) return RC_ERROR;
    return RC_SUCCESS;
}

RC_t DesktopFilesystem::read(uint8_t* data, uint32_t n) {
    if (!hasOpenFile()) return RC_ERROR_OPEN;
    file.read((char*)data, n);
    if (file.fail() && !file.eof()) return RC_ERROR_READ_FAILS;
    return RC_SUCCESS;
}

RC_t DesktopFilesystem::readUntil(uint8_t* data, uint32_t n, uint8_t end) {
    if (!hasOpenFile()) return RC_ERROR_OPEN;
    file.getline((char*)data, n, (char)end);
    if (file.fail() && !file.eof()) return RC_ERROR_READ_FAILS;
    return RC_SUCCESS;
}

bool DesktopFilesystem::eof() {
    return !hasOpenFile() || file.eof();
}

bool DesktopFilesystem::fileExists(const char filename[]) {
    std::ifstream f;
    f.open(filename);
    if (f.fail()) return false;

    file.close();
    return true;
}

RC_t DesktopFilesystem::deleteFile(const char filename[]) {
    if (0 == remove(filename))
        return RC_SUCCESS;
    else
        return RC_ERROR;
}