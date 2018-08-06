#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

#include "tpl_os.h"

// Parameter size must be a power of 2
template <typename T, unsigned int n_senders, unsigned int n_receivers, unsigned int size = 8>
class MessageQueue
{
public:

	MessageQueue(ResourceType resource, EventMaskType empty, EventMaskType full, const TaskType senders[n_senders], const TaskType receivers[n_receivers])
		: insert_index(0), remove_index(0), resource(resource), buffer_empty(empty), buffer_full(full), senders(senders), receivers(receivers)
	{}
	
	void send(const T& data)
	{
		while(1)
		{
			GetResource(resource);
			if((insert_index - remove_index) & ~(size-1))
			{
				ClearEvent(buffer_full);
				ReleaseResource(resource);
				WaitEvent(buffer_full);
			} else
			{
				break;
			}
		}
		buffer[insert_index & (size-1)] = data;
		insert_index++;
		for(unsigned int i = 0; i < n_receivers; ++i)
		{
			SetEvent(receivers[i], buffer_empty);
		}
		ReleaseResource(resource);
	}
	
	void receive(T& data)
	{
		while(1)
		{			
			GetResource(resource);
			if(insert_index == remove_index)
			{
				ClearEvent(buffer_empty);
				ReleaseResource(resource);
				WaitEvent(buffer_empty);
			} else
			{
				break;
			}
		}
		data = buffer[remove_index & (size-1)];
		remove_index++;
		for(unsigned int i = 0; i < n_senders; ++i)
		{
			SetEvent(senders[i], buffer_full);
		}
		ReleaseResource(resource);
	}
private:
	unsigned int insert_index;
	unsigned int remove_index;
	T buffer[size];
	ResourceType resource;
	EventMaskType buffer_empty;
	EventMaskType buffer_full;
	const TaskType* senders;
	const TaskType* receivers;
};

#endif // MESSAGE_QUEUE_H