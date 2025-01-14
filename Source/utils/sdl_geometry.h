/**
 * @file sdl_geometry.h
 * Helpers for SDL geometry types
 */
#pragma once

#include <SDL_rect.h>

#include "engine/rectangle.hpp"

namespace devilution {

/**
 * @brief Same as constructing directly but avoids type conversion warnings.
 */
inline SDL_Rect MakeSdlRect(
    decltype(SDL_Rect {}.x) x, decltype(SDL_Rect {}.y) y,
    decltype(SDL_Rect {}.w) w, decltype(SDL_Rect {}.h) h)
{
	return SDL_Rect { x, y, w, h };
}

inline SDL_Rect MakeSdlRect(Rectangle rect)
{
	return MakeSdlRect(rect.position.x, rect.position.y, rect.size.width, rect.size.height);
}

} // namespace devilution
