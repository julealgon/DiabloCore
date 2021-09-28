#pragma once

#include <queue>

#include "storm/storm.h"

namespace devilution {
namespace net {

typedef std::vector<unsigned char> buffer_t;

class loopback {
private:
	std::queue<buffer_t> message_queue;
	buffer_t message_last;

public:
	bool SNetReceiveMessage(int *sender, void **data, uint32_t *size);
	bool SNetSendMessage(int dest, void *data, unsigned int size);
	bool SNetReceiveTurns(char **data, size_t *size);
	void SNetGetProviderCaps(struct _SNETCAPS *caps);
};

} // namespace net
} // namespace devilution
