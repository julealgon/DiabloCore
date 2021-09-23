/**
 * @file error.cpp
 *
 * Implementation of in-game message functions.
 */

#include <deque>

#include "error.h"

#include "control.h"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "stores.h"

namespace devilution {

namespace {

std::deque<std::string> DiabloMessages;
std::vector<std::string> TextLines;
uint32_t msgdelay;
int ErrorWindowHeight = 54;
const int LineHeight = 12;
const int LineWidth = 418;

void InitNextLines()
{
	msgdelay = SDL_GetTicks();
	auto message = DiabloMessages.front();

	TextLines.clear();

	char tempstr[1536]; // Longest test is about 768 chars * 2 for unicode
	strcpy(tempstr, message.data());

	WordWrapString(tempstr, LineWidth, GameFont12, 1);
	const string_view paragraphs = tempstr;

	size_t previous = 0;
	while (true) {
		size_t next = paragraphs.find('\n', previous);
		TextLines.emplace_back(paragraphs.substr(previous, next));
		if (next == std::string::npos)
			break;
		previous = next + 1;
	}

	ErrorWindowHeight = std::max(54, static_cast<int>((TextLines.size() * LineHeight) + 42));
}

} // namespace

/** Maps from error_id to error message. */
const char *const MsgStrings[] = {
	"",
	"No automap available in town",
	"No multiplayer functions in demo",
	"Direct Sound Creation Failed",
	"Not available in shareware version",
	"Not enough space to save",
	"No Pause in town",
	"Copying to a hard disk is recommended",
	"Multiplayer sync problem",
	"No pause in multiplayer",
	"Loading...",
	"Saving...",
	"Some are weakened as one grows strong",
	"New strength is forged through destruction",
	"Those who defend seldom attack",
	"The sword of justice is swift and sharp",
	"While the spirit is vigilant the body thrives",
	"The powers of mana refocused renews",
	"Time cannot diminish the power of steel",
	"Magic is not always what it seems to be",
	"What once was opened now is closed",
	"Intensity comes at the cost of wisdom",
	"Arcane power brings destruction",
	"That which cannot be held cannot be harmed",
	"Crimson and Azure become as the sun",
	"Knowledge and wisdom at the cost of self",
	"Drink and be refreshed",
	"Wherever you go, there you are",
	"Energy comes at the cost of wisdom",
	"Riches abound when least expected",
	"Where avarice fails, patience gains reward",
	"Blessed by a benevolent companion!",
	"The hands of men may be guided by fate",
	"Strength is bolstered by heavenly faith",
	"The essence of life flows from within",
	"The way is made clear when viewed from above",
	"Salvation comes at the cost of wisdom",
	"Mysteries are revealed in the light of reason",
	"Those who are last may yet be first",
	"Generosity brings its own rewards",
	"You must be at least level 8 to use this.",
	"You must be at least level 13 to use this.",
	"You must be at least level 17 to use this.",
	"Arcane knowledge gained!",
	"That which does not kill you...",
	"Knowledge is power.",
	"Give and you shall receive.",
	"Some experience is gained by touch.",
	"There's no place like home.",
	"Spiritual energy is restored.",
	"You feel more agile.",
	"You feel stronger.",
	"You feel wiser.",
	"You feel refreshed.",
	"That which can break will.",
};

void InitDiabloMsg(diablo_message e)
{
	std::string msg = MsgStrings[e];
	InitDiabloMsg(msg);
}

void InitDiabloMsg(std::string msg)
{
	if (DiabloMessages.size() >= MAX_SEND_STR_LEN)
		return;

	if (std::find(DiabloMessages.begin(), DiabloMessages.end(), msg) != DiabloMessages.end())
		return;

	DiabloMessages.push_back(msg);
	if (DiabloMessages.size() == 1)
		InitNextLines();
}

bool IsDiabloMsgAvailable()
{
	return !DiabloMessages.empty();
}

void CancelCurrentDiabloMsg()
{
	msgdelay = 0;
}

void ClrDiabloMsg()
{
	DiabloMessages.clear();
}

void DrawDiabloMsg(const Surface &out)
{
	int dialogStartY = ((gnScreenHeight - PANEL_HEIGHT) / 2) - (ErrorWindowHeight / 2) + 9;

	CelDrawTo(out, { PANEL_X + 101, dialogStartY }, *pSTextSlidCels, 1);
	CelDrawTo(out, { PANEL_X + 527, dialogStartY }, *pSTextSlidCels, 4);
	CelDrawTo(out, { PANEL_X + 101, dialogStartY + ErrorWindowHeight - 6 }, *pSTextSlidCels, 2);
	CelDrawTo(out, { PANEL_X + 527, dialogStartY + ErrorWindowHeight - 6 }, *pSTextSlidCels, 3);

	int sx = PANEL_X + 109;
	for (int i = 0; i < 35; i++) {
		CelDrawTo(out, { sx, dialogStartY }, *pSTextSlidCels, 5);
		CelDrawTo(out, { sx, dialogStartY + ErrorWindowHeight - 6 }, *pSTextSlidCels, 7);
		sx += 12;
	}
	int drawnYborder = 12;
	while ((drawnYborder + 12) < ErrorWindowHeight) {
		CelDrawTo(out, { PANEL_X + 101, dialogStartY + drawnYborder }, *pSTextSlidCels, 6);
		CelDrawTo(out, { PANEL_X + 527, dialogStartY + drawnYborder }, *pSTextSlidCels, 8);
		drawnYborder += 12;
	}

	DrawHalfTransparentRectTo(out, PANEL_X + 104, dialogStartY - 8, 432, ErrorWindowHeight);

	auto message = DiabloMessages.front();
	int lineNumber = 0;
	for (auto &line : TextLines) {
		DrawString(out, line.c_str(), { { PANEL_X + 109, dialogStartY + 12 + lineNumber * LineHeight }, { LineWidth, LineHeight } }, UiFlags::AlignCenter, 1, LineHeight);
		lineNumber += 1;
	}

	if (msgdelay > 0 && msgdelay <= SDL_GetTicks() - 3500) {
		msgdelay = 0;
	}
	if (msgdelay == 0) {
		DiabloMessages.pop_front();
		if (!DiabloMessages.empty())
			InitNextLines();
	}
}

} // namespace devilution
