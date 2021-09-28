#pragma once

#include <queue>
#include <string>

#include "storm/storm.h"

namespace devilution {
namespace net {

typedef std::vector<unsigned char> buffer_t;
typedef unsigned long provider_t;

class loopback {
private:
	std::queue<buffer_t> message_queue;
	buffer_t message_last;

public:
	virtual bool SNetReceiveMessage(int *sender, void **data, uint32_t *size);
	virtual bool SNetSendMessage(int dest, void *data, unsigned int size);
	virtual bool SNetReceiveTurns(char **data, size_t *size);
	virtual void SNetGetProviderCaps(struct _SNETCAPS *caps);
};

} // namespace net
} // namespace devilution
