#ifndef MSG_QUEUE_TEST_H
#define MSG_QUEUE_TEST_H

#include "code_generator_tests/target_header_files_shadow/message_queue.h"
#include "code_generator_tests/mocks/mutual_exclusion_handler_mock.h"
#include <thread>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

namespace
{
}

TEST(MessageQueue, test1)
{
    MutualExclusionHandlerMock mutual_exclusion_handler;
    MessageQueue<unsigned int, MutualExclusionHandlerMock> msg_queue{mutual_exclusion_handler};
    std::vector<std::thread> producers;
    std::vector<unsigned int> values;
    std::thread consumer(
    [&values, &msg_queue]
    {
        while(values.size() < 100)
        {
            unsigned int data;
            msg_queue.receive(data);
            values.push_back(data);
        }
    });
    for(auto i = 0U; i < 10; ++i)
    {
        producers.emplace_back(
        [initial_value = i * 10, &msg_queue]
        {
            for(auto j = initial_value; j < initial_value + 10; ++j)
            {
                msg_queue.send(j);
            }
        });
    }
    for(auto&& producer : producers)
    {
        producer.join();
    }
    consumer.join();
    std::sort(values.begin(), values.end());
    for(auto i = 0U; i < 100; ++i)
    {
        ASSERT_EQ(values[i], i);
    }
}
#endif // MSG_QUEUE_TEST_H
