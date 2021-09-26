#include "dvlnet/abstract_net.h"

#include "utils/stubs.h"
#include "dvlnet/loopback.h"
#include "storm/storm.h"

namespace devilution {
namespace net {

std::unique_ptr<abstract_net> abstract_net::MakeNet(provider_t provider)
{
	return std::make_unique<loopback>();
}

} // namespace net
} // namespace devilution
