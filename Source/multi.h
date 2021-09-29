/**
 * @file multi.h
 *
 * Interface of functions for keeping multiplayer games in sync.
 */
#pragma once

#include <cstdint>

#include "msg.h"

namespace devilution {

// must be unsigned to generate unsigned comparisons with pnum
#define MAX_PLRS 4

struct GameData {
	int32_t size;
	/** Used to initialise the seed table for dungeon levels so players in multiplayer games generate the same layout */
	uint32_t dwSeed;
	uint32_t programid;
	uint8_t nTickRate;
	uint8_t bRunInTown;
	uint8_t bTheoQuest;
	uint8_t bCowQuest;
	uint8_t bFriendlyFire;
};

extern bool gbSomebodyWonGameKludge;
extern char szPlayerDescript[128];
extern uint16_t sgwPackPlrOffsetTbl[MAX_PLRS];
extern BYTE gbActivePlayers;
extern bool gbGameDestroyed;
extern GameData sgGameInitInfo;
extern bool gbSelectProvider;
extern char szPlayerName[128];
extern BYTE gbDeltaSender;
extern uint32_t player_state[MAX_PLRS];

void multi_msg_add(std::byte *pbMsg, BYTE bLen);
void NetSendLoPri(int playerId, std::byte *pbMsg, BYTE bLen);
void NetSendHiPri(int playerId, std::byte *pbMsg, BYTE bLen);
void multi_send_msg_packet(uint32_t pmask, std::byte *src, BYTE len);
void multi_msg_countdown();
void multi_net_ping();

/**
 * @return Always true for singleplayer
 */
bool multi_handle_delta();
void multi_process_network_packets();
void NetClose();
bool NetInit();
void recv_plrinfo(int pnum, TCmdPlrInfoHdr *p, bool recv);

} // namespace devilution
