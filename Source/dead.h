/**
 * @file dead.h
 *
 * Interface of functions for placing dead monsters.
 */
#pragma once

#include <array>
#include <cstdint>

#include "engine/point.hpp"
#include "engine.h"

namespace devilution {

static constexpr unsigned MaxCorpses = 31;

struct Corpse {
	std::array<const std::byte *, 8> data;
	int frame;
	int width;
	uint8_t translationPaletteIndex;
};

extern Corpse Corpses[MaxCorpses];
extern int8_t stonendx;

void InitCorpses();
void AddCorpse(Point tilePosition, int8_t dv, Direction ddir);
void SyncUniqDead();

} // namespace devilution
