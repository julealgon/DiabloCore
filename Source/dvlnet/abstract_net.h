#pragma once

#include <exception>
#include <memory>
#include <string>
#include <vector>

#include "storm/storm.h"

namespace devilution {
namespace net {

typedef std::vector<unsigned char> buffer_t;
typedef unsigned long provider_t;

class abstract_net {
public:
	virtual bool SNetReceiveMessage(int *sender, void **data, uint32_t *size) = 0;
	virtual bool SNetSendMessage(int dest, void *data, unsigned int size) = 0;
	virtual bool SNetReceiveTurns(char **data, size_t *size) = 0;
	virtual void SNetGetProviderCaps(struct _SNETCAPS *caps) = 0;
	virtual ~abstract_net() = default;

	static std::unique_ptr<abstract_net> MakeNet(provider_t provider);
};

} // namespace net
} // namespace devilution
