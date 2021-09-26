#pragma once

#include <queue>
#include <string>

#include "dvlnet/abstract_net.h"

namespace devilution {
namespace net {

class loopback : public abstract_net {
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
