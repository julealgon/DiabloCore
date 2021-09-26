/**
 * @file debug.h
 *
 * Interface of debug functions.
 */
#pragma once

#include <optional>
#include <unordered_map>

#include "engine.h"
#include "engine/cel_sprite.hpp"
#include "miniwin/miniwin.h"

namespace devilution {

extern std::optional<CelSprite> pSquareCel;
extern bool DebugToggle;
extern bool DebugGodMode;
extern bool DebugVision;
extern bool DebugGrid;
extern std::unordered_map<int, Point> DebugCoordsMap;
extern bool DebugScrollViewEnabled;

void FreeDebugGFX();
void LoadDebugGFX();
void GetDebugMonster();
void NextDebugMonster();
void SetDebugLevelSeedInfos(uint32_t mid1Seed, uint32_t mid2Seed, uint32_t mid3Seed, uint32_t endSeed);
bool CheckDebugTextCommand(const std::string_view text);
bool IsDebugGridTextNeeded();
bool GetDebugGridText(Point dungeonCoords, char *debugGridTextBuffer);

} // namespace devilution
