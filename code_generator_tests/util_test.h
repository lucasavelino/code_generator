#ifndef UTIL_TEST_H
#define UTIL_TEST_H

#include "util.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <QTemporaryDir>

inline QString create_temporary_file(const QString& resource, const QTemporaryDir& temporary_dir, const QString& name)
{
    QString file_name(temporary_dir.path() + "/" + name);
    QFile::copy(resource, file_name);
    return file_name;
}

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

TEST(ReadDef,test1)
{
    std::vector<code_generator::ast::TimerHandler> timer_handlers;
    std::vector<code_generator::ast::KeyHandler> key_handlers;
    code_generator::ast::MessageHandlerPgnAll msg_handler_pgn_all;
    QTemporaryDir temporary_dir;
    auto file_name = create_temporary_file(R"(:/test/files/test1.def)", temporary_dir, "test1.def");
    std::tie(timer_handlers, key_handlers, msg_handler_pgn_all) = code_generator::util::read_def(file_name.toStdString());
    ASSERT_EQ(timer_handlers.size(), 1);
    ASSERT_EQ(key_handlers.size(), 2);
    ASSERT_EQ(timer_handlers[0].name, "EEC1");
    ASSERT_EQ(timer_handlers[0].milliseconds, 20);
    ASSERT_EQ(key_handlers[0].key, "i");
    ASSERT_EQ(key_handlers[1].key, "d");
    ASSERT_EQ(msg_handler_pgn_all.declared, false);
}

TEST(ReadDef,test2)
{
    std::vector<code_generator::ast::TimerHandler> timer_handlers;
    std::vector<code_generator::ast::KeyHandler> key_handlers;
    code_generator::ast::MessageHandlerPgnAll msg_handler_pgn_all;
    QTemporaryDir temporary_dir;
    auto file_name = create_temporary_file(R"(:/test/files/test2.def)", temporary_dir, "test2.def");
    std::tie(timer_handlers, key_handlers, msg_handler_pgn_all) = code_generator::util::read_def(file_name.toStdString());
    ASSERT_EQ(timer_handlers.size(), 1);
    ASSERT_EQ(key_handlers.size(), 1);
    ASSERT_EQ(timer_handlers[0].name, "Test");
    ASSERT_EQ(timer_handlers[0].milliseconds, 33);
    ASSERT_EQ(key_handlers[0].key, "t");
    ASSERT_EQ(msg_handler_pgn_all.declared, true);
}

TEST(ReadDbf,test1)
{
    QTemporaryDir temporary_dir;
    auto file_name = create_temporary_file(R"(:/test/files/New_DatabaseJ19391.DBF)", temporary_dir, "New_DatabaseJ19391.DBF");
    auto messages = code_generator::util::read_dbf(file_name.toStdString());
    ASSERT_EQ(messages.size(), 4);

    ASSERT_EQ(messages[0].name, "EEC1");
    ASSERT_EQ(messages[0].pgn, 61444);
    ASSERT_EQ(messages[0].length, 8);
    ASSERT_EQ(messages[0].n_signals, 1);
    ASSERT_EQ(messages[0].signals.size(), 1);
    ASSERT_EQ(messages[0].signals[0].name, "mEngineSpeed");
    ASSERT_EQ(messages[0].signals[0].length, 16);
    ASSERT_EQ(messages[0].signals[0].byte_index, 4);
    ASSERT_EQ(messages[0].signals[0].bit_no, 0);
    ASSERT_EQ(messages[0].signals[0].type, 'U');
    ASSERT_EQ(messages[0].signals[0].max_val, 65535);
    ASSERT_EQ(messages[0].signals[0].min_val, 0);
    ASSERT_EQ(messages[0].signals[0].byte_order, 1);
    ASSERT_NEAR(messages[0].signals[0].offset, 0.0, 0.0000001);
    ASSERT_NEAR(messages[0].signals[0].scale_factor, 0.125, 0.0000001);

    ASSERT_EQ(messages[1].name, "EngineTemperature");
    ASSERT_EQ(messages[1].pgn, 65262);
    ASSERT_EQ(messages[1].length, 8);
    ASSERT_EQ(messages[1].n_signals, 1);
    ASSERT_EQ(messages[1].signals.size(), 1);
    ASSERT_EQ(messages[1].signals[0].name, "mEngineTemp");
    ASSERT_EQ(messages[1].signals[0].length, 8);
    ASSERT_EQ(messages[1].signals[0].byte_index, 1);
    ASSERT_EQ(messages[1].signals[0].bit_no, 0);
    ASSERT_EQ(messages[1].signals[0].type, 'U');
    ASSERT_EQ(messages[1].signals[0].max_val, 255);
    ASSERT_EQ(messages[1].signals[0].min_val, 0);
    ASSERT_EQ(messages[1].signals[0].byte_order, 1);
    ASSERT_NEAR(messages[1].signals[0].offset, 0.0, 0.0000001);
    ASSERT_NEAR(messages[1].signals[0].scale_factor, -40.0, 0.0000001);

    ASSERT_EQ(messages[2].name, "TCO1");
    ASSERT_EQ(messages[2].pgn, 65132);
    ASSERT_EQ(messages[2].length, 8);
    ASSERT_EQ(messages[2].n_signals, 1);
    ASSERT_EQ(messages[2].signals.size(), 1);
    ASSERT_EQ(messages[2].signals[0].name, "mTachSpeed");
    ASSERT_EQ(messages[2].signals[0].length, 16);
    ASSERT_EQ(messages[2].signals[0].byte_index, 7);
    ASSERT_EQ(messages[2].signals[0].bit_no, 0);
    ASSERT_EQ(messages[2].signals[0].type, 'U');
    ASSERT_EQ(messages[2].signals[0].max_val, 65535);
    ASSERT_EQ(messages[2].signals[0].min_val, 0);
    ASSERT_EQ(messages[2].signals[0].byte_order, 1);
    ASSERT_NEAR(messages[2].signals[0].offset, 0.0, 0.0000001);
    ASSERT_NEAR(messages[2].signals[0].scale_factor, 0.00391, 0.0000001);

    ASSERT_EQ(messages[3].name, "SoftwareIdentification");
    ASSERT_EQ(messages[3].pgn, 65242);
    ASSERT_EQ(messages[3].length, 8);
    ASSERT_EQ(messages[3].n_signals, 1);
    ASSERT_EQ(messages[3].signals.size(), 1);
    ASSERT_EQ(messages[3].signals[0].name, "mSoftId");
    ASSERT_EQ(messages[3].signals[0].length, 8);
    ASSERT_EQ(messages[3].signals[0].byte_index, 1);
    ASSERT_EQ(messages[3].signals[0].bit_no, 0);
    ASSERT_EQ(messages[3].signals[0].type, 'U');
    ASSERT_EQ(messages[3].signals[0].max_val, 255);
    ASSERT_EQ(messages[3].signals[0].min_val, 0);
    ASSERT_EQ(messages[3].signals[0].byte_order, 1);
    ASSERT_NEAR(messages[3].signals[0].offset, 0.0, 0.0000001);
    ASSERT_NEAR(messages[3].signals[0].scale_factor, 1.0, 0.0000001);
}

TEST(ReadDbf,test2)
{
    QTemporaryDir temporary_dir;
    auto file_name = create_temporary_file(R"(:/test/files/test1.DBF)", temporary_dir, "test1.DBF");
    auto messages = code_generator::util::read_dbf(file_name.toStdString());
    ASSERT_EQ(messages.size(), 2);

    ASSERT_EQ(messages[0].name, "Msg1");
    ASSERT_EQ(messages[0].pgn, 291);
    ASSERT_EQ(messages[0].length, 8);
    ASSERT_EQ(messages[0].n_signals, 6);
    ASSERT_EQ(messages[0].signals.size(), 6);

    ASSERT_EQ(messages[0].signals[0].name, "Sig1");
    ASSERT_EQ(messages[0].signals[0].length, 15);
    ASSERT_EQ(messages[0].signals[0].byte_index, 1);
    ASSERT_EQ(messages[0].signals[0].bit_no, 0);
    ASSERT_EQ(messages[0].signals[0].type, 'I');
    ASSERT_EQ(messages[0].signals[0].max_val, 16383);
    ASSERT_EQ(messages[0].signals[0].min_val, -16384);
    ASSERT_EQ(messages[0].signals[0].byte_order, 1);
    ASSERT_NEAR(messages[0].signals[0].offset, -2.048000, 0.0000001);
    ASSERT_NEAR(messages[0].signals[0].scale_factor, 4.096000, 0.0000001);

    ASSERT_EQ(messages[0].signals[1].name, "Sig2");
    ASSERT_EQ(messages[0].signals[1].length, 1);
    ASSERT_EQ(messages[0].signals[1].byte_index, 2);
    ASSERT_EQ(messages[0].signals[1].bit_no, 7);
    ASSERT_EQ(messages[0].signals[1].type, 'B');
    ASSERT_EQ(messages[0].signals[1].max_val, 1);
    ASSERT_EQ(messages[0].signals[1].min_val, 0);
    ASSERT_EQ(messages[0].signals[1].byte_order, 1);
    ASSERT_NEAR(messages[0].signals[1].offset, 0.000000, 0.0000001);
    ASSERT_NEAR(messages[0].signals[1].scale_factor, 1.000000, 0.0000001);

    ASSERT_EQ(messages[0].signals[2].name, "Sig3");
    ASSERT_EQ(messages[0].signals[2].length, 2);
    ASSERT_EQ(messages[0].signals[2].byte_index, 3);
    ASSERT_EQ(messages[0].signals[2].bit_no, 0);
    ASSERT_EQ(messages[0].signals[2].type, 'U');
    ASSERT_EQ(messages[0].signals[2].max_val, 3);
    ASSERT_EQ(messages[0].signals[2].min_val, 0);
    ASSERT_EQ(messages[0].signals[2].byte_order, 1);
    ASSERT_NEAR(messages[0].signals[2].offset, 0.000000, 0.0000001);
    ASSERT_NEAR(messages[0].signals[2].scale_factor, 1.000000, 0.0000001);

    ASSERT_EQ(messages[0].signals[3].name, "Sig4");
    ASSERT_EQ(messages[0].signals[3].length, 6);
    ASSERT_EQ(messages[0].signals[3].byte_index, 3);
    ASSERT_EQ(messages[0].signals[3].bit_no, 2);
    ASSERT_EQ(messages[0].signals[3].type, 'I');
    ASSERT_EQ(messages[0].signals[3].max_val, 31);
    ASSERT_EQ(messages[0].signals[3].min_val, -32);
    ASSERT_EQ(messages[0].signals[3].byte_order, 1);
    ASSERT_NEAR(messages[0].signals[3].offset, 0.000000, 0.0000001);
    ASSERT_NEAR(messages[0].signals[3].scale_factor, 256.000000, 0.0000001);

    ASSERT_EQ(messages[0].signals[4].name, "Sig5");
    ASSERT_EQ(messages[0].signals[4].length, 1);
    ASSERT_EQ(messages[0].signals[4].byte_index, 4);
    ASSERT_EQ(messages[0].signals[4].bit_no, 0);
    ASSERT_EQ(messages[0].signals[4].type, 'B');
    ASSERT_EQ(messages[0].signals[4].max_val, 1);
    ASSERT_EQ(messages[0].signals[4].min_val, 0);
    ASSERT_EQ(messages[0].signals[4].byte_order, 1);
    ASSERT_NEAR(messages[0].signals[4].offset, 0.000000, 0.0000001);
    ASSERT_NEAR(messages[0].signals[4].scale_factor, 1.000000, 0.0000001);

    ASSERT_EQ(messages[0].signals[5].name, "Sig6");
    ASSERT_EQ(messages[0].signals[5].length, 39);
    ASSERT_EQ(messages[0].signals[5].byte_index, 4);
    ASSERT_EQ(messages[0].signals[5].bit_no, 1);
    ASSERT_EQ(messages[0].signals[5].type, 'U');
    ASSERT_EQ(messages[0].signals[5].max_val, 549755813887);
    ASSERT_EQ(messages[0].signals[5].min_val, 0);
    ASSERT_EQ(messages[0].signals[5].byte_order, 1);
    ASSERT_NEAR(messages[0].signals[5].offset, -50.000000, 0.0000001);
    ASSERT_NEAR(messages[0].signals[5].scale_factor, 0.102400, 0.0000001);

    ASSERT_EQ(messages[1].name, "Msg2");
    ASSERT_EQ(messages[1].pgn, 801);
    ASSERT_EQ(messages[1].length, 8);
    ASSERT_EQ(messages[1].n_signals, 1);
    ASSERT_EQ(messages[1].signals.size(), 1);

    ASSERT_EQ(messages[1].signals[0].name, "Sig7");
    ASSERT_EQ(messages[1].signals[0].length, 64);
    ASSERT_EQ(messages[1].signals[0].byte_index, 1);
    ASSERT_EQ(messages[1].signals[0].bit_no, 0);
    ASSERT_EQ(messages[1].signals[0].type, 'I');
    ASSERT_EQ(messages[1].signals[0].max_val, 9223372036854775807);
    ASSERT_EQ(messages[1].signals[0].min_val, -9223372036854775808);
    ASSERT_EQ(messages[1].signals[0].byte_order, 1);
    ASSERT_NEAR(messages[1].signals[0].offset, -3.000000, 0.0000001);
    ASSERT_NEAR(messages[1].signals[0].scale_factor, 3.125000, 0.0000001);
}

#endif // UTIL_TEST_H
