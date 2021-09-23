#include "controls/menu_controls.h"

#include "DiabloUI/diabloui.h"
#include "controls/remap_keyboard.h"
#include "utils/sdl_compat.h"

namespace devilution {

MenuAction GetMenuAction(const SDL_Event &event)
{
	if (event.type == SDL_KEYDOWN) {
		auto sym = event.key.keysym.sym;
		remap_keyboard_key(&sym);
		switch (sym) {
		case SDLK_UP:
			return MenuAction_UP;
		case SDLK_DOWN:
			return MenuAction_DOWN;
		case SDLK_TAB:
			if ((SDL_GetModState() & KMOD_SHIFT) != 0)
				return MenuAction_UP;
			else
				return MenuAction_DOWN;
		case SDLK_PAGEUP:
			return MenuAction_PAGE_UP;
		case SDLK_PAGEDOWN:
			return MenuAction_PAGE_DOWN;
		case SDLK_RETURN: {
			const Uint8 *state = SDLC_GetKeyState();
			if (state[SDLC_KEYSTATE_LALT] == 0 && state[SDLC_KEYSTATE_RALT] == 0) {
				return MenuAction_SELECT;
			}
			break;
		}
		case SDLK_KP_ENTER:
			return MenuAction_SELECT;
		case SDLK_SPACE:
			if (!textInputActive) {
				return MenuAction_SELECT;
			}
			break;
		case SDLK_DELETE:
			return MenuAction_DELETE;
		case SDLK_LEFT:
			return MenuAction_LEFT;
		case SDLK_RIGHT:
			return MenuAction_RIGHT;
		case SDLK_ESCAPE:
			return MenuAction_BACK;
		default:
			break;
		}
	}

	return MenuAction_NONE;
} // namespace devilution

} // namespace devilution
