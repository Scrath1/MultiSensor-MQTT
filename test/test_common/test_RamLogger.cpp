#include <gtest/gtest.h>

#include <string>

#include "RamLogger.h"
// uncomment line below if you plan to use GMock
// #include <gmock/gmock.h>

#define NUM_START_ENTRIES 2
// Length of each entry in RamLogger minus the null terminator
#define MAX_STRING_LENGTH 6
// Number of strings the Ramlogger can hold
#define MAX_ENTRIES 3
// Number of test strings available
#define NUM_TEST_STRINGS 7

#define TIMESTAMP_STR_LEN 32

#if(NUM_START_ENTRIES >= MAX_ENTRIES)
    #error Number of start entries in the buffer should be less than maximum
#endif

class RamLoggerTest : public testing::Test {
   protected:
    /**
     * Make sure non-const objects are part of the test fixture or they wont be
     * reset between each test
     */
    // Actual RamLogger object
    RamLogger<MAX_ENTRIES, MAX_STRING_LENGTH, TIMESTAMP_STR_LEN> testLogger;
    // Strings used for testing the RamLogger
    const char* testStrings[NUM_TEST_STRINGS];

    void SetUp() override {
        // Prepare test data
        testStrings[0] = "Lorem";
        testStrings[1] = "ipsum";
        testStrings[2] = "dolor";
        testStrings[3] = "sit";
        testStrings[4] = "amet";
        testStrings[5] = "consectetur";
        testStrings[6] = "adipiscing";

        // Enter some test data but don't fill RamLogger up completely
        for(uint32_t i = 0; i < NUM_START_ENTRIES; i++) {
            RC_t err = testLogger.logLn(testStrings[i]);
            ASSERT_EQ(err, RC_SUCCESS);
        }
    }
};

/**
 * @brief Tests that the remaining free entries in the RamLogger are
 * calculated correctly
 */
TEST_F(RamLoggerTest, RemainingSpaceCorrect) {
    // Calculate how many strings should be in the ramLogger now
    uint32_t expectedRemainingEntries = MAX_ENTRIES - NUM_START_ENTRIES;

    ASSERT_EQ(testLogger.remaining(), expectedRemainingEntries);

    // Add a new entry
    testLogger.logLn(testStrings[NUM_START_ENTRIES]);
    expectedRemainingEntries--;
    ASSERT_EQ(testLogger.remaining(), expectedRemainingEntries);

    // Remove oldest entry
    char tmp[MAX_STRING_LENGTH];
    testLogger.pop(tmp, MAX_STRING_LENGTH);
    expectedRemainingEntries++;
    ASSERT_EQ(testLogger.remaining(), expectedRemainingEntries);

    testLogger.clear();
    ASSERT_EQ(testLogger.available(), 0);
    ASSERT_EQ(testLogger.remaining(), MAX_ENTRIES);
}

/**
 * @brief Tests that the RamLoggers msgCounter increments correctly
 */
TEST_F(RamLoggerTest, msgCounterIncrementTest) {
    uint32_t expectedMsgCounter = NUM_START_ENTRIES;
    EXPECT_EQ(testLogger.getMsgCounter(), expectedMsgCounter);

    char entry[MAX_STRING_LENGTH];
    RC_t err = testLogger.pop(entry, MAX_STRING_LENGTH);
    ASSERT_EQ(err, RC_SUCCESS);
    // Message counter should not change when popping from the buffer
    EXPECT_EQ(testLogger.getMsgCounter(), expectedMsgCounter);

    err = testLogger.logLn("foo");
    ASSERT_EQ(err, RC_SUCCESS);
    // Message counter should have incremented by one
    EXPECT_EQ(testLogger.getMsgCounter(), expectedMsgCounter + 1);
}

/**
 * @brief Tests the indexing of the available get function
 */
TEST_F(RamLoggerTest, GetFunctionTest) {
    // Oldest entry after setup should be the first test string
    char entry[MAX_STRING_LENGTH];
    RC_t err = testLogger.get(0, entry, MAX_STRING_LENGTH);
    ASSERT_EQ(err, RC_SUCCESS);
    EXPECT_STREQ(entry, testStrings[0]);

    // test negative indexing
    err = testLogger.logLn("foo");
    ASSERT_EQ(err, RC_SUCCESS);
    err = testLogger.logLn("bar");
    ASSERT_EQ(err, RC_SUCCESS);
    err = testLogger.get(-1, entry, MAX_STRING_LENGTH);
    EXPECT_STREQ(entry, "bar");
    err = testLogger.get(-2, entry, MAX_STRING_LENGTH);
    EXPECT_STREQ(entry, "foo");

    // test string length checking
    err = testLogger.get(0, entry, MAX_STRING_LENGTH - 1);
    EXPECT_EQ(err, RC_ERROR_MEMORY);

    // clear testLogger buffer
    testLogger.clear();

    // Try retrieving from an empty buffer
    err = testLogger.get(0, entry, MAX_STRING_LENGTH);
    EXPECT_EQ(err, RC_ERROR_BUFFER_EMPTY);

    // try retrieving non-existent indexes
    err = testLogger.logLn("foo");
    ASSERT_EQ(err, RC_SUCCESS);
    err = testLogger.get(-2, entry, MAX_STRING_LENGTH);
    EXPECT_EQ(err, RC_ERROR_BAD_PARAM);
    err = testLogger.get(1, entry, MAX_STRING_LENGTH);
    EXPECT_EQ(err, RC_ERROR_BAD_PARAM);
}

/**
 * @brief Tests the correct calculation of each messages number
 */
TEST_F(RamLoggerTest, GetWithMsgNumberTest) {
    char entry[MAX_STRING_LENGTH];
    uint32_t msgId = 0;

    // First with negative indexing
    uint32_t expectedMsgID = NUM_START_ENTRIES - 1;
    RC_t err = testLogger.get(-1, entry, MAX_STRING_LENGTH, msgId);
    ASSERT_EQ(err, RC_SUCCESS);
    EXPECT_EQ(msgId, expectedMsgID);
    EXPECT_STREQ(entry, testStrings[NUM_START_ENTRIES - 1]);

    // Then with positive
    expectedMsgID = 0;
    err = testLogger.get(0, entry, MAX_STRING_LENGTH, msgId);
    ASSERT_EQ(err, RC_SUCCESS);
    EXPECT_EQ(msgId, expectedMsgID);
    EXPECT_STREQ(entry, testStrings[0]);
}

/**
 * @brief Test that the oldest string is replaced in the RamLogger
 * if the buffer is full
 */
TEST_F(RamLoggerTest, RingbufferingTest) {
    // fill up the rest of the RamLogger with teststrings
    ASSERT_EQ(MAX_STRING_LENGTH, testLogger.getMaxMsgLen());
    for(uint32_t i = NUM_START_ENTRIES; i < MAX_ENTRIES; i++) {
        RC_t err = testLogger.logLn(testStrings[i]);
        ASSERT_EQ(err, RC_SUCCESS);
    }
    ASSERT_TRUE(testLogger.isFull());
    ASSERT_EQ(testLogger.getMsgCounter(), MAX_ENTRIES);

    // Oldest string should now be the first test string
    char entry[MAX_STRING_LENGTH + 1];
    entry[MAX_STRING_LENGTH] = '\0';
    uint32_t msgId = 0;
    RC_t err = testLogger.get(0, entry, MAX_STRING_LENGTH, msgId);
    ASSERT_EQ(err, RC_SUCCESS);
    EXPECT_STREQ(entry, testStrings[0]);
    ASSERT_EQ(msgId, 0);

    // Now check that after logging 1 more string than the buffer can hold,
    // the oldest string is now the second test string since the oldest
    // was overwritten.
    err = testLogger.logLn("foo");
    ASSERT_EQ(err, RC_SUCCESS);
    err = testLogger.get(0, entry, MAX_STRING_LENGTH, msgId);
    ASSERT_EQ(err, RC_SUCCESS);
    EXPECT_STREQ(entry, testStrings[1]);
    EXPECT_EQ(msgId, 1);
}

/**
 * @brief Tests that the formatting using logf is equivalent to that of snprintf
 *
 */
TEST_F(RamLoggerTest, FormattedLogTest) {
    const uint32_t maxStringLength = 64;
    const uint32_t maxEntries = 3;
    RamLogger<maxEntries, maxStringLength, TIMESTAMP_STR_LEN> localTestLogger;
    ASSERT_EQ(maxStringLength, localTestLogger.getMaxMsgLen());
    ASSERT_EQ(maxEntries, localTestLogger.getMaxNumEntries());

    // check with logf
    char expected[maxStringLength];
    snprintf(expected, sizeof(expected), "Literally %u", 1984);
    char result[maxStringLength];
    RC_t err = localTestLogger.logf("Literally %u", 1984);
    ASSERT_EQ(RC_SUCCESS, err);
    err = localTestLogger.get(-1, result, sizeof(result));
    ASSERT_EQ(RC_SUCCESS, err);
    EXPECT_STREQ(result, expected);

    // check with logLnf
    memset(result, 0, strlen(result));
    snprintf(expected, sizeof(expected), "foo %u", 1984);
    localTestLogger.logLnf("foo %u", 1984);
    localTestLogger.get(-1, result, maxStringLength);
    EXPECT_STREQ(result, expected);
}