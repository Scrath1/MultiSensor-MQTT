#include "LittleFilesystem.h"
#include <exception>

#define FORMAT_LITTLEFS_IF_FAILED true

LittleFilesystem::LittleFilesystem(){
    if(LittleFS.begin(FORMAT_LITTLEFS_IF_FAILED)){
        successfullyMounted = true;
    }
}

RC_t LittleFilesystem::openFile(const char filename[], OpenMode_t openMode){
    char mode[3];
    switch(openMode){
        default:
        case READ_ONLY:
        strcpy(mode, "r");
        break;
        case WRITE_TRUNCATE:
        strcpy(mode, "w+");
        break;
        case WRITE_APPEND:
        strcpy(mode, "a+");
        break;
    }
    // if a file was previously opened, close it first
    if(hasOpenFile()) return RC_ERROR_BUSY;

    fileMode = openMode;
    if(openMode == READ_ONLY && !LittleFS.exists(filename))
        return RC_ERROR_OPEN;
    file = LittleFS.open(filename, mode);
    if(hasOpenFile()) return RC_SUCCESS;
    else return RC_ERROR_OPEN;
}

bool LittleFilesystem::hasOpenFile(){
    if(!file || file.isDirectory()) return false;
    else return true;
}

RC_t LittleFilesystem::closeFile(){
    if(hasOpenFile()) file.close();
    return RC_SUCCESS;
}

RC_t LittleFilesystem::write(const uint8_t* data, uint32_t n){
    if(!hasOpenFile()) return RC_ERROR_OPEN;
    file.write(data, n);
    return RC_SUCCESS;
}

RC_t LittleFilesystem::flush(){
    if(!hasOpenFile()) return RC_ERROR_OPEN;
    file.flush();
    return RC_SUCCESS;
}

RC_t LittleFilesystem::read(uint8_t* data, uint32_t n){
    if(!hasOpenFile()) return RC_ERROR_OPEN;
    file.read(data, n);
    return RC_SUCCESS;
}

RC_t LittleFilesystem::readUntil(uint8_t* data, uint32_t n, uint8_t end){
    if(!hasOpenFile()) return RC_ERROR_OPEN;
    file.readBytesUntil((char)end, data, n);
    return RC_SUCCESS;
}

bool LittleFilesystem::eof(){
    return !file.available();
}

bool LittleFilesystem::fileExists(const char filename[]){
    return LittleFS.exists(filename);
}

RC_t LittleFilesystem::deleteFile(const char filename[]){
    if(hasOpenFile()) closeFile();
    if(1 == LittleFS.remove(filename)) return RC_SUCCESS;
    else return RC_ERROR;
}