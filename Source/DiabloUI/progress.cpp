#include "DiabloUI/art_draw.h"
#include "DiabloUI/button.h"
#include "DiabloUI/diabloui.h"
#include "DiabloUI/fonts.h"
#include "control.h"
#include "controls/menu_controls.h"
#include "dx.h"
#include "hwcursor.hpp"
#include "palette.h"
#include "utils/display.h"
#include "utils/ttf_wrap.h"

namespace devilution {
namespace {
Art dialogArt;
Art progressArt;
Art ArtPopupSm;
Art ArtProgBG;
Art ProgFil;
SDLSurfaceUniquePtr msgSurface;
SDLSurfaceUniquePtr msgShadow;
std::vector<std::unique_ptr<UiItemBase>> vecProgress;
bool endMenu;

void DialogActionCancel()
{
	endMenu = true;
}

void ProgressLoad(const char *msg)
{
	LoadBackgroundArt("ui_art\\black.pcx");
	LoadArt("ui_art\\spopup.pcx", &ArtPopupSm);
	LoadArt("ui_art\\prog_bg.pcx", &ArtProgBG);
	LoadArt("ui_art\\prog_fil.pcx", &ProgFil);
	LoadSmlButtonArt();
	LoadTtfFont();

	if (font != nullptr) {
		SDL_Color color = { 243, 243, 243, 0 };
		SDL_Color black = { 0, 0, 0, 0 };

		msgSurface = TTFWrap::RenderText_Solid(font, msg, color);
		msgShadow = TTFWrap::RenderText_Solid(font, msg, black);
	}
	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 265), (Sint16)(UI_OFFSET_Y + 267), SML_BUTTON_WIDTH, SML_BUTTON_HEIGHT };
	vecProgress.push_back(std::make_unique<UiButton>(&SmlButton, "Cancel", &DialogActionCancel, rect3));
}

void ProgressFree()
{
	ArtBackground.Unload();
	ArtPopupSm.Unload();
	ArtProgBG.Unload();
	ProgFil.Unload();
	UnloadSmlButtonArt();
	msgSurface = nullptr;
	msgShadow = nullptr;
	UnloadTtfFont();
}

void ProgressRender(BYTE progress)
{
	SDL_FillRect(DiabloUiSurface(), nullptr, 0x000000);
	DrawArt({ 0, 0 }, &ArtBackground);

	Point position = { GetCenterOffset(280), GetCenterOffset(144, gnScreenHeight) };

	DrawArt(position, &ArtPopupSm, 0, 280, 140);
	DrawArt({ GetCenterOffset(227), position.y + 52 }, &ArtProgBG, 0, 227);
	if (progress != 0) {
		DrawArt({ GetCenterOffset(227), position.y + 52 }, &ProgFil, 0, 227 * progress / 100);
	}
	DrawArt({ GetCenterOffset(110), position.y + 99 }, &SmlButton, 2, 110);

	if (msgSurface != nullptr) {
		SDL_Rect dscRect = {
			static_cast<Sint16>(position.x + 50 + 1),
			static_cast<Sint16>(position.y + 8 + 1),
			static_cast<Uint16>(msgSurface->w),
			static_cast<Uint16>(msgSurface->h)
		};
		Blit(msgShadow.get(), nullptr, &dscRect);
		dscRect.x -= 1;
		dscRect.y -= 1;
		Blit(msgSurface.get(), nullptr, &dscRect);
	}
}

} // namespace

bool UiProgressDialog(const char *msg, int (*fnfunc)())
{
	ProgressLoad(msg);
	SetFadeLevel(256);

	endMenu = false;
	int progress = 0;

	SDL_Event event;
	while (!endMenu && progress < 100) {
		progress = fnfunc();
		ProgressRender(progress);
		UiRenderItems(vecProgress);
		DrawMouse();
		RenderPresent();

		while (SDL_PollEvent(&event) != 0) {
			switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
			case SDL_MOUSEBUTTONUP:
				UiItemMouseEvents(&event, vecProgress);
				break;
			case SDLK_KP_ENTER:
			case SDLK_ESCAPE:
			case SDLK_RETURN:
			case SDLK_SPACE:
				endMenu = true;
				break;
			default:
				switch (GetMenuAction(event)) {
				case MenuAction_BACK:
				case MenuAction_SELECT:
					endMenu = true;
					break;
				default:
					break;
				}
				break;
			}
			UiHandleEvents(&event);
		}
	}
	ProgressFree();

	return progress == 100;
}

} // namespace devilution
