#include "dvlnet/abstract_net.h"

#include "utils/stubs.h"
#ifndef NONET
#include "dvlnet/base_protocol.h"
#include "dvlnet/cdwrap.h"
#endif
#include "dvlnet/loopback.h"
#include "storm/storm.h"

namespace devilution {
namespace net {

std::unique_ptr<abstract_net> abstract_net::MakeNet(provider_t provider)
{
#ifdef NONET
	return std::make_unique<loopback>();
#else
	switch (provider) {
	case SELCONN_LOOPBACK:
		return std::make_unique<loopback>();
	default:
		ABORT();
	}
#endif
}

} // namespace net
} // namespace devilution
