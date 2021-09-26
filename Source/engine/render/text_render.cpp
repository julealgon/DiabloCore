/**
 * @file text_render.cpp
 *
 * Text rendering.
 */
#include "text_render.hpp"

#include <array>
#include <unordered_map>
#include <utility>

#include "DiabloUI/art_draw.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/ui_item.h"
#include "cel_render.hpp"
#include "engine.h"
#include "engine/load_cel.hpp"
#include "engine/load_file.hpp"
#include "engine/point.hpp"
#include "palette.h"
#include "utils/display.h"
#include "utils/sdl_compat.h"

namespace devilution {

namespace {

std::unordered_map<uint32_t, std::unique_ptr<Art>> Fonts;

std::array<std::array<uint8_t, 256>, 5> FontKerns;
std::array<int, 5> FontSizes = { 12, 24, 30, 42, 46 };
std::array<int, 5> LineHeights = { 12, 26, 38, 42, 50 };
std::array<int, 5> BaseLineOffset = { -3, -2, -3, -6, -7 };

std::array<const char *, 12> ColorTranlations = {
	"fonts\\goldui.trn",
	"fonts\\grayui.trn",
	"fonts\\golduis.trn",
	"fonts\\grayuis.trn",

	nullptr,
	"fonts\\black.trn",

	"fonts\\white.trn",
	"fonts\\whitegold.trn",
	"fonts\\red.trn",
	"fonts\\blue.trn",

	"fonts\\buttonface.trn",
	"fonts\\buttonpushed.trn",
};

GameFontTables GetSizeFromFlags(UiFlags flags)
{
	if (HasAnyOf(flags, UiFlags::FontSize24))
		return GameFont24;
	else if (HasAnyOf(flags, UiFlags::FontSize30))
		return GameFont30;
	else if (HasAnyOf(flags, UiFlags::FontSize42))
		return GameFont42;
	else if (HasAnyOf(flags, UiFlags::FontSize46))
		return GameFont46;

	return GameFont12;
}

text_color GetColorFromFlags(UiFlags flags)
{
	if (HasAnyOf(flags, UiFlags::ColorWhite))
		return ColorWhite;
	else if (HasAnyOf(flags, UiFlags::ColorBlue))
		return ColorBlue;
	else if (HasAnyOf(flags, UiFlags::ColorRed))
		return ColorRed;
	else if (HasAnyOf(flags, UiFlags::ColorBlack))
		return ColorBlack;
	else if (HasAnyOf(flags, UiFlags::ColorGold))
		return ColorGold;
	else if (HasAnyOf(flags, UiFlags::ColorUiGold))
		return ColorUiGold;
	else if (HasAnyOf(flags, UiFlags::ColorUiSilver))
		return ColorUiSilver;
	else if (HasAnyOf(flags, UiFlags::ColorUiGoldDark))
		return ColorUiGoldDark;
	else if (HasAnyOf(flags, UiFlags::ColorUiSilverDark))
		return ColorUiSilverDark;
	else if (HasAnyOf(flags, UiFlags::ColorButtonface))
		return ColorButtonface;
	else if (HasAnyOf(flags, UiFlags::ColorButtonpushed))
		return ColorButtonpushed;

	return ColorWhitegold;
}

} // namespace

void LoadFont(GameFontTables size, text_color color)
{
	auto font = std::make_unique<Art>();

	char path[32];
	sprintf(path, "fonts\\%i-00.pcx", FontSizes[size]);

	if (ColorTranlations[color] != nullptr) {
		std::array<uint8_t, 256> colorMapping;
		LoadFileInMem(ColorTranlations[color], colorMapping);
		LoadMaskedArt(path, font.get(), 256, 1, &colorMapping);
	} else {
		LoadMaskedArt(path, font.get(), 256, 1);
	}

	uint32_t fontId = (color << 24) | (size << 16);
	Fonts.insert(make_pair(fontId, move(font)));

	sprintf(path, "fonts\\%i-00.bin", FontSizes[size]);
	LoadFileInMem(path, FontKerns[size]);
}

void UnloadFont(GameFontTables size, text_color color)
{
	uint32_t fontId = (color << 24) | (size << 16);

	for (auto font = Fonts.begin(); font != Fonts.end();) {
		if ((font->first & 0xFFFF0000) == fontId) {
			Fonts.erase(font++);
		} else {
			font++;
		}
	}
}

void UnloadFonts()
{
	Fonts.clear();
}

int GetLineWidth(std::string_view text, GameFontTables size, int spacing, int *charactersInLine)
{
	int lineWidth = 0;

	size_t i = 0;
	for (; i < text.length(); i++) {
		if (text[i] == '\n')
			break;

		uint8_t frame = text[i] & 0xFF;
		lineWidth += FontKerns[size][frame] + spacing;
	}

	if (charactersInLine != nullptr)
		*charactersInLine = i;

	return lineWidth != 0 ? (lineWidth - spacing) : 0;
}

int AdjustSpacingToFitHorizontally(int &lineWidth, int maxSpacing, int charactersInLine, int availableWidth)
{
	if (lineWidth <= availableWidth || charactersInLine < 2)
		return maxSpacing;

	const int overhang = lineWidth - availableWidth;
	const int spacingRedux = (overhang + charactersInLine - 2) / (charactersInLine - 1);
	lineWidth -= spacingRedux * (charactersInLine - 1);
	return maxSpacing - spacingRedux;
}

void WordWrapString(char *text, size_t width, GameFontTables size, int spacing)
{
	const size_t textLength = strlen(text);
	size_t lineStart = 0;
	size_t lineWidth = 0;
	for (unsigned i = 0; i < textLength; i++) {
		if (text[i] == '\n') { // Existing line break, scan next line
			lineStart = i + 1;
			lineWidth = 0;
			continue;
		}

		uint8_t frame = text[i] & 0xFF;
		lineWidth += FontKerns[size][frame] + spacing;

		if (lineWidth - spacing <= width) {
			continue; // String is still within the limit, continue to the next line
		}

		size_t j; // Backtrack to the previous space
		for (j = i; j > lineStart; j--) {
			if (text[j] == ' ') {
				break;
			}
		}

		if (j == lineStart) { // Single word longer than width
			if (i == textLength)
				break;
			j = i;
		}

		// Break line and continue to next line
		i = j;
		text[i] = '\n';
		lineStart = i + 1;
		lineWidth = 0;
	}
}

/**
 * @todo replace Rectangle with cropped Surface
 */
uint32_t DrawString(const Surface &out, std::string_view text, const Rectangle &rect, UiFlags flags, int spacing, int lineHeight)
{
	GameFontTables size = GetSizeFromFlags(flags);
	text_color color = GetColorFromFlags(flags);

	int charactersInLine = 0;
	int lineWidth = 0;
	if (HasAnyOf(flags, (UiFlags::AlignCenter | UiFlags::AlignRight | UiFlags::KerningFitSpacing)))
		lineWidth = GetLineWidth(text, size, spacing, &charactersInLine);

	int maxSpacing = spacing;
	if (HasAnyOf(flags, UiFlags::KerningFitSpacing))
		spacing = AdjustSpacingToFitHorizontally(lineWidth, maxSpacing, charactersInLine, rect.size.width);

	Point characterPosition = rect.position;
	if (HasAnyOf(flags, UiFlags::AlignCenter))
		characterPosition.x += (rect.size.width - lineWidth) / 2;
	else if (HasAnyOf(flags, UiFlags::AlignRight))
		characterPosition.x += rect.size.width - lineWidth;

	int rightMargin = rect.position.x + rect.size.width;
	int bottomMargin = rect.size.height != 0 ? rect.position.y + rect.size.height : out.h();

	if (lineHeight == -1)
		lineHeight = LineHeights[size];

	if (HasAnyOf(flags, UiFlags::VerticalCenter)) {
		int textHeight = (std::count(text.cbegin(), text.cend(), '\n') + 1) * lineHeight;
		characterPosition.y += (rect.size.height - textHeight) / 2;
	}

	characterPosition.y += BaseLineOffset[size];

	uint32_t fontId = (color << 24) | (size << 16);
	auto font = Fonts.find(fontId);
	if (font == Fonts.end()) {
		Log("Font: size {} and color {} not loaded ", size, color);
		return 0;
	}

	const auto &activeFont = font->second;

	uint32_t i = 0;
	for (; i < text.length(); i++) {
		uint8_t frame = text[i] & 0xFF;
		if (text[i] == '\n' || characterPosition.x > rightMargin) {
			if (characterPosition.y + lineHeight >= bottomMargin)
				break;
			characterPosition.y += lineHeight;

			if (HasAnyOf(flags, (UiFlags::AlignCenter | UiFlags::AlignRight | UiFlags::KerningFitSpacing))) {
				int nextLineIndex = text[i] == '\n' ? i + 1 : i;
				if (nextLineIndex < text.length())
					lineWidth = GetLineWidth(&text[nextLineIndex], size, spacing, &charactersInLine);
				else
					lineWidth = 0;
			}

			if (HasAnyOf(flags, UiFlags::KerningFitSpacing))
				spacing = AdjustSpacingToFitHorizontally(lineWidth, maxSpacing, charactersInLine, rect.size.width);

			characterPosition.x = rect.position.x;
			if (HasAnyOf(flags, UiFlags::AlignCenter))
				characterPosition.x += (rect.size.width - lineWidth) / 2;
			else if (HasAnyOf(flags, UiFlags::AlignRight))
				characterPosition.x += rect.size.width - lineWidth;
		}
		DrawArt(out, characterPosition, activeFont.get(), frame);
		if (text[i] != '\n')
			characterPosition.x += FontKerns[size][frame] + spacing;
	}

	if (HasAnyOf(flags, UiFlags::PentaCursor)) {
		CelDrawTo(out, characterPosition + Displacement { 0, lineHeight - BaseLineOffset[size] }, *pSPentSpn2Cels, PentSpn2Spin());
	} else if (HasAnyOf(flags, UiFlags::TextCursor) && GetAnimationFrame(2, 500) != 0) {
		DrawArt(out, characterPosition, activeFont.get(), '|');
	}

	return i;
}

uint8_t PentSpn2Spin()
{
	return (SDL_GetTicks() / 50) % 8 + 1;
}

} // namespace devilution
