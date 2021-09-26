#include <memory>

#include "dvlnet/abstract_net.h"
#include "menu.h"
#include "options.h"
#include "storm/storm_dvlnet.h"
#include "utils/stubs.h"

namespace devilution {

static std::unique_ptr<net::abstract_net> dvlnet_inst;
static char gpszGameName[128] = {};
static char gpszGamePassword[128] = {};

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

bool SNetReceiveTurns(int arraysize, char **arraydata, size_t *arraydatabytes, uint32_t *arrayplayerstatus)
{
	if (arraysize != MAX_PLRS)
		UNIMPLEMENTED();
	if (!dvlnet_inst->SNetReceiveTurns(arraydata, arraydatabytes, arrayplayerstatus)) {
		SErrSetLastError(STORM_ERROR_NO_MESSAGES_WAITING);
		return false;
	}
	return true;
}

bool SNetSendTurn(char *data, unsigned int databytes)
{
	return dvlnet_inst->SNetSendTurn(data, databytes);
}

void SNetGetProviderCaps(struct _SNETCAPS *caps)
{
	dvlnet_inst->SNetGetProviderCaps(caps);
}

bool SNetUnregisterEventHandler(event_type evtype)
{
	return dvlnet_inst->SNetUnregisterEventHandler(evtype);
}

bool SNetRegisterEventHandler(event_type evtype, SEVTHANDLER func)
{
	return dvlnet_inst->SNetRegisterEventHandler(evtype, func);
}

bool SNetDestroy()
{
	return true;
}

bool SNetDropPlayer(int playerid, uint32_t flags)
{
	return dvlnet_inst->SNetDropPlayer(playerid, flags);
}

bool SNetGetGameInfo(game_info type, void *dst, unsigned int length)
{
	switch (type) {
	case GAMEINFO_NAME:
		strncpy((char *)dst, gpszGameName, length);
		break;
	case GAMEINFO_PASSWORD:
		strncpy((char *)dst, gpszGamePassword, length);
		break;
	}

	return true;
}

bool SNetLeaveGame(int type)
{
	if (dvlnet_inst == nullptr)
		return true;
	return dvlnet_inst->SNetLeaveGame(type);
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
bool SNetCreateGame(const char *pszGameName, const char *pszGamePassword, char *gameTemplateData, int gameTemplateSize, int *playerID)
{
	if (gameTemplateSize != sizeof(GameData))
		ABORT();
	net::buffer_t gameInitInfo(gameTemplateData, gameTemplateData + gameTemplateSize);
	dvlnet_inst->setup_gameinfo(std::move(gameInitInfo));

	std::string defaultName;
	if (pszGameName == nullptr) {
		defaultName = dvlnet_inst->make_default_gamename();
		pszGameName = defaultName.c_str();
	}

	strncpy(gpszGameName, pszGameName, sizeof(gpszGameName) - 1);
	if (pszGamePassword != nullptr)
		strncpy(gpszGamePassword, pszGamePassword, sizeof(gpszGamePassword) - 1);
	*playerID = dvlnet_inst->create(pszGameName, pszGamePassword);
	return *playerID != -1;
}

bool SNetJoinGame(char *pszGameName, char *pszGamePassword, int *playerID)
{
	if (pszGameName != nullptr)
		strncpy(gpszGameName, pszGameName, sizeof(gpszGameName) - 1);
	if (pszGamePassword != nullptr)
		strncpy(gpszGamePassword, pszGamePassword, sizeof(gpszGamePassword) - 1);
	*playerID = dvlnet_inst->join(pszGameName, pszGamePassword);
	return *playerID != -1;
}

/**
 * @brief Is this the mirror image of SNetGetTurnsInTransit?
 */
bool SNetGetOwnerTurnsWaiting(uint32_t *turns)
{
	return dvlnet_inst->SNetGetOwnerTurnsWaiting(turns);
}

bool SNetGetTurnsInTransit(uint32_t *turns)
{
	return dvlnet_inst->SNetGetTurnsInTransit(turns);
}

/**
 * @brief engine calls this only once with argument 1
 */
bool SNetSetBasePlayer(int /*unused*/)
{
	return true;
}

void DvlNet_SendInfoRequest()
{
	dvlnet_inst->send_info_request();
}

std::vector<std::string> DvlNet_GetGamelist()
{
	return dvlnet_inst->get_gamelist();
}

void DvlNet_SetPassword(std::string pw)
{
	dvlnet_inst->setup_password(std::move(pw));
}

} // namespace devilution
