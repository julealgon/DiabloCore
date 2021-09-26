#include <memory>

#include "dvlnet/abstract_net.h"
#include "menu.h"
#include "options.h"
#include "utils/stubs.h"

namespace devilution {

static std::unique_ptr<net::abstract_net> dvlnet_inst;

bool SNetReceiveMessage(int *senderplayerid, void **data, uint32_t *databytes)
{
	if (!dvlnet_inst->SNetReceiveMessage(senderplayerid, data, databytes)) {
		SErrSetLastError(STORM_ERROR_NO_MESSAGES_WAITING);
		return false;
	}
	return true;
}

bool SNetSendMessage(int playerID, void *data, unsigned int databytes)
{
	return dvlnet_inst->SNetSendMessage(playerID, data, databytes);
}

bool SNetReceiveTurns(int arraysize, char **arraydata, size_t *arraydatabytes)
{
	if (arraysize != MAX_PLRS)
		UNIMPLEMENTED();
	if (!dvlnet_inst->SNetReceiveTurns(arraydata, arraydatabytes)) {
		SErrSetLastError(STORM_ERROR_NO_MESSAGES_WAITING);
		return false;
	}
	return true;
}

void SNetGetProviderCaps(struct _SNETCAPS *caps)
{
	dvlnet_inst->SNetGetProviderCaps(caps);
}

/**
 * @brief Called by engine for single, called by ui for multi
 * @param provider BNET, IPXN, MODM, SCBL or UDPN
 */
bool SNetInitializeProvider(uint32_t provider, struct GameData *gameData)
{
	dvlnet_inst = net::abstract_net::MakeNet(provider);
	return mainmenu_select_hero_dialog(gameData);
}

/**
 * @brief Called by engine for single, called by ui for multi
 */
bool SNetCreateGame(char *gameTemplateData, int gameTemplateSize)
{
	if (gameTemplateSize != sizeof(GameData))
		ABORT();
	net::buffer_t gameInitInfo(gameTemplateData, gameTemplateData + gameTemplateSize);

	return true;
}

} // namespace devilution
