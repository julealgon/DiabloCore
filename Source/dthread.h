/**
 * @file dthread.h
 *
 * Interface of functions for updating game state from network commands.
 */
#pragma once

#include <memory>

namespace devilution {

void dthread_remove_player(uint8_t pnum);
void DThreadCleanup();

} // namespace devilution
