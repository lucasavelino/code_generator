#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "util_test.h"
#include "msg_queue_test.h"

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    RUN_ALL_TESTS();
    return 0;
}
