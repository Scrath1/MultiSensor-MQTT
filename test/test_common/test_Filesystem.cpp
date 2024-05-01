#include <gtest/gtest.h>
#ifdef ARDUINO
#include "filesystem/LittleFilesystem.h"
#else
#include "filesystem/DesktopFilesystem.h"
#endif // ARDUINO

class FilesystemTest : public testing::Test
{
    protected:
    #ifdef ARDUINO
    LittleFilesystem fs;
    const char testFilename[15] = "/testfile.txt";
    #else
    DesktopFilesystem fs;
    const char testFilename[15] = "./testfile.txt";
    #endif // ARDUINO
    void SetUp() override
    {

    }

    void TearDown() override{
        // Doesn't work for some reason
        fs.deleteFile(testFilename);
    }
};

TEST_F(FilesystemTest, OpenCloseTest){
    if(fs.fileExists(testFilename))
        fs.deleteFile(testFilename);
    // Should fail since mode is read-only and cannot create file
    EXPECT_EQ(fs.openFile(testFilename), RC_ERROR_OPEN);
    
    // should work and a file be created
    EXPECT_EQ(fs.openFile(testFilename, Filesystem::WRITE_TRUNCATE), RC_SUCCESS);
    fs.closeFile();
    // should work now since a file was created
    EXPECT_EQ(fs.openFile(testFilename), RC_SUCCESS);

    fs.deleteFile(testFilename);
}

TEST_F(FilesystemTest, ReadWriteTest){
    EXPECT_EQ(fs.openFile(testFilename, Filesystem::WRITE_TRUNCATE), RC_SUCCESS);
    const char dataOut[] = "abc\ndefg";
    ASSERT_EQ(fs.write((uint8_t*)dataOut, sizeof(dataOut)-1), RC_SUCCESS);
    fs.closeFile();
    ASSERT_EQ(fs.openFile(testFilename), RC_SUCCESS);
    char dataIn[sizeof(dataOut)] = {0};
    ASSERT_EQ(fs.readUntil((uint8_t*)dataIn, sizeof(dataIn), '\n'), RC_SUCCESS);
    EXPECT_STREQ(dataIn, "abc");
    ASSERT_EQ(fs.read((uint8_t*)dataIn, sizeof(dataIn)-1), RC_SUCCESS);
    EXPECT_STREQ(dataIn, "defg");

    EXPECT_TRUE(fs.eof());
    
    fs.deleteFile(testFilename);
}

TEST_F(FilesystemTest, DeleteFileTest){
    ASSERT_EQ(fs.openFile(testFilename, Filesystem::WRITE_TRUNCATE), RC_SUCCESS);
    const char data[] = "abc";
    fs.write((uint8_t*)data, sizeof(data));
    fs.closeFile();
    ASSERT_TRUE(fs.fileExists(testFilename));
    EXPECT_EQ(fs.deleteFile(testFilename), RC_SUCCESS);
}