#ifndef MUTUAL_EXCLUSION_HANDLER_MOCK_H
#define MUTUAL_EXCLUSION_HANDLER_MOCK_H

#include "code_generator_tests/target_header_files_shadow/message_queue.h"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <map>

class MutualExclusionHandlerMock
{
public:
    MutualExclusionHandlerMock()
        : m{}
    {}

    void acquire_resource()
    {
        get_lock_instance(std::this_thread::get_id()).lock();
    }

    void release_resource()
    {
        get_lock_instance(std::this_thread::get_id()).unlock();
    }

    void wait_buffer_full_signal()
    {
        buffer_full.wait(get_lock_instance(std::this_thread::get_id()));
    }

    void wait_buffer_empty_signal()
    {
        buffer_empty.wait(get_lock_instance(std::this_thread::get_id()));
    }

    void broadcast_buffer_full_signal()
    {
        buffer_full.notify_all();
    }

    void broadcast_buffer_empty_signal()
    {
        buffer_empty.notify_all();
    }

private:

    std::unique_lock<std::mutex>& get_lock_instance(std::thread::id id)
    {
        if(thread_lock_map.find(id) == thread_lock_map.end())
        {
            thread_lock_map.insert(std::make_pair(id, std::unique_lock<std::mutex>(m, std::defer_lock)));
        }

        return thread_lock_map.at(id);
    }

    std::mutex m;
    std::map<std::thread::id, std::unique_lock<std::mutex>> thread_lock_map;
    std::condition_variable buffer_empty;
    std::condition_variable buffer_full;
};

#endif // MUTUAL_EXCLUSION_HANDLER_MOCK_H
