#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

namespace meta
{
    template <unsigned int N>
    struct is_power_of_2
    {
        static const bool value = (N & (N-1)) == 0;
    };

    template <bool>
    struct required
    {};

    template <>
    struct required<false>;
}

// Parameter size must be positive and a power of 2
template <typename T, typename MutualExclusionHandler, unsigned int size = 4>
class MessageQueue : meta::required< meta::is_power_of_2<size>::value >
{
public:

    MessageQueue(MutualExclusionHandler& mutual_exclusion_handler)
        : insert_index(0), remove_index(0), mutual_exclusion_handler(mutual_exclusion_handler)
    {}

    void send(const T& data)
    {
        mutual_exclusion_handler.acquire_resource();
        while((insert_index - remove_index) & ~(size-1))
        {
            mutual_exclusion_handler.wait_buffer_full_signal();
        }
        buffer[insert_index & (size-1)] = data;
        insert_index++;
        mutual_exclusion_handler.broadcast_buffer_empty_signal();
        mutual_exclusion_handler.release_resource();
    }

    void receive(T& data)
    {
        mutual_exclusion_handler.acquire_resource();
        while(insert_index == remove_index)
        {
            mutual_exclusion_handler.wait_buffer_empty_signal();
        }
        data = buffer[remove_index & (size-1)];
        remove_index++;
        mutual_exclusion_handler.broadcast_buffer_full_signal();
        mutual_exclusion_handler.release_resource();
    }
private:
    unsigned int insert_index;
    unsigned int remove_index;
    T buffer[size];
    MutualExclusionHandler& mutual_exclusion_handler;
};

#endif // MESSAGE_QUEUE_H
