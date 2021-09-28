#include "selgame.h"

#include <fmt/format.h>

#include "DiabloUI/diabloui.h"
#include "DiabloUI/dialogs.h"
#include "DiabloUI/selhero.h"
#include "DiabloUI/selok.h"
#include "config.h"
#include "control.h"
#include "menu.h"
#include "options.h"
#include "storm/storm.h"

namespace devilution {

char selgame_Label[32];
char selgame_Ip[129] = "";
char selgame_Password[16] = "";
char selgame_Description[256];
bool selgame_enteringGame;
bool selgame_endMenu;
int *gdwPlayerId;
_difficulty nDifficulty;
int nTickRate;
int heroLevel;

static GameData *m_game_data;
extern int provider;

#define DESCRIPTION_WIDTH 205

namespace {

const char *title = "";

std::vector<std::unique_ptr<UiListItem>> vecSelGameDlgItems;
std::vector<std::unique_ptr<UiItemBase>> vecSelGameDialog;

} // namespace

void selgame_FreeVectors()
{
	vecSelGameDlgItems.clear();

	vecSelGameDialog.clear();
}

void selgame_Free()
{
	ArtBackground.Unload();

	selgame_FreeVectors();
}

/**
 * @brief Load the current hero level from save file
 * @param pInfo Hero info
 * @return always true
 */
bool UpdateHeroLevel(_uiheroinfo *pInfo)
{
	if (pInfo->saveNumber == gSaveNumber)
		heroLevel = pInfo->level;

	return true;
}

void selgame_GameSelection_Select()
{
	selgame_enteringGame = true;

	gfnHeroInfo(UpdateHeroLevel);

	selgame_FreeVectors();

	UiAddBackground(&vecSelGameDialog);
	UiAddLogo(&vecSelGameDialog);

	SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 24), (Sint16)(UI_OFFSET_Y + 161), 590, 35 };
	vecSelGameDialog.push_back(std::make_unique<UiArtText>(&title, rect1, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver, 3));

	SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 34), (Sint16)(UI_OFFSET_Y + 211), 205, 33 };
	vecSelGameDialog.push_back(std::make_unique<UiArtText>(selgame_Label, rect2, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver, 3));

	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 35), (Sint16)(UI_OFFSET_Y + 256), DESCRIPTION_WIDTH, 192 };
	vecSelGameDialog.push_back(std::make_unique<UiArtText>(selgame_Description, rect3, UiFlags::FontSize12 | UiFlags::ColorUiSilverDark, 1, 16));

	title = "Create Game";

	SDL_Rect rect4 = { (Sint16)(PANEL_LEFT + 299), (Sint16)(UI_OFFSET_Y + 211), 295, 35 };
	vecSelGameDialog.push_back(std::make_unique<UiArtText>("Select Difficulty", rect4, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver, 3));

	vecSelGameDlgItems.push_back(std::make_unique<UiListItem>("Normal", DIFF_NORMAL));
	vecSelGameDlgItems.push_back(std::make_unique<UiListItem>("Nightmare", DIFF_NIGHTMARE));
	vecSelGameDlgItems.push_back(std::make_unique<UiListItem>("Hell", DIFF_HELL));

	vecSelGameDialog.push_back(std::make_unique<UiList>(vecSelGameDlgItems, PANEL_LEFT + 300, (UI_OFFSET_Y + 282), 295, 26, UiFlags::AlignCenter | UiFlags::FontSize24 | UiFlags::ColorUiGold));

	SDL_Rect rect5 = { (Sint16)(PANEL_LEFT + 299), (Sint16)(UI_OFFSET_Y + 427), 140, 35 };
	vecSelGameDialog.push_back(std::make_unique<UiArtTextButton>("OK", &UiFocusNavigationSelect, rect5, UiFlags::AlignCenter | UiFlags::VerticalCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

	SDL_Rect rect6 = { (Sint16)(PANEL_LEFT + 449), (Sint16)(UI_OFFSET_Y + 427), 140, 35 };
	vecSelGameDialog.push_back(std::make_unique<UiArtTextButton>("CANCEL", &UiFocusNavigationEsc, rect6, UiFlags::AlignCenter | UiFlags::VerticalCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

	UiInitList(vecSelGameDlgItems.size(), selgame_Diff_Focus, selgame_Diff_Select, selgame_Diff_Esc, vecSelGameDialog, true);
}

void selgame_GameSelection_Esc()
{
	UiInitList_clear();
	selgame_enteringGame = false;
	selgame_endMenu = true;
}

void selgame_Diff_Focus(int value)
{
	switch (vecSelGameDlgItems[value]->m_value) {
	case DIFF_NORMAL:
		strncpy(selgame_Label, "Normal", sizeof(selgame_Label) - 1);
		strncpy(selgame_Description, "Normal Difficulty\nThis is where a starting character should begin the quest to defeat Diablo.", sizeof(selgame_Description) - 1);
		break;
	case DIFF_NIGHTMARE:
		strncpy(selgame_Label, "Nightmare", sizeof(selgame_Label) - 1);
		strncpy(selgame_Description, "Nightmare Difficulty\nThe denizens of the Labyrinth have been bolstered and will prove to be a greater challenge. This is recommended for experienced characters only.", sizeof(selgame_Description) - 1);
		break;
	case DIFF_HELL:
		strncpy(selgame_Label, "Hell", sizeof(selgame_Label) - 1);
		strncpy(selgame_Description, "Hell Difficulty\nThe most powerful of the underworld's creatures lurk at the gateway into Hell. Only the most experienced characters should venture in this realm.", sizeof(selgame_Description) - 1);
		break;
	}
	WordWrapString(selgame_Description, DESCRIPTION_WIDTH);
}

void selgame_Diff_Select(int value)
{
	nDifficulty = (_difficulty)vecSelGameDlgItems[value]->m_value;

	// This is part of a dangerous hack to enable difficulty selection in single-player.
	// FIXME: Dialogs should not refer to each other's variables.

	// We're in the selhero loop instead of the selgame one.
	// Free the selgame data and flag the end of the selhero loop.
	selhero_endMenu = true;

	// We only call FreeVectors because ArtBackground.Unload()
	// will be called by selheroFree().
	selgame_FreeVectors();

	// We must clear the InitList because selhero's loop will perform
	// one more iteration after this function exits.
	UiInitList_clear();
}

void selgame_Diff_Esc()
{
	selgame_Free();

	selhero_Init();
	selhero_List_Init();
}
} // namespace devilution
