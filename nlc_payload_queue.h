#include "llc_array_ptr.h"
#include <mutex>

#ifndef NLC_PAYLOAD_QUEUE_H
#define NLC_PAYLOAD_QUEUE_H

namespace nlc
{
	stxp	uint8_t	MAX_MESSAGES = 100;
	using namespace llc;
	// Use this to store messages received from async communications to be picked up from loop.
	class AsyncPayloadQueue {
		typedef std::lock_guard<std::mutex>	lock_mutex;
		typedef	apobj<au0_t>				TPayloadList;

		std::mutex		QueueLock;	// Lock this when reading or writing to `Messages`.
		TPayloadList	Messages;	// Store here input payloads to be picked up in loop().
	public:
		// Use this to add a message to the queue. (used by the receiving comms handler)
		inline	error_t	PostMessage	(pobj<asc_t> msg)	{ return PostMessage(*(pobj<au0_t>*)&msg); }
		error_t			PostMessage	(pobj<au0_t> msg)	{
			if_fail_fef(Messages.size() >= MAX_MESSAGES, "MAX_MESSAGES:%i.", MAX_MESSAGES);
			lock_mutex		lock(QueueLock);
			if_fail_fe(Messages.push_back({msg}));
			return Messages.size();
		}
		inline	error_t	PostMessage	(vcsc_t msg)	{ return PostMessage(msg.cu8()); }
		error_t			PostMessage	(vcu0_t msg)	{
			pobj<au0_t>	pmsg;
			if_fail_fe(0 == pmsg.create());
			if_fail_fe(pmsg->append(msg));
			return PostMessage(pmsg);
		}
		// Use this to retrieve the messages from the queue. (from the main thread's application loop)
		error_t			GetMessage	(pobj<au0_t> & msg)			{
			lock_mutex		lock(QueueLock);
			if(0 == Messages.size())
				return 0;
			msg	= Messages[0];
			if_fail_fe(Messages.remove_unordered(0));
			return msg->size();
		}
		ndin	error_t	Empty	()	{ lock_mutex lock(QueueLock); return 0 == Messages.size(); }
		ndin	error_t	Size	()	{ lock_mutex lock(QueueLock); return Messages.size(); }
	};

    struct AsyncQueuePair {
        AsyncPayloadQueue	Outbox	; // loop() stores here the payloads to be sent to the target (like the SOS or another NIO).
        AsyncPayloadQueue	Inbox	; // Store here received payloads to be picked up in loop().
    };

} // namespace

#endif // NLC_PAYLOAD_QUEUE_H
