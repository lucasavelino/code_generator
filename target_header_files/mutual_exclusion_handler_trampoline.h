#ifndef MUTUAL_EXCLUSION_HANDLER_TRAMPOLINE_H
#define MUTUAL_EXCLUSION_HANDLER_TRAMPOLINE_H

#include "tpl_os.h"
#include "message_queue.h"

template <unsigned int n_senders, unsigned int n_receivers>
class MutualExclusionHandlerTrampoline
{
public:
    MutualExclusionHandlerTrampoline(ResourceType resource, EventMaskType empty, EventMaskType full, const TaskType* senders, const TaskType* receivers)
        : resource(resource), buffer_empty(empty), buffer_full(full), senders(senders), receivers(receivers)
    {}

    void acquire_resource() const
    {
        GetResource(resource);
    }

    void release_resource() const
    {
        ReleaseResource(resource);
    }

    void wait_buffer_full_signal() const
    {
        ClearEvent(buffer_full);
        ReleaseResource(resource);
        WaitEvent(buffer_full);
        GetResource(resource);
    }

    void wait_buffer_empty_signal() const
    {
        ClearEvent(buffer_empty);
        ReleaseResource(resource);
        WaitEvent(buffer_empty);
        GetResource(resource);
    }

    void broadcast_buffer_full_signal() const
    {
        for(unsigned int i = 0; i < n_senders; ++i)
        {
            SetEvent(senders[i], buffer_full);
        }
    }

    void broadcast_buffer_empty_signal() const
    {
        for(unsigned int i = 0; i < n_receivers; ++i)
        {
            SetEvent(receivers[i], buffer_empty);
        }
    }

private:
    const ResourceType resource;
    const EventMaskType buffer_empty;
    const EventMaskType buffer_full;
    const TaskType* senders;
    const TaskType* receivers;
};

#endif // MUTUAL_EXCLUSION_HANDLER_TRAMPOLINE_H
