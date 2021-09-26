#include "dvlnet/loopback.h"
#include "utils/stubs.h"

namespace devilution {
namespace net {

bool loopback::SNetReceiveMessage(int *sender, void **data, uint32_t *size)
{
	if (message_queue.empty())
		return false;
	message_last = message_queue.front();
	message_queue.pop();
	*sender = 0;
	*size = message_last.size();
	*data = message_last.data();
	return true;
}

bool loopback::SNetSendMessage(int dest, void *data, unsigned int size)
{
	if (dest == 0 || dest == SNPLAYER_ALL) {
		auto *rawMessage = reinterpret_cast<unsigned char *>(data);
		buffer_t message(rawMessage, rawMessage + size);
		message_queue.push(message);
	}
	return true;
}

bool loopback::SNetReceiveTurns(char **data, size_t *size)
{
	for (auto i = 0; i < MAX_PLRS; ++i) {
		size[i] = 0;
		data[i] = nullptr;
	}
	return true;
}

void loopback::SNetGetProviderCaps(struct _SNETCAPS *caps)
{
	caps->size = 0;                  // engine writes only ?!?
	caps->flags = 0;                 // unused
	caps->maxmessagesize = 512;      // capped to 512; underflow if < 24
	caps->maxqueuesize = 0;          // unused
	caps->maxplayers = MAX_PLRS;     // capped to 4
	caps->bytessec = 1000000;        // ?
	caps->latencyms = 0;             // unused
	caps->defaultturnssec = 10;      // ?
	caps->defaultturnsintransit = 1; // maximum acceptable number
	                                 // of turns in queue?
}

} // namespace net
} // namespace devilution
