
#include "control.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/selok.h"

namespace devilution {
namespace {
int mainmenu_attract_time_out; //seconds
uint32_t dwAttractTicks;

std::vector<std::unique_ptr<UiItemBase>> vecMainMenuDialog;
std::vector<std::unique_ptr<UiListItem>> vecMenuItems;

_mainmenu_selections MainMenuResult;

void UiMainMenuSelect(int value)
{
	MainMenuResult = (_mainmenu_selections)vecMenuItems[value]->m_value;
}

void MainmenuEsc()
{
	std::size_t last = vecMenuItems.size() - 1;
	if (SelectedItem == last) {
		UiMainMenuSelect(last);
	} else {
		SelectedItem = last;
	}
}

void MainmenuLoad(const char *name, void (*fnSound)(const char *file))
{
	gfnSoundFunction = fnSound;

	vecMenuItems.push_back(std::make_unique<UiListItem>("Single Player", MAINMENU_SINGLE_PLAYER));
	vecMenuItems.push_back(std::make_unique<UiListItem>("Replay Intro", MAINMENU_REPLAY_INTRO));
	vecMenuItems.push_back(std::make_unique<UiListItem>("Support", MAINMENU_SHOW_SUPPORT));
	vecMenuItems.push_back(std::make_unique<UiListItem>("Show Credits", MAINMENU_SHOW_CREDITS));
	vecMenuItems.push_back(std::make_unique<UiListItem>(gbIsHellfire ? "Exit Hellfire" : "Exit Diablo", MAINMENU_EXIT_DIABLO));

	if (gbIsHellfire)
		LoadArt("ui_art\\mainmenuw.pcx", &ArtBackgroundWidescreen);
	LoadBackgroundArt("ui_art\\mainmenu.pcx");

	UiAddBackground(&vecMainMenuDialog);
	UiAddLogo(&vecMainMenuDialog);

	vecMainMenuDialog.push_back(std::make_unique<UiList>(vecMenuItems, PANEL_LEFT + 64, (UI_OFFSET_Y + 192), 510, 43, UiFlags::FontSize42 | UiFlags::ColorUiGold | UiFlags::AlignCenter, 5));

	SDL_Rect rect = { 17, (Sint16)(gnScreenHeight - 36), 605, 21 };
	vecMainMenuDialog.push_back(std::make_unique<UiArtText>(name, rect, UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));

	UiInitList(vecMenuItems.size(), nullptr, UiMainMenuSelect, MainmenuEsc, vecMainMenuDialog, true);
}

void MainmenuFree()
{
	ArtBackgroundWidescreen.Unload();
	ArtBackground.Unload();

	vecMainMenuDialog.clear();

	vecMenuItems.clear();
}

} // namespace

void mainmenu_restart_repintro()
{
	dwAttractTicks = SDL_GetTicks() + mainmenu_attract_time_out * 1000;
}

bool UiMainMenuDialog(const char *name, _mainmenu_selections *pdwResult, void (*fnSound)(const char *file), int attractTimeOut)
{
	MainMenuResult = MAINMENU_NONE;
	while (MainMenuResult == MAINMENU_NONE) {
		mainmenu_attract_time_out = attractTimeOut;
		MainmenuLoad(name, fnSound);

		mainmenu_restart_repintro(); // for automatic starts

		while (MainMenuResult == MAINMENU_NONE) {
			UiClearScreen();
			UiPollAndRender();
			if (SDL_GetTicks() >= dwAttractTicks) {
				MainMenuResult = MAINMENU_ATTRACT_MODE;
			}
		}

		MainmenuFree();
	}

	*pdwResult = MainMenuResult;
	return true;
}

} // namespace devilution
