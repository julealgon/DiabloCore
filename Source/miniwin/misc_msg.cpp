#include <SDL.h>
#include <cstdint>
#include <deque>
#include <string>

#include "control.h"
#include "controls/remap_keyboard.h"
#include "cursor.h"
#include "engine/rectangle.hpp"
#include "hwcursor.hpp"
#include "inv.h"
#include "menu.h"
#include "miniwin/miniwin.h"
#include "movie.h"
#include "storm/storm.h"
#include "utils/display.h"
#include "utils/log.hpp"
#include "utils/sdl_compat.h"
#include "utils/stubs.h"

/** @file
 * *
 * Windows message handling and keyboard event conversion for SDL.
 */

namespace devilution {

static std::deque<tagMSG> message_queue;

bool mouseWarping = false;
Point mousePositionWarping;

void SetCursorPos(Point position)
{
	mousePositionWarping = position;
	mouseWarping = true;
	LogicalToOutput(&position.x, &position.y);
	SDL_WarpMouseInWindow(ghMainWnd, position.x, position.y);
}

// Moves the mouse to the first attribute "+" button.
void FocusOnCharInfo()
{
	auto &myPlayer = Players[MyPlayerId];

	if (invflag || myPlayer._pStatPts <= 0)
		return;

	// Find the first incrementable stat.
	int stat = -1;
	for (auto attribute : enum_values<CharacterAttribute>()) {
		if (myPlayer.GetBaseAttributeValue(attribute) >= myPlayer.GetMaximumAttributeValue(attribute))
			continue;
		stat = static_cast<int>(attribute);
	}
	if (stat == -1)
		return;

	SetCursorPos(ChrBtnsRect[stat].Center());
}

static int TranslateSdlKey(SDL_Keysym key)
{
	// ref: https://wiki.libsdl.org/SDL_Keycode
	// ref: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	SDL_Keycode sym = key.sym;
	remap_keyboard_key(&sym);
	switch (sym) {
	case SDLK_BACKSPACE:
		return DVL_VK_BACK;
	case SDLK_TAB:
		return DVL_VK_TAB;
	case SDLK_RETURN:
		return DVL_VK_RETURN;
	case SDLK_ESCAPE:
		return DVL_VK_ESCAPE;
	case SDLK_SPACE:
		return DVL_VK_SPACE;
	case SDLK_QUOTE:
		return DVL_VK_OEM_7;
	case SDLK_COMMA:
		return DVL_VK_OEM_COMMA;
	case SDLK_MINUS:
		return DVL_VK_OEM_MINUS;
	case SDLK_PERIOD:
		return DVL_VK_OEM_PERIOD;
	case SDLK_SLASH:
		return DVL_VK_OEM_2;
	case SDLK_SEMICOLON:
		return DVL_VK_OEM_1;
	case SDLK_EQUALS:
		return DVL_VK_OEM_PLUS;
	case SDLK_LEFTBRACKET:
		return DVL_VK_OEM_4;
	case SDLK_BACKSLASH:
		return DVL_VK_OEM_5;
	case SDLK_RIGHTBRACKET:
		return DVL_VK_OEM_6;
	case SDLK_BACKQUOTE:
		return DVL_VK_OEM_3;
	case SDLK_DELETE:
		return DVL_VK_DELETE;
	case SDLK_CAPSLOCK:
		return DVL_VK_CAPITAL;
	case SDLK_F1:
		return DVL_VK_F1;
	case SDLK_F2:
		return DVL_VK_F2;
	case SDLK_F3:
		return DVL_VK_F3;
	case SDLK_F4:
		return DVL_VK_F4;
	case SDLK_F5:
		return DVL_VK_F5;
	case SDLK_F6:
		return DVL_VK_F6;
	case SDLK_F7:
		return DVL_VK_F7;
	case SDLK_F8:
		return DVL_VK_F8;
	case SDLK_F9:
		return DVL_VK_F9;
	case SDLK_F10:
		return DVL_VK_F10;
	case SDLK_F11:
		return DVL_VK_F11;
	case SDLK_F12:
		return DVL_VK_F12;
	case SDLK_PRINTSCREEN:
		return DVL_VK_SNAPSHOT;
	case SDLK_SCROLLLOCK:
		return DVL_VK_SCROLL;
	case SDLK_PAUSE:
		return DVL_VK_PAUSE;
	case SDLK_INSERT:
		return DVL_VK_INSERT;
	case SDLK_HOME:
		return DVL_VK_HOME;
	case SDLK_PAGEUP:
		return DVL_VK_PRIOR;
	case SDLK_END:
		return DVL_VK_END;
	case SDLK_PAGEDOWN:
		return DVL_VK_NEXT;
	case SDLK_RIGHT:
		return DVL_VK_RIGHT;
	case SDLK_LEFT:
		return DVL_VK_LEFT;
	case SDLK_DOWN:
		return DVL_VK_DOWN;
	case SDLK_UP:
		return DVL_VK_UP;
	case SDLK_NUMLOCKCLEAR:
		return DVL_VK_NUMLOCK;
	case SDLK_KP_DIVIDE:
		return DVL_VK_DIVIDE;
	case SDLK_KP_MULTIPLY:
		return DVL_VK_MULTIPLY;
	case SDLK_KP_MINUS:
		// Returning DVL_VK_OEM_MINUS to play nice with Devilution automap zoom.
		//
		// For a distinct keypad key-code, DVL_VK_SUBTRACT should be returned.
		return DVL_VK_OEM_MINUS;
	case SDLK_KP_PLUS:
		// Returning DVL_VK_OEM_PLUS to play nice with Devilution automap zoom.
		//
		// For a distinct keypad key-code, DVL_VK_ADD should be returned.
		return DVL_VK_OEM_PLUS;
	case SDLK_KP_ENTER:
		return DVL_VK_RETURN;
	case SDLK_KP_1:
		return DVL_VK_NUMPAD1;
	case SDLK_KP_2:
		return DVL_VK_NUMPAD2;
	case SDLK_KP_3:
		return DVL_VK_NUMPAD3;
	case SDLK_KP_4:
		return DVL_VK_NUMPAD4;
	case SDLK_KP_5:
		return DVL_VK_NUMPAD5;
	case SDLK_KP_6:
		return DVL_VK_NUMPAD6;
	case SDLK_KP_7:
		return DVL_VK_NUMPAD7;
	case SDLK_KP_8:
		return DVL_VK_NUMPAD8;
	case SDLK_KP_9:
		return DVL_VK_NUMPAD9;
	case SDLK_KP_000:
	case SDLK_KP_00:
	case SDLK_KP_0:
		return DVL_VK_NUMPAD0;
	case SDLK_KP_PERIOD:
		return DVL_VK_DECIMAL;
	case SDLK_MENU:
		return DVL_VK_MENU;
	case SDLK_KP_COMMA:
		return DVL_VK_OEM_COMMA;
	case SDLK_LCTRL:
		return DVL_VK_LCONTROL;
	case SDLK_LSHIFT:
		return DVL_VK_LSHIFT;
	case SDLK_LALT:
		return DVL_VK_LMENU;
	case SDLK_LGUI:
		return DVL_VK_LWIN;
	case SDLK_RCTRL:
		return DVL_VK_RCONTROL;
	case SDLK_RSHIFT:
		return DVL_VK_RSHIFT;
	case SDLK_RALT:
		return DVL_VK_RMENU;
	case SDLK_RGUI:
		return DVL_VK_RWIN;
	default:
		if (sym >= SDLK_a && sym <= SDLK_z) {
			return 'A' + (sym - SDLK_a);
		} else if (sym >= SDLK_0 && sym <= SDLK_9) {
			return '0' + (sym - SDLK_0);
		} else if (sym >= SDLK_F1 && sym <= SDLK_F12) {
			return DVL_VK_F1 + (sym - SDLK_F1);
		}
		Log("unknown key: name={} sym=0x{:X} scan={} mod=0x{:X}", SDL_GetKeyName(sym), sym, key.scancode, key.mod);
		return -1;
	}
}

namespace {

int32_t PositionForMouse(int16_t x, int16_t y)
{
	return (((uint16_t)(y & 0xFFFF)) << 16) | (uint16_t)(x & 0xFFFF);
}

int32_t KeystateForMouse(int32_t ret)
{
	ret |= (SDL_GetModState() & KMOD_SHIFT) != 0 ? DVL_MK_SHIFT : 0;
	// XXX: other DVL_MK_* codes not implemented
	return ret;
}

bool FalseAvail(const char *name, int value)
{
	LogDebug("Unhandled SDL event: {} {}", name, value);
	return true;
}

} // namespace

bool FetchMessage(tagMSG *lpMsg)
{
	if (!message_queue.empty()) {
		*lpMsg = message_queue.front();
		message_queue.pop_front();
		return true;
	}

	SDL_Event e;
	if (SDL_PollEvent(&e) == 0) {
		return false;
	}

	lpMsg->message = 0;
	lpMsg->lParam = 0;
	lpMsg->wParam = 0;

	if (e.type == SDL_QUIT) {
		lpMsg->message = DVL_WM_QUIT;
		return true;
	}

	if (!movie_playing) {
		// SDL generates mouse events from touch-based inputs to provide basic
		// touchscreeen support for apps that don't explicitly handle touch events
		if (e.type == AnyOf(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP) && e.button.which == SDL_TOUCH_MOUSEID)
			return true;
		if (e.type == SDL_MOUSEMOTION && e.motion.which == SDL_TOUCH_MOUSEID)
			return true;
		if (e.type == SDL_MOUSEWHEEL && e.wheel.which == SDL_TOUCH_MOUSEID)
			return true;
	}

	if ((e.type == SDL_KEYUP || e.type == SDL_KEYDOWN) && e.key.keysym.sym == SDLK_UNKNOWN) {
		// Erroneous events generated by RG350 kernel.
		return true;
	}

	switch (e.type) {
	case SDL_QUIT:
		lpMsg->message = DVL_WM_QUIT;
		break;
	case SDL_KEYDOWN:
	case SDL_KEYUP: {
		int key = TranslateSdlKey(e.key.keysym);
		if (key == -1)
			return FalseAvail(e.type == SDL_KEYDOWN ? "SDL_KEYDOWN" : "SDL_KEYUP", e.key.keysym.sym);
		lpMsg->message = e.type == SDL_KEYDOWN ? DVL_WM_KEYDOWN : DVL_WM_KEYUP;
		lpMsg->wParam = (uint32_t)key;
		// HACK: Encode modifier in lParam for TranslateMessage later
		lpMsg->lParam = e.key.keysym.mod << 16;
	} break;
	case SDL_MOUSEMOTION:
		lpMsg->message = DVL_WM_MOUSEMOVE;
		lpMsg->lParam = PositionForMouse(e.motion.x, e.motion.y);
		lpMsg->wParam = KeystateForMouse(0);
		break;
	case SDL_MOUSEBUTTONDOWN: {
		int button = e.button.button;
		if (button == SDL_BUTTON_LEFT) {
			lpMsg->message = DVL_WM_LBUTTONDOWN;
			lpMsg->lParam = PositionForMouse(e.button.x, e.button.y);
			lpMsg->wParam = KeystateForMouse(DVL_MK_LBUTTON);
		} else if (button == SDL_BUTTON_RIGHT) {
			lpMsg->message = DVL_WM_RBUTTONDOWN;
			lpMsg->lParam = PositionForMouse(e.button.x, e.button.y);
			lpMsg->wParam = KeystateForMouse(DVL_MK_RBUTTON);
		}
	} break;
	case SDL_MOUSEBUTTONUP: {
		int button = e.button.button;
		if (button == SDL_BUTTON_LEFT) {
			lpMsg->message = DVL_WM_LBUTTONUP;
			lpMsg->lParam = PositionForMouse(e.button.x, e.button.y);
			lpMsg->wParam = KeystateForMouse(0);
		} else if (button == SDL_BUTTON_RIGHT) {
			lpMsg->message = DVL_WM_RBUTTONUP;
			lpMsg->lParam = PositionForMouse(e.button.x, e.button.y);
			lpMsg->wParam = KeystateForMouse(0);
		}
	} break;
	case SDL_MOUSEWHEEL:
		lpMsg->message = DVL_WM_KEYDOWN;
		if (e.wheel.y > 0) {
			lpMsg->wParam = GetAsyncKeyState(DVL_VK_CONTROL) ? DVL_VK_OEM_PLUS : DVL_VK_UP;
		} else if (e.wheel.y < 0) {
			lpMsg->wParam = GetAsyncKeyState(DVL_VK_CONTROL) ? DVL_VK_OEM_MINUS : DVL_VK_DOWN;
		} else if (e.wheel.x > 0) {
			lpMsg->wParam = DVL_VK_LEFT;
		} else if (e.wheel.x < 0) {
			lpMsg->wParam = DVL_VK_RIGHT;
		}
		break;
	case SDL_AUDIODEVICEADDED:
		return FalseAvail("SDL_AUDIODEVICEADDED", e.adevice.which);
	case SDL_AUDIODEVICEREMOVED:
		return FalseAvail("SDL_AUDIODEVICEREMOVED", e.adevice.which);
	case SDL_KEYMAPCHANGED:
		return FalseAvail("SDL_KEYMAPCHANGED", 0);
	case SDL_TEXTEDITING:
		return FalseAvail("SDL_TEXTEDITING", e.edit.length);
	case SDL_TEXTINPUT:
		return FalseAvail("SDL_TEXTINPUT", e.text.windowID);
	case SDL_WINDOWEVENT:
		switch (e.window.event) {
		case SDL_WINDOWEVENT_SHOWN:
			gbActive = true;
			lpMsg->message = DVL_WM_PAINT;
			break;
		case SDL_WINDOWEVENT_HIDDEN:
			gbActive = false;
			break;
		case SDL_WINDOWEVENT_EXPOSED:
			lpMsg->message = DVL_WM_PAINT;
			break;
		case SDL_WINDOWEVENT_LEAVE:
			lpMsg->message = DVL_WM_CAPTURECHANGED;
			break;
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			ReinitializeHardwareCursor();
			break;
		case SDL_WINDOWEVENT_MOVED:
		case SDL_WINDOWEVENT_RESIZED:
		case SDL_WINDOWEVENT_MINIMIZED:
		case SDL_WINDOWEVENT_MAXIMIZED:
		case SDL_WINDOWEVENT_RESTORED:
		case SDL_WINDOWEVENT_TAKE_FOCUS:
			break;
		case SDL_WINDOWEVENT_ENTER:
			// Bug in SDL, SDL_WarpMouseInWindow doesn't emit SDL_MOUSEMOTION
			// and SDL_GetMouseState gives previous location if mouse was
			// outside window (observed on Ubuntu 19.04)
			if (mouseWarping) {
				MousePosition = mousePositionWarping;
				mouseWarping = false;
			}
			break;
		case SDL_WINDOWEVENT_CLOSE:
			lpMsg->message = DVL_WM_QUERYENDSESSION;
			break;

		case SDL_WINDOWEVENT_FOCUS_LOST:
			diablo_focus_pause();
			break;
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			diablo_focus_unpause();
			break;

		default:
			return FalseAvail("SDL_WINDOWEVENT", e.window.event);
		}

		break;
	default:
		return FalseAvail("unknown", e.type);
	}
	return true;
}

bool TranslateMessage(const tagMSG *lpMsg)
{
	if (lpMsg->message == DVL_WM_KEYDOWN) {
		int key = lpMsg->wParam;
		unsigned mod = (uint32_t)lpMsg->lParam >> 16;

		bool shift = (mod & KMOD_SHIFT) != 0;
		bool caps = (mod & KMOD_CAPS) != 0;
		bool upper = shift != caps;

		bool isAlpha = (key >= 'A' && key <= 'Z');
		bool isNumeric = (key >= '0' && key <= '9');
		bool isControl = key == DVL_VK_SPACE || key == DVL_VK_BACK || key == DVL_VK_ESCAPE || key == DVL_VK_TAB || key == DVL_VK_RETURN;
		bool isOem = (key >= DVL_VK_OEM_1 && key <= DVL_VK_OEM_7);

		if (isControl || isAlpha || isNumeric || isOem) {
			if (!upper && isAlpha) {
				key = tolower(key);
			} else if (shift && isNumeric) {
				switch (key) {
				case '1':
					key = '!';
					break;
				case '2':
					key = '@';
					break;
				case '3':
					key = '#';
					break;
				case '4':
					key = '$';
					break;
				case '5':
					key = '%';
					break;
				case '6':
					key = '^';
					break;
				case '7':
					key = '&';
					break;
				case '8':
					key = '*';
					break;
				case '9':
					key = '(';
					break;
				case '0':
					key = ')';
					break;
				}
			} else if (isOem) {
				// XXX: This probably only supports US keyboard layout
				switch (key) {
				case DVL_VK_OEM_1:
					key = shift ? ':' : ';';
					break;
				case DVL_VK_OEM_2:
					key = shift ? '?' : '/';
					break;
				case DVL_VK_OEM_3:
					key = shift ? '~' : '`';
					break;
				case DVL_VK_OEM_4:
					key = shift ? '{' : '[';
					break;
				case DVL_VK_OEM_5:
					key = shift ? '|' : '\\';
					break;
				case DVL_VK_OEM_6:
					key = shift ? '}' : ']';
					break;
				case DVL_VK_OEM_7:
					key = shift ? '"' : '\'';
					break;
				case DVL_VK_OEM_MINUS:
					key = shift ? '_' : '-';
					break;
				case DVL_VK_OEM_PLUS:
					key = shift ? '+' : '=';
					break;
				case DVL_VK_OEM_PERIOD:
					key = shift ? '>' : '.';
					break;
				case DVL_VK_OEM_COMMA:
					key = shift ? '<' : ',';
					break;

				default:
					UNIMPLEMENTED();
				}
			}

#ifdef _DEBUG
			if (key >= 32) {
				Log("char: {:c}", key);
			}
#endif

			// XXX: This does not add extended info to lParam
			PostMessage(DVL_WM_CHAR, key, 0);
		}
	}

	return true;
}

bool GetAsyncKeyState(int vKey)
{
	if (vKey == DVL_MK_LBUTTON)
		return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
	if (vKey == DVL_MK_RBUTTON)
		return (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;

	const Uint8 *state = SDLC_GetKeyState();
	switch (vKey) {
	case DVL_VK_CONTROL:
		return state[SDLC_KEYSTATE_LEFTCTRL] != 0 || state[SDLC_KEYSTATE_RIGHTCTRL] != 0;
	case DVL_VK_SHIFT:
		return state[SDLC_KEYSTATE_LEFTSHIFT] != 0 || state[SDLC_KEYSTATE_RIGHTSHIFT] != 0;
	case DVL_VK_MENU:
		return state[SDLC_KEYSTATE_LALT] != 0 || state[SDLC_KEYSTATE_RALT] != 0;
	case DVL_VK_LEFT:
		return state[SDLC_KEYSTATE_LEFT] != 0;
	case DVL_VK_UP:
		return state[SDLC_KEYSTATE_UP] != 0;
	case DVL_VK_RIGHT:
		return state[SDLC_KEYSTATE_RIGHT] != 0;
	case DVL_VK_DOWN:
		return state[SDLC_KEYSTATE_DOWN] != 0;
	default:
		return false;
	}
}

void PushMessage(const tagMSG *lpMsg)
{
	assert(CurrentProc);

	CurrentProc(lpMsg->message, lpMsg->wParam, lpMsg->lParam);
}

bool PostMessage(uint32_t type, int32_t wParam, int32_t lParam)
{
	message_queue.push_back({ type, wParam, lParam });

	return true;
}

void ClearMessageQueue()
{
	message_queue.clear();
}

} // namespace devilution
