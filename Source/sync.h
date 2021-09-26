/**
 * @file sync.h
 *
 * Interface of functionality for syncing game state with other players.
 */
#pragma once

#include <cstdint>

namespace devilution {

uint32_t sync_all_monsters(const std::byte *pbBuf, uint32_t dwMaxLen);
uint32_t sync_update(int pnum, const std::byte *pbBuf);
void sync_init();

} // namespace devilution
