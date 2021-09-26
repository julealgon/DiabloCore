/**
 * @file help.cpp
 *
 * Implementation of the in-game help text.
 */
#include <string>
#include <vector>

#include "control.h"
#include "engine/render/text_render.hpp"
#include "init.h"
#include "minitext.h"
#include "stores.h"

namespace devilution {

bool HelpFlag;

namespace {

unsigned int SkipLines;

const char *const HelpText[] = {
	"$Keyboard Shortcuts:",
	"F1:    Open Help Screen",
	"Esc:   Display Main Menu",
	"Tab:   Display Auto-map",
	"Space: Hide all info screens",
	"S: Open Speedbook",
	"B: Open Spellbook",
	"I: Open Inventory screen",
	"C: Open Character screen",
	"Q: Open Quest log",
	"F: Reduce screen brightness",
	"G: Increase screen brightness",
	"Z: Zoom Game Screen",
	"+ / -: Zoom Automap",
	"1 - 8: Use Belt item",
	"F5, F6, F7, F8:     Set hotkey for skill or spell",
	"Shift + Left Mouse Button: Attack without moving",
	"Shift + Left Mouse Button (on character screen): Assign all stat points",
	"Shift + Left Mouse Button (on inventory): Move item to belt or equip/unequip item",
	"Shift + Left Mouse Button (on belt): Move item to inventory",
	"",
	"$Movement:",
	"If you hold the mouse button down while moving, the character "
	   "will continue to move in that direction.",
	"",
	"$Combat:",
	"Holding down the shift key and then left-clicking allows the "
	   "character to attack without moving.",
	"",
	"$Auto-map:",
	"To access the auto-map, click the 'MAP' button on the "
	   "Information Bar or press 'TAB' on the keyboard. Zooming in and "
	   "out of the map is done with the + and - keys. Scrolling the map "
	   "uses the arrow keys.",
	"",
	"$Picking up Objects:",
	"Useable items that are small in size, such as potions or scrolls, "
	   "are automatically placed in your 'belt' located at the top of "
	   "the Interface bar . When an item is placed in the belt, a small "
	   "number appears in that box. Items may be used by either pressing "
	   "the corresponding number or right-clicking on the item.",
	"",
	"$Gold",
	"You can select a specific amount of gold to drop by "
	   "right-clicking on a pile of gold in your inventory.",
	"",
	"$Skills & Spells:",
	"You can access your list of skills and spells by left-clicking on "
	   "the 'SPELLS' button in the interface bar. Memorized spells and "
	   "those available through staffs are listed here. Left-clicking on "
	   "the spell you wish to cast will ready the spell. A readied spell "
	   "may be cast by simply right-clicking in the play area.",
	"",
	"$Using the Speedbook for Spells",
	"Left-clicking on the 'readied spell' button will open the 'Speedbook' "
	   "which allows you to select a skill or spell for immediate use. "
	   "To use a readied skill or spell, simply right-click in the main play "
	   "area.",
	"Shift + Left-clicking on the 'select current spell' button will clear the readied spell",
	"",
	"$Setting Spell Hotkeys",
	"You can assign up to four Hotkeys for skills, spells or scrolls. "
	   "Start by opening the 'speedbook' as described in the section above. "
	   "Press the F5, F6, F7 or F8 keys after highlighting the spell you "
	   "wish to assign.",
	"",
	"$Spell Books",
	"Reading more than one book increases your knowledge of that "
	   "spell, allowing you to cast the spell more effectively.",
};

std::vector<std::string> HelpTextLines;

} // namespace

void InitHelp()
{
	HelpFlag = false;
	char tempString[512];

	for (const auto *text : HelpText) {
		strcpy(tempString, text);

		WordWrapString(tempString, 577);
		const std::string_view paragraph = tempString;

		size_t previous = 0;
		while (true) {
			size_t next = paragraph.find('\n', previous);
			HelpTextLines.emplace_back(paragraph.substr(previous, next));
			if (next == std::string::npos)
				break;
			previous = next + 1;
		}
	}
}

void DrawHelp(const Surface &out)
{
	DrawSTextHelp();
	DrawQTextBack(out);

	const char *title;
	if (gbIsHellfire)
		title = gbIsSpawn ? "Shareware Hellfire Help" : "Hellfire Help";
	else
		title = gbIsSpawn ? "Shareware Diablo Help" : "Diablo Help";
	PrintSString(out, 0, 2, title, UiFlags::ColorWhitegold | UiFlags::AlignCenter);

	DrawSLine(out, 5);

	const int sx = PANEL_X + 32;
	const int sy = UI_OFFSET_Y + 51;

	for (int i = 6; i < 21; i++) {
		const char *line = HelpTextLines[i - 6 + SkipLines].c_str();
		if (line[0] == '\0') {
			continue;
		}

		int offset = 0;
		UiFlags style = UiFlags::ColorWhite;
		if (line[0] == '$') {
			offset = 1;
			style = UiFlags::ColorRed;
		}

		DrawString(out, &line[offset], { { sx, sy + i * 12 }, { 577, 12 } }, style);
	}

	PrintSString(out, 0, 23, "Press ESC to end or the arrow keys to scroll.", UiFlags::ColorWhitegold | UiFlags::AlignCenter);
}

void DisplayHelp()
{
	SkipLines = 0;
	HelpFlag = true;
}

void HelpScrollUp()
{
	if (SkipLines > 0)
		SkipLines--;
}

void HelpScrollDown()
{
	if (SkipLines < HelpTextLines.size() - 15)
		SkipLines++;
}

} // namespace devilution
