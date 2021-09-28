#pragma once

#include "gendung.h"

namespace devilution {

extern _difficulty nDifficulty;

void selgame_GameSelection_Select();
void selgame_GameSelection_Esc();
void selgame_Diff_Focus(int value);
void selgame_Diff_Select(int value);
void selgame_Diff_Esc();

} // namespace devilution
