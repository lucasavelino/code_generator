#include "util.h"
#include "gtest/gtest.h"
#include "gmock/gmock.h"


TEST(GetSignalType,test)
{
    code_generator::ast::Signal sig;
    sig.type = 'U';
    sig.length = 1;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "uint8_t");
    sig.length = 8;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "uint8_t");
    sig.length = 9;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "uint16_t");
    sig.length = 16;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "uint16_t");
    sig.length = 17;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "uint32_t");
    sig.length = 32;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "uint32_t");
    sig.length = 33;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "uint64_t");
    sig.length = 64;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "uint64_t");
    sig.type = 'I';
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "int64_t");
    sig.length = 33;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "int64_t");
    sig.length = 32;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "int32_t");
    sig.length = 17;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "int32_t");
    sig.length = 16;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "int16_t");
    sig.length = 9;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "int16_t");
    sig.length = 8;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "int8_t");
    sig.length = 1;
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "int8_t");
    sig.type = 'B';
    ASSERT_EQ(code_generator::util::get_appropriate_signal_type(sig), "bool");
}