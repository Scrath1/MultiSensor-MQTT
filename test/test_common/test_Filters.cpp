#include <gtest/gtest.h>
#include "filters/SimpleMovingAverageFilter.h"
#include "filters/NoFilter.h"

TEST(Filters, NoFilter){
    const uint16_t input = 512;
    ASSERT_EQ(input, NoFilter::instance.get()->filter(input));
    ASSERT_EQ(input, NoFilter::instance.get()->filter(input));
}

TEST(Filters, SimpleMovingAverage){
    SimpleMovingAverageFilter sma(4);

    // The first call to this objects filter function
    // completely fills the buffer with its input
    // to provide a baseline average
    uint16_t ret = sma.filter(128);
    uint16_t exp = 128;
    ASSERT_EQ(ret, exp);
    
    ret = sma.filter(256);
    exp = (128 * 3 + 256) / 4;
    ASSERT_EQ(ret, exp);

    ret = sma.filter(512);
    exp = (128 * 2 + 256 + 512) / 4;
    ASSERT_EQ(ret, exp);

    ret = sma.filter(1024);
    exp = (128 + 256 + 512 + 1024) / 4;
    ASSERT_EQ(ret, exp);

    ret = sma.filter(2048);
    exp = (256 + 512 + 1024 + 2048) / 4;
    ASSERT_EQ(ret, exp);
}