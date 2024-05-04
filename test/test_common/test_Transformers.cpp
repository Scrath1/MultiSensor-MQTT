#include <gtest/gtest.h>
#include "transformers/SimpleMovingAverageFilter.h"
#include "transformers/Remapper.h"

// Baseline from Arduino for comparison against reimplementation in Remapper
// https://www.arduino.cc/reference/en/language/functions/math/map/
float_t remap(float_t x, float_t in_min, float_t in_max, float_t out_min, float_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

TEST(Transformers, SimpleMovingAverage){
    SimpleMovingAverageFilter sma(4);

    // The first call to this objects filter function
    // completely fills the buffer with its input
    // to provide a baseline average
    uint16_t ret = sma.applyTransformations(128);
    uint16_t exp = 128;
    ASSERT_EQ(ret, exp);
    
    ret = sma.applyTransformations(256);
    exp = (128 * 3 + 256) / 4;
    ASSERT_EQ(ret, exp);

    ret = sma.applyTransformations(512);
    exp = (128 * 2 + 256 + 512) / 4;
    ASSERT_EQ(ret, exp);

    ret = sma.applyTransformations(1024);
    exp = (128 + 256 + 512 + 1024) / 4;
    ASSERT_EQ(ret, exp);

    ret = sma.applyTransformations(2048);
    exp = (256 + 512 + 1024 + 2048) / 4;
    ASSERT_EQ(ret, exp);
}

TEST(Transformers, Remapper){
    Remapper remapper(0, UINT16_MAX, 0, 100);
    float_t ret = remapper.applyTransformations(0);
    float_t exp = remap(0, 0, UINT16_MAX, 0, 100);
    ASSERT_EQ(ret, exp);

    ret = remapper.applyTransformations(65535);
    exp = remap(65535, 0, UINT16_MAX, 0, 100);
    ASSERT_EQ(ret, exp);

    ret = remapper.applyTransformations(32768);
    exp = remap(32768, 0, UINT16_MAX, 0, 100);
    ASSERT_EQ(ret, exp);

    // Second test to verify inverting the range also works without over/underflows
    Remapper remapper2(0, UINT16_MAX, 100, 0);
    ret = remapper2.applyTransformations(0);
    exp = remap(0, 0, UINT16_MAX, 100, 0);
    ASSERT_EQ(ret, exp);
}

TEST(Transformers, Pipelining){
    std::shared_ptr<Transformer> remapper = std::make_shared<Remapper>(128, 2048, 0, 100);
    Transformer* sma = new SimpleMovingAverageFilter(4, remapper);

    float_t ret = sma->applyTransformations(128);
    float_t exp = 128;
    exp = remap(exp, 128, 2048, 0, 100);
    ASSERT_EQ(ret, exp);

    ret = sma->applyTransformations(256);
    exp = (128 * 3 + 256) / 4;
    exp = remap(exp, 128, 2048, 0, 100);
    ASSERT_EQ(ret, exp);

    ret = sma->applyTransformations(512);
    exp = (128 * 2 + 256 + 512) / 4;
    exp = remap(exp, 128, 2048, 0, 100);
    ASSERT_EQ(ret, exp);

    ret = sma->applyTransformations(1024);
    exp = (128 + 256 + 512 + 1024) / 4;
    exp = remap(exp, 128, 2048, 0, 100);
    ASSERT_EQ(ret, exp);

    ret = sma->applyTransformations(2048);
    exp = (256 + 512 + 1024 + 2048) / 4;
    exp = remap(exp, 128, 2048, 0, 100);
    ASSERT_EQ(ret, exp);
}