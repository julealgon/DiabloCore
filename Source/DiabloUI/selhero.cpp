#include "selhero.h"

#include <algorithm>
#include <chrono>
#include <random>

#include <fmt/format.h>

#include "DiabloUI/diabloui.h"
#include "DiabloUI/dialogs.h"
#include "DiabloUI/scrollbar.h"
#include "DiabloUI/selok.h"
#include "DiabloUI/selyesno.h"
#include "control.h"
#include "options.h"
#include "pfile.h"

namespace devilution {

bool selhero_endMenu;

bool (*gfnHeroInfo)(bool (*fninfofunc)(_uiheroinfo *));
bool (*gfnHeroCreate)(_uiheroinfo *);
void (*gfnHeroStats)(unsigned int, _uidefaultstats *);

namespace {

std::size_t selhero_SaveCount = 0;
_uiheroinfo selhero_heros[MAX_CHARACTERS];
_uiheroinfo selhero_heroInfo;
const size_t MaxViewportItems = 6;
char textStats[6][4];
const char *title = "";
_selhero_selections selhero_result;
bool selhero_navigateYesNo;
bool selhero_deleteEnabled;

std::vector<std::unique_ptr<UiItemBase>> vecSelHeroDialog;
std::vector<std::unique_ptr<UiListItem>> vecSelHeroDlgItems;
std::vector<std::unique_ptr<UiItemBase>> vecSelDlgItems;

UiImage *SELHERO_DIALOG_HERO_IMG;

void SelheroListFocus(int value);
void SelheroListSelect(int value);
void SelheroListEsc();
void SelheroLoadFocus(int value);
void SelheroLoadSelect(int value);
void SelheroNameSelect(int value);
void SelheroNameEsc();
void SelheroClassSelectorFocus(int value);
void SelheroClassSelectorSelect(int value);
void SelheroClassSelectorEsc();

void SelheroUiFocusNavigationYesNo()
{
	if (selhero_deleteEnabled)
		UiFocusNavigationYesNo();
}

void SelheroFree()
{
	ArtBackground.Unload();

	vecSelHeroDialog.clear();

	vecSelDlgItems.clear();
	vecSelHeroDlgItems.clear();
	UnloadScrollBar();
}

void SelheroSetStats()
{
	SELHERO_DIALOG_HERO_IMG->m_frame = static_cast<int>(selhero_heroInfo.heroclass);
	snprintf(textStats[0], sizeof(textStats[0]), "%i", selhero_heroInfo.level);
	snprintf(textStats[1], sizeof(textStats[1]), "%i", selhero_heroInfo.strength);
	snprintf(textStats[2], sizeof(textStats[2]), "%i", selhero_heroInfo.magic);
	snprintf(textStats[3], sizeof(textStats[3]), "%i", selhero_heroInfo.dexterity);
	snprintf(textStats[4], sizeof(textStats[4]), "%i", selhero_heroInfo.vitality);
	snprintf(textStats[5], sizeof(textStats[5]), "%i", selhero_heroInfo.saveNumber);
}

std::size_t listOffset = 0;
UiArtTextButton *SELLIST_DIALOG_DELETE_BUTTON;

void SelheroUpdateViewportItems()
{
	const std::size_t numViewportHeroes = std::min(selhero_SaveCount - listOffset, MaxViewportItems);
	for (std::size_t i = 0; i < numViewportHeroes; i++) {
		const std::size_t index = i + listOffset;
		vecSelHeroDlgItems[i]->m_text = selhero_heros[index].name;
		vecSelHeroDlgItems[i]->m_value = static_cast<int>(index);
	}
	if (numViewportHeroes < MaxViewportItems) {
		vecSelHeroDlgItems[numViewportHeroes]->m_text = "New Hero";
		vecSelHeroDlgItems[numViewportHeroes]->m_value = static_cast<int>(selhero_SaveCount);
	}
}

void SelheroScrollIntoView(std::size_t index)
{
	std::size_t newOffset = listOffset;
	if (index >= listOffset + MaxViewportItems)
		newOffset = index - (MaxViewportItems - 1);
	if (index < listOffset)
		newOffset = index;
	if (newOffset != listOffset) {
		listOffset = newOffset;
		SelheroUpdateViewportItems();
	}
}

bool SelHeroGetHeroInfo(_uiheroinfo *pInfo)
{
	selhero_heros[selhero_SaveCount] = *pInfo;

	selhero_SaveCount++;

	return true;
}

void SelheroListFocus(int value)
{
	const auto index = static_cast<std::size_t>(value);
	SelheroScrollIntoView(index);
	UiFlags baseFlags = UiFlags::AlignCenter | UiFlags::FontSize30;
	if (selhero_SaveCount != 0 && index < selhero_SaveCount) {
		memcpy(&selhero_heroInfo, &selhero_heros[index], sizeof(selhero_heroInfo));
		SelheroSetStats();
		SELLIST_DIALOG_DELETE_BUTTON->m_iFlags = baseFlags | UiFlags::ColorUiGold;
		selhero_deleteEnabled = true;
		return;
	}

	SELHERO_DIALOG_HERO_IMG->m_frame = static_cast<int>(enum_size<HeroClass>::value);
	strncpy(textStats[0], "--", sizeof(textStats[0]) - 1);
	strncpy(textStats[1], "--", sizeof(textStats[1]) - 1);
	strncpy(textStats[2], "--", sizeof(textStats[2]) - 1);
	strncpy(textStats[3], "--", sizeof(textStats[3]) - 1);
	strncpy(textStats[4], "--", sizeof(textStats[4]) - 1);
	strncpy(textStats[5], "--", sizeof(textStats[5]) - 1);
	SELLIST_DIALOG_DELETE_BUTTON->m_iFlags = baseFlags | UiFlags::ColorUiSilver | UiFlags::ElementDisabled;
	selhero_deleteEnabled = false;
}

bool SelheroListDeleteYesNo()
{
	selhero_navigateYesNo = selhero_deleteEnabled;

	return selhero_navigateYesNo;
}

void SelheroListSelect(int value)
{
	if (static_cast<std::size_t>(value) == selhero_SaveCount) {
		vecSelDlgItems.clear();

		SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 264), (Sint16)(UI_OFFSET_Y + 211), 320, 33 };
		vecSelDlgItems.push_back(std::make_unique<UiArtText>("Choose Class", rect1, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver, 3));

		vecSelHeroDlgItems.clear();
		vecSelHeroDlgItems.push_back(std::make_unique<UiListItem>("Warrior", static_cast<int>(HeroClass::Warrior)));
		vecSelHeroDlgItems.push_back(std::make_unique<UiListItem>("Rogue", static_cast<int>(HeroClass::Rogue)));
		vecSelHeroDlgItems.push_back(std::make_unique<UiListItem>("Sorcerer", static_cast<int>(HeroClass::Sorcerer)));
		vecSelHeroDlgItems.push_back(std::make_unique<UiListItem>("Monk", static_cast<int>(HeroClass::Monk)));
		vecSelHeroDlgItems.push_back(std::make_unique<UiListItem>("Bard", static_cast<int>(HeroClass::Bard)));
		vecSelHeroDlgItems.push_back(std::make_unique<UiListItem>("Barbarian", static_cast<int>(HeroClass::Barbarian)));
		int	itemH = 26;
		int itemY = 246 + (176 - vecSelHeroDlgItems.size() * itemH) / 2;
		vecSelDlgItems.push_back(std::make_unique<UiList>(vecSelHeroDlgItems, PANEL_LEFT + 264, (UI_OFFSET_Y + itemY), 320, itemH, UiFlags::AlignCenter | UiFlags::FontSize24 | UiFlags::ColorUiGold));

		SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 279), (Sint16)(UI_OFFSET_Y + 429), 140, 35 };
		vecSelDlgItems.push_back(std::make_unique<UiArtTextButton>("OK", &UiFocusNavigationSelect, rect2, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

		SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 429), (Sint16)(UI_OFFSET_Y + 429), 140, 35 };
		vecSelDlgItems.push_back(std::make_unique<UiArtTextButton>("Cancel", &UiFocusNavigationEsc, rect3, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

		UiInitList(vecSelHeroDlgItems.size(), SelheroClassSelectorFocus, SelheroClassSelectorSelect, SelheroClassSelectorEsc, vecSelDlgItems, true);
		memset(&selhero_heroInfo.name, 0, sizeof(selhero_heroInfo.name));
		selhero_heroInfo.saveNumber = pfile_ui_get_first_unused_save_num();
		SelheroSetStats();
		title = "New Hero";
		return;
	}

	if (selhero_heroInfo.hassaved) {
		vecSelDlgItems.clear();

		SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 264), (Sint16)(UI_OFFSET_Y + 211), 320, 33 };
		vecSelDlgItems.push_back(std::make_unique<UiArtText>("Save File Exists", rect1, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver, 3));

		vecSelHeroDlgItems.clear();
		vecSelHeroDlgItems.push_back(std::make_unique<UiListItem>("Load Game", 0));
		vecSelHeroDlgItems.push_back(std::make_unique<UiListItem>("New Game", 1));
		vecSelDlgItems.push_back(std::make_unique<UiList>(vecSelHeroDlgItems, PANEL_LEFT + 265, (UI_OFFSET_Y + 285), 320, 33, UiFlags::AlignCenter | UiFlags::FontSize24 | UiFlags::ColorUiGold));

		SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 279), (Sint16)(UI_OFFSET_Y + 427), 140, 35 };
		vecSelDlgItems.push_back(std::make_unique<UiArtTextButton>("OK", &UiFocusNavigationSelect, rect2, UiFlags::AlignCenter | UiFlags::VerticalCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

		SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 429), (Sint16)(UI_OFFSET_Y + 427), 140, 35 };
		vecSelDlgItems.push_back(std::make_unique<UiArtTextButton>("Cancel", &UiFocusNavigationEsc, rect3, UiFlags::AlignCenter | UiFlags::VerticalCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

		UiInitList(vecSelHeroDlgItems.size(), SelheroLoadFocus, SelheroLoadSelect, selhero_List_Init, vecSelDlgItems, true);
		title = "Characters";
		return;
	}

	SelheroLoadSelect(1);
}

void SelheroListEsc()
{
	UiInitList_clear();

	selhero_endMenu = true;
	selhero_result = SELHERO_PREVIOUS;
}

void SelheroClassSelectorFocus(int value)
{
	const auto heroClass = static_cast<HeroClass>(vecSelHeroDlgItems[value]->m_value);

	_uidefaultstats defaults;
	gfnHeroStats(static_cast<unsigned int>(heroClass), &defaults);

	selhero_heroInfo.level = 1;
	selhero_heroInfo.heroclass = heroClass;
	selhero_heroInfo.strength = defaults.strength;
	selhero_heroInfo.magic = defaults.magic;
	selhero_heroInfo.dexterity = defaults.dexterity;
	selhero_heroInfo.vitality = defaults.vitality;

	SelheroSetStats();
}

void SelheroClassSelectorSelect(int value)
{
	auto hClass = static_cast<HeroClass>(vecSelHeroDlgItems[value]->m_value);

	title = "New Hero";
	memset(selhero_heroInfo.name, '\0', sizeof(selhero_heroInfo.name));
	vecSelDlgItems.clear();
	SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 264), (Sint16)(UI_OFFSET_Y + 211), 320, 33 };
	vecSelDlgItems.push_back(std::make_unique<UiArtText>("Enter Name", rect1, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver, 3));

	SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 265), (Sint16)(UI_OFFSET_Y + 317), 320, 33 };
	vecSelDlgItems.push_back(std::make_unique<UiEdit>("Enter Name", selhero_heroInfo.name, 15, rect2, UiFlags::FontSize24 | UiFlags::ColorUiGold));

	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 279), (Sint16)(UI_OFFSET_Y + 429), 140, 35 };
	vecSelDlgItems.push_back(std::make_unique<UiArtTextButton>("OK", &UiFocusNavigationSelect, rect3, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

	SDL_Rect rect4 = { (Sint16)(PANEL_LEFT + 429), (Sint16)(UI_OFFSET_Y + 429), 140, 35 };
	vecSelDlgItems.push_back(std::make_unique<UiArtTextButton>("Cancel", &UiFocusNavigationEsc, rect4, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

	UiInitList(0, nullptr, SelheroNameSelect, SelheroNameEsc, vecSelDlgItems);
}

void SelheroClassSelectorEsc()
{
	vecSelDlgItems.clear();
	vecSelHeroDlgItems.clear();

	if (selhero_SaveCount != 0) {
		selhero_List_Init();
		return;
	}

	SelheroListEsc();
}

void SelheroNameSelect(int /*value*/)
{
	if (gfnHeroCreate(&selhero_heroInfo)) {
		SelheroLoadSelect(1);
		return;
	}
	UiErrorOkDialog("Unable to create character.", vecSelDlgItems);

	memset(selhero_heroInfo.name, '\0', sizeof(selhero_heroInfo.name));
	SelheroClassSelectorSelect(0);
}

void SelheroNameEsc()
{
	SelheroListSelect(selhero_SaveCount);
}

void SelheroLoadFocus(int value)
{
}

void SelheroLoadSelect(int value)
{
	UiInitList_clear();
	selhero_endMenu = true;
	if (vecSelHeroDlgItems[value]->m_value == 0) {
		selhero_result = SELHERO_CONTINUE;
		return;
	}

	SelheroFree();
	selhero_endMenu = true;
	selhero_result = SELHERO_NEW_DUNGEON;
}

} // namespace

void selhero_Init()
{
	LoadBackgroundArt("ui_art\\selhero.pcx");
	UiAddBackground(&vecSelHeroDialog);
	UiAddLogo(&vecSelHeroDialog);
	LoadScrollBar();

	selhero_SaveCount = 0;
	gfnHeroInfo(SelHeroGetHeroInfo);
	std::reverse(selhero_heros, selhero_heros + selhero_SaveCount);

	vecSelDlgItems.clear();
	SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 24), (Sint16)(UI_OFFSET_Y + 161), 590, 35 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>(&title, rect1, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver, 3));

	SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 30), (Sint16)(UI_OFFSET_Y + 211), 180, 76 };
	auto heroImg = std::make_unique<UiImage>(&ArtHero, rect2, UiFlags::None, /*bAnimated=*/false, static_cast<int>(enum_size<HeroClass>::value));
	SELHERO_DIALOG_HERO_IMG = heroImg.get();
	vecSelHeroDialog.push_back(std::move(heroImg));

	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 323), 110, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>("Level:", rect3, UiFlags::AlignRight | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));

	SDL_Rect rect4 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 323), 110, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>("Level:", rect4, UiFlags::AlignRight | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));
	SDL_Rect rect5 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 323), 40, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>(textStats[0], rect5, UiFlags::AlignCenter | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));

	SDL_Rect rect6 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 358), 110, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>("Strength:", rect6, UiFlags::AlignRight | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));
	SDL_Rect rect7 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 358), 40, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>(textStats[1], rect7, UiFlags::AlignCenter | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));

	SDL_Rect rect8 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 380), 110, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>("Magic:", rect8, UiFlags::AlignRight | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));
	SDL_Rect rect9 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 380), 40, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>(textStats[2], rect9, UiFlags::AlignCenter | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));

	SDL_Rect rect10 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 401), 110, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>("Dexterity:", rect10, UiFlags::AlignRight | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));
	SDL_Rect rect11 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 401), 40, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>(textStats[3], rect11, UiFlags::AlignCenter | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));

	SDL_Rect rect12 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 422), 110, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>("Vitality:", rect12, UiFlags::AlignRight | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));
	SDL_Rect rect13 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 422), 40, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>(textStats[4], rect13, UiFlags::AlignCenter | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));

#if _DEBUG
	SDL_Rect rect14 = { (Sint16)(PANEL_LEFT + 39), (Sint16)(UI_OFFSET_Y + 443), 110, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>("Savegame:", rect14, UiFlags::AlignRight | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));
	SDL_Rect rect15 = { (Sint16)(PANEL_LEFT + 159), (Sint16)(UI_OFFSET_Y + 443), 40, 21 };
	vecSelHeroDialog.push_back(std::make_unique<UiArtText>(textStats[5], rect15, UiFlags::AlignCenter | UiFlags::FontSize12 | UiFlags::ColorUiSilverDark));
#endif
}

void selhero_List_Init()
{
	listOffset = 0;
	vecSelDlgItems.clear();

	SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 264), (Sint16)(UI_OFFSET_Y + 211), 320, 33 };
	vecSelDlgItems.push_back(std::make_unique<UiArtText>("Select Hero", rect1, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver, 3));

	vecSelHeroDlgItems.clear();
	const size_t numViewportHeroes = std::min(selhero_SaveCount + 1, MaxViewportItems);
	for (std::size_t i = 0; i < numViewportHeroes; i++) {
		vecSelHeroDlgItems.push_back(std::make_unique<UiListItem>("", -1));
	}
	SelheroUpdateViewportItems();

	vecSelDlgItems.push_back(std::make_unique<UiList>(vecSelHeroDlgItems, PANEL_LEFT + 265, (UI_OFFSET_Y + 256), 320, 26, UiFlags::AlignCenter | UiFlags::FontSize24 | UiFlags::ColorUiGold));

	SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 585), (Sint16)(UI_OFFSET_Y + 244), 25, 178 };
	auto pinnedScrollBar = std::make_unique<UiScrollbar>(&ArtScrollBarBackground, &ArtScrollBarThumb, &ArtScrollBarArrow, rect2);
	auto *scrollBar = pinnedScrollBar.get();
	vecSelDlgItems.push_back(std::move(pinnedScrollBar));

	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 239), (Sint16)(UI_OFFSET_Y + 429), 120, 35 };
	vecSelDlgItems.push_back(std::make_unique<UiArtTextButton>("OK", &UiFocusNavigationSelect, rect3, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

	SDL_Rect rect4 = { (Sint16)(PANEL_LEFT + 364), (Sint16)(UI_OFFSET_Y + 429), 120, 35 };
	auto setlistDialogDeleteButton = std::make_unique<UiArtTextButton>("Delete", &SelheroUiFocusNavigationYesNo, rect4, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiSilver | UiFlags::ElementDisabled);
	SELLIST_DIALOG_DELETE_BUTTON = setlistDialogDeleteButton.get();
	vecSelDlgItems.push_back(std::move(setlistDialogDeleteButton));

	SDL_Rect rect5 = { (Sint16)(PANEL_LEFT + 489), (Sint16)(UI_OFFSET_Y + 429), 120, 35 };
	vecSelDlgItems.push_back(std::make_unique<UiArtTextButton>("Cancel", &UiFocusNavigationEsc, rect5, UiFlags::AlignCenter | UiFlags::FontSize30 | UiFlags::ColorUiGold));

	UiInitList(selhero_SaveCount + 1, SelheroListFocus, SelheroListSelect, SelheroListEsc, vecSelDlgItems, false, SelheroListDeleteYesNo);
	UiInitScrollBar(scrollBar, MaxViewportItems, &listOffset);
	title = "Characters";
}

static void UiSelHeroDialog(
    bool (*fninfo)(bool (*fninfofunc)(_uiheroinfo *)),
    bool (*fncreate)(_uiheroinfo *),
    void (*fnstats)(unsigned int, _uidefaultstats *),
    bool (*fnremove)(_uiheroinfo *),
    _selhero_selections *dlgresult,
    uint32_t *saveNumber)
{
	do {
		gfnHeroInfo = fninfo;
		gfnHeroCreate = fncreate;
		gfnHeroStats = fnstats;
		selhero_result = *dlgresult;

		selhero_navigateYesNo = false;

		selhero_Init();

		if (selhero_SaveCount != 0) {
			selhero_List_Init();
		} else {
			SelheroListSelect(selhero_SaveCount);
		}

		selhero_endMenu = false;
		while (!selhero_endMenu && !selhero_navigateYesNo) {
			UiClearScreen();
			UiRenderItems(vecSelHeroDialog);
			UiPollAndRender();
		}
		SelheroFree();

		if (selhero_navigateYesNo) {
			char dialogTitle[32];
			char dialogText[256];
			strncpy(dialogTitle, "Delete Hero", sizeof(dialogTitle) - 1);
			strncpy(dialogText, fmt::format("Are you sure you want to delete the character \"{:s}\"?", selhero_heroInfo.name).c_str(), sizeof(dialogText));

			if (UiSelHeroYesNoDialog(dialogTitle, dialogText))
				fnremove(&selhero_heroInfo);
		}
	} while (selhero_navigateYesNo);

	*dlgresult = selhero_result;
	*saveNumber = selhero_heroInfo.saveNumber;
}

void UiSelHeroSingDialog(
    bool (*fninfo)(bool (*fninfofunc)(_uiheroinfo *)),
    bool (*fncreate)(_uiheroinfo *),
    bool (*fnremove)(_uiheroinfo *),
    void (*fnstats)(unsigned int, _uidefaultstats *),
    _selhero_selections *dlgresult,
    uint32_t *saveNumber)
{
	UiSelHeroDialog(fninfo, fncreate, fnstats, fnremove, dlgresult, saveNumber);
}

} // namespace devilution
