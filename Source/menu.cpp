/**
 * @file mainmenu.cpp
 *
 * Implementation of functions for interacting with the main menu.
 */

#include "DiabloUI/diabloui.h"
#include "init.h"
#include "movie.h"
#include "options.h"
#include "pfile.h"
#include "storm/storm.h"

namespace devilution {

uint32_t gSaveNumber;

namespace {

/** The active music track id for the main menu. */
uint8_t menu_music_track_id = TMUSIC_INTRO;

void RefreshMusic()
{
	music_start(menu_music_track_id);

	do {
		menu_music_track_id++;
		if (menu_music_track_id == NUM_MUSIC || (!gbIsHellfire && menu_music_track_id > TMUSIC_L4))
			menu_music_track_id = TMUSIC_L2;
	} while (menu_music_track_id == TMUSIC_TOWN || menu_music_track_id == TMUSIC_L1);
}

bool InitMenu(_selhero_selections type)
{
	bool success;

	if (type == SELHERO_PREVIOUS)
		return true;

	music_stop();

	success = StartGame(type != SELHERO_CONTINUE);
	if (success)
		RefreshMusic();

	return success;
}

bool InitSinglePlayerMenu()
{
	return InitMenu(SELHERO_NEW_DUNGEON);
}

void PlayIntro()
{
	music_stop();
	if (gbIsHellfire)
		play_movie("gendata\\Hellfire.smk", true);
	else
		play_movie("gendata\\diablo1.smk", true);
	RefreshMusic();
}

} // namespace

bool mainmenu_select_hero_dialog(GameData *gameData)
{
	_selhero_selections dlgresult = SELHERO_NEW_DUNGEON;
	UiSelHeroSingDialog(
		pfile_ui_set_hero_infos,
		pfile_ui_save_create,
		pfile_delete_save,
		pfile_ui_set_class_stats,
		&dlgresult,
		&gSaveNumber);

	gbLoadGame = (dlgresult == SELHERO_CONTINUE);

	if (dlgresult == SELHERO_PREVIOUS) {
		SErrSetLastError(1223);
		return false;
	}

	pfile_read_player_from_save(gSaveNumber, Players[MyPlayerId]);

	return true;
}

void mainmenu_loop()
{
	bool done;
	_mainmenu_selections menu;

	RefreshMusic();
	done = false;

	do {
		menu = MAINMENU_NONE;
		if (!UiMainMenuDialog(gszProductName, &menu, effects_play_sound, 30))
			app_fatal("%s", "Unable to display mainmenu");

		switch (menu) {
		case MAINMENU_NONE:
			break;
		case MAINMENU_SINGLE_PLAYER:
			if (!InitSinglePlayerMenu())
				done = true;
			break;
		case MAINMENU_ATTRACT_MODE:
		case MAINMENU_REPLAY_INTRO:
			if (gbActive)
				PlayIntro();
			break;
		case MAINMENU_SHOW_CREDITS:
			UiCreditsDialog();
			break;
		case MAINMENU_SHOW_SUPPORT:
			UiSupportDialog();
			break;
		case MAINMENU_EXIT_DIABLO:
			done = true;
			break;
		}
	} while (!done);

	music_stop();
}

} // namespace devilution
