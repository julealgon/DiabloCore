#pragma once

#include <cstdint>

#include "pack.h"

namespace devilution {

struct DiabloOptions {
	/** @brief Play game intro video on startup. */
	bool bIntro;
	/** @brief Cornerstone of the world item. */
	char szItem[sizeof(ItemPack) * 2 + 1];
};

struct AudioOptions {
	/** @brief Movie and SFX volume. */
	int nSoundVolume;
	/** @brief Music volume. */
	int nMusicVolume;
};

struct GraphicsOptions {
	/** @brief Render width. */
	int nWidth;
	/** @brief Render height. */
	int nHeight;
	/** @brief Run in fullscreen or windowed mode. */
	bool bFullscreen;
	/** @brief Scale the image after rendering. */
	bool bUpscale;
	/** @brief Expand the aspect ratio to match the screen. */
	bool bFitToScreen;
	/** @brief See SDL_HINT_RENDER_SCALE_QUALITY. */
	char szScaleQuality[2];
	/** @brief Only scale by values divisible by the width and height. */
	bool bIntegerScaling;
	/** @brief Enable vsync on the output. */
	bool bVSync;
	/** @brief Gamma correction level. */
	int nGammaCorrection;
	/** @brief Enable FPS Limit. */
	bool bFPSLimit;
	/** @brief Show FPS, even without the -f command line flag. */
	bool bShowFPS;
};

struct GameplayOptions {
	/** @brief Gameplay ticks per second. */
	int nTickRate;
	/** @brief Enable double walk speed when in town. */
	bool bRunInTown;
	/** @brief Do not let the mouse leave the application window. */
	bool bGrabInput;
	/** @brief Show enemy health at the top of the screen. */
	bool bEnemyHealthBar;
	/** @brief Automatically attempt to equip weapon-type items when picking them up. */
	bool bAutoEquipWeapons;
	/** @brief Automatically attempt to equip armor-type items when picking them up. */
	bool bAutoEquipArmor;
	/** @brief Automatically attempt to equip helm-type items when picking them up. */
	bool bAutoEquipHelms;
	/** @brief Automatically attempt to equip shield-type items when picking them up. */
	bool bAutoEquipShields;
	/** @brief Automatically attempt to equip jewelry-type items when picking them up. */
	bool bAutoEquipJewelry;
	/** @brief Indicates whether or not monster type (Animal, Demon, Undead) is shown along with other monster information. */
	bool bShowMonsterType;
};

struct Options {
	DiabloOptions Diablo;
	AudioOptions Audio;
	GameplayOptions Gameplay;
	GraphicsOptions Graphics;
};

bool GetIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, const char *defaultString = "");
void SetIniValue(const char *sectionName, const char *keyName, const char *value, int len = 0);

extern Options sgOptions;
extern bool sbWasOptionsLoaded;

/**
 * @brief Save game configurations to ini file
 */
void SaveOptions();

/**
 * @brief Load game configurations from ini file
 */
void LoadOptions();

} // namespace devilution
