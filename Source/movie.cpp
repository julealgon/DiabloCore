/**
 * @file movie.cpp
 *
 * Implementation of video playback.
 */

#include "diablo.h"
#include "effects.h"
#include "hwcursor.hpp"
#include "sound.h"
#include "storm/storm_svid.h"
#include "utils/display.h"

namespace devilution {

/** Should the movie continue playing. */
bool movie_playing;
/** Should the movie play in a loop. */
bool loop_movie;

void play_movie(const char *pszMovie, bool userCanClose)
{
	movie_playing = true;

	sound_disable_music(true);
	stream_stop();
	effects_play_sound("Sfx\\Misc\\blank.wav");

	if (IsHardwareCursorEnabled()) {
		SetHardwareCursorVisible(false);
	}

	if (SVidPlayBegin(pszMovie, loop_movie ? 0x100C0808 : 0x10280808)) {
		tagMSG msg;
		while (movie_playing) {
			while (movie_playing && FetchMessage(&msg)) {
				switch (msg.message) {
				case DVL_WM_KEYDOWN:
				case DVL_WM_LBUTTONDOWN:
				case DVL_WM_RBUTTONDOWN:
					if (userCanClose || (msg.message == DVL_WM_KEYDOWN && msg.wParam == DVL_VK_ESCAPE))
						movie_playing = false;
					break;
				case DVL_WM_QUIT:
					SVidPlayEnd();
					diablo_quit(0);
				}
			}
			if (!SVidPlayContinue())
				break;
		}
		SVidPlayEnd();
	}

	sound_disable_music(false);

	movie_playing = false;

	SDL_GetMouseState(&MousePosition.x, &MousePosition.y);
	OutputToLogical(&MousePosition.x, &MousePosition.y);
}

void PlayInGameMovie(const char *pszMovie)
{
	PaletteFadeOut(8);
	play_movie(pszMovie, false);
	ClearScreenBuffer();
	force_redraw = 255;
	scrollrt_draw_game_screen();
	PaletteFadeIn(8);
	force_redraw = 255;
}

} // namespace devilution
