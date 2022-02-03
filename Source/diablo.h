/**
 * @file diablo.h
 *
 * Interface of the main game initialization functions.
 */
#pragma once

#include <cstdint>

#include "utils/endian.hpp"

#include "controls/keymapper.hpp"
#ifdef _DEBUG
#include "monstdat.h"
#endif
#include "gendung.h"
#include "init.h"

namespace devilution {

#define NUMLEVELS 25

enum clicktype : int8_t {
	CLICK_NONE,
	CLICK_LEFT,
	CLICK_RIGHT,
};

/**
 * @brief Specifices what game logic step is currently executed
 */
enum class GameLogicStep {
	None,
	ProcessPlayers,
	ProcessMonsters,
	ProcessObjects,
	ProcessMissiles,
	ProcessItems,
	ProcessTowners,
	ProcessItemsTown,
	ProcessMissilesTown,
};

extern SDL_Window *ghMainWnd;
extern uint32_t glSeedTbl[NUMLEVELS];
extern dungeon_type gnLevelTypeTbl[NUMLEVELS];
extern Point MousePosition;
extern bool gbRunGame;
extern bool gbRunGameResult;
extern bool zoomflag;
extern bool gbProcessPlayers;
extern bool gbLoadGame;
extern bool cineflag;
/* These are defined in fonts.h */
extern bool was_fonts_init;
extern void FontsCleanup();
extern int PauseMode;
extern bool gbNestArt;
/**
 * @brief Don't show Messageboxes or other user-interaction. Needed for UnitTests.
 */
extern bool gbQuietMode;
extern clicktype sgbMouseDown;
extern uint16_t gnTickDelay;
extern char gszProductName[64];

void FreeGameMem();
bool StartGame(bool bNewGame);
[[noreturn]] void diablo_quit(int exitStatus);
int DiabloMain(int argc, char **argv);
bool TryIconCurs();
void diablo_pause_game();
void diablo_focus_pause();
void diablo_focus_unpause();
bool PressEscKey();
void DisableInputWndProc(uint32_t uMsg, int32_t wParam, int32_t lParam);
void LoadGameLevel(bool firstflag, lvl_entry lvldir);

/**
 * @param bStartup Process additional ticks before returning
 */
void game_loop(bool bStartup);
void diablo_color_cyc_logic();

/* rdata */

extern Keymapper keymapper;
extern bool gbForceWindowed;
#ifdef _DEBUG
extern bool DebugDisableNetworkTimeout;
#endif

/**
 * @brief Specifices what game logic step is currently executed
 */
extern GameLogicStep gGameLogicStep;

} // namespace devilution
