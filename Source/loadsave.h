/**
 * @file loadsave.h
 *
 * Interface of save game functionality.
 */
#pragma once

#include "player.h"

namespace devilution {

extern uint8_t giNumberOfLevels;

bool IsHeaderValid(uint32_t magicNumber);
void LoadHotkeys();
void LoadHeroItems(Player &player);
/**
 * @brief Remove invalid inventory items from the inventory grid
 * @param pnum The id of the player
 */
void RemoveEmptyInventory(Player &player);

/**
 * @brief Load game state
 * @param firstflag Can be set to false if we are simply reloading the current game
 */
void LoadGame(bool firstflag);
void SaveHotkeys();
void SaveHeroItems(Player &player);
void SaveGameData();
void SaveGame();
void SaveLevel();
void LoadLevel();

} // namespace devilution
