/**
 * @file options.cpp
 *
 * Load and save options from the diablo.ini file.
 */

#include <cstdint>
#include <fstream>
#include <locale>

#define SI_SUPPORT_IOSTREAMS
#include <SimpleIni.h>

#include "diablo.h"
#include "options.h"
#include "utils/file_util.h"
#include "utils/paths.h"

namespace devilution {

#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 640
#endif
#ifndef DEFAULT_HEIGHT
#define DEFAULT_HEIGHT 480
#endif

namespace {

std::string GetIniPath()
{
	auto path = paths::ConfigPath() + std::string("diablo.ini");
	return path;
}

CSimpleIni &GetIni()
{
	static CSimpleIni ini;
	static bool isIniLoaded = false;
	if (!isIniLoaded) {
		auto path = GetIniPath();
		auto stream = CreateFileStream(path.c_str(), std::fstream::in | std::fstream::binary);
		ini.SetSpaces(false);
		if (stream)
			ini.LoadData(*stream);
		isIniLoaded = true;
	}
	return ini;
}

bool IniChanged = false;

/**
 * @brief Checks if a ini entry is changed by comparing value before and after
 */
class IniChangedChecker {
public:
	IniChangedChecker(const char *sectionName, const char *keyName)
	{
		this->sectionName_ = sectionName;
		this->keyName_ = keyName;
		std::list<CSimpleIni::Entry> values;
		if (!GetIni().GetAllValues(sectionName, keyName, values)) {
			// No entry found in original ini => new entry => changed
			IniChanged = true;
		}
		const auto *value = GetIni().GetValue(sectionName, keyName);
		if (value != nullptr)
			oldValue_ = value;
	}
	~IniChangedChecker()
	{
		const auto *value = GetIni().GetValue(sectionName_, keyName_);
		std::string newValue;
		if (value != nullptr)
			newValue = value;
		if (oldValue_ != newValue)
			IniChanged = true;
	}

private:
	std::string oldValue_;
	const char *sectionName_;
	const char *keyName_;
};

int GetIniInt(const char *keyname, const char *valuename, int defaultValue)
{
	return GetIni().GetLongValue(keyname, valuename, defaultValue);
}

bool GetIniBool(const char *sectionName, const char *keyName, bool defaultValue)
{
	return GetIni().GetBoolValue(sectionName, keyName, defaultValue);
}

float GetIniFloat(const char *sectionName, const char *keyName, float defaultValue)
{
	return (float)GetIni().GetDoubleValue(sectionName, keyName, defaultValue);
}

void SetIniValue(const char *keyname, const char *valuename, int value)
{
	IniChangedChecker changedChecker(keyname, valuename);
	GetIni().SetLongValue(keyname, valuename, value);
}

void SetIniValue(const char *keyname, const char *valuename, std::uint8_t value)
{
	IniChangedChecker changedChecker(keyname, valuename);
	GetIni().SetLongValue(keyname, valuename, value);
}

void SetIniValue(const char *keyname, const char *valuename, std::uint32_t value)
{
	IniChangedChecker changedChecker(keyname, valuename);
	GetIni().SetLongValue(keyname, valuename, value);
}

void SetIniValue(const char *keyname, const char *valuename, bool value)
{
	IniChangedChecker changedChecker(keyname, valuename);
	GetIni().SetLongValue(keyname, valuename, value ? 1 : 0);
}

void SetIniValue(const char *keyname, const char *valuename, float value)
{
	IniChangedChecker changedChecker(keyname, valuename);
	GetIni().SetDoubleValue(keyname, valuename, value);
}

void SaveIni()
{
	if (!IniChanged)
		return;
	auto iniPath = GetIniPath();
	auto stream = CreateFileStream(iniPath.c_str(), std::fstream::out | std::fstream::trunc | std::fstream::binary);
	GetIni().Save(*stream, true);
	IniChanged = false;
}

bool HardwareCursorDefault()
{
	return true;
}

} // namespace

void SetIniValue(const char *sectionName, const char *keyName, const char *value, int len)
{
	IniChangedChecker changedChecker(sectionName, keyName);
	auto &ini = GetIni();
	std::string stringValue(value, len != 0 ? len : strlen(value));
	ini.SetValue(sectionName, keyName, stringValue.c_str());
}

bool GetIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, const char *defaultString)
{
	const char *value = GetIni().GetValue(sectionName, keyName);
	if (value == nullptr) {
		strncpy(string, defaultString, stringSize);
		return false;
	}
	strncpy(string, value, stringSize);
	return true;
}

/** Game options */
Options sgOptions;
bool sbWasOptionsLoaded = false;

void LoadOptions()
{
	sgOptions.Diablo.bIntro = GetIniBool("Diablo", "Intro", true);
	GetIniValue("Diablo", "SItem", sgOptions.Diablo.szItem, sizeof(sgOptions.Diablo.szItem), "");

	sgOptions.Audio.nSoundVolume = GetIniInt("Audio", "Sound Volume", VOLUME_MAX);
	sgOptions.Audio.nMusicVolume = GetIniInt("Audio", "Music Volume", VOLUME_MAX);

	sgOptions.Graphics.nWidth = GetIniInt("Graphics", "Width", DEFAULT_WIDTH);
	sgOptions.Graphics.nHeight = GetIniInt("Graphics", "Height", DEFAULT_HEIGHT);
	sgOptions.Graphics.bFullscreen = GetIniBool("Graphics", "Fullscreen", true);
	sgOptions.Graphics.bUpscale = GetIniBool("Graphics", "Upscale", true);
	sgOptions.Graphics.bFitToScreen = GetIniBool("Graphics", "Fit to Screen", true);
	GetIniValue("Graphics", "Scaling Quality", sgOptions.Graphics.szScaleQuality, sizeof(sgOptions.Graphics.szScaleQuality), "2");
	sgOptions.Graphics.bIntegerScaling = GetIniBool("Graphics", "Integer Scaling", false);
	sgOptions.Graphics.bVSync = GetIniBool("Graphics", "Vertical Sync", true);
	sgOptions.Graphics.nGammaCorrection = GetIniInt("Graphics", "Gamma Correction", 100);
	sgOptions.Graphics.bFPSLimit = GetIniBool("Graphics", "FPS Limiter", true);
	sgOptions.Graphics.bShowFPS = (GetIniInt("Graphics", "Show FPS", 0) != 0);

	sgOptions.Gameplay.nTickRate = GetIniInt("Game", "Speed", 20);
	sgOptions.Gameplay.bRunInTown = GetIniBool("Game", "Run in Town", false);
	sgOptions.Gameplay.bGrabInput = GetIniBool("Game", "Grab Input", false);
	sgOptions.Gameplay.bEnemyHealthBar = GetIniBool("Game", "Enemy Health Bar", false);
	sgOptions.Gameplay.bAutoEquipWeapons = GetIniBool("Game", "Auto Equip Weapons", true);
	sgOptions.Gameplay.bAutoEquipArmor = GetIniBool("Game", "Auto Equip Armor", false);
	sgOptions.Gameplay.bAutoEquipHelms = GetIniBool("Game", "Auto Equip Helms", false);
	sgOptions.Gameplay.bAutoEquipShields = GetIniBool("Game", "Auto Equip Shields", false);
	sgOptions.Gameplay.bAutoEquipJewelry = GetIniBool("Game", "Auto Equip Jewelry", false);
	sgOptions.Gameplay.bShowMonsterType = GetIniBool("Game", "Show Monster Type", false);

	keymapper.Load();

	sbWasOptionsLoaded = true;
}

void SaveOptions()
{
	SetIniValue("Diablo", "Intro", sgOptions.Diablo.bIntro);
	SetIniValue("Diablo", "SItem", sgOptions.Diablo.szItem);

	SetIniValue("Audio", "Sound Volume", sgOptions.Audio.nSoundVolume);
	SetIniValue("Audio", "Music Volume", sgOptions.Audio.nMusicVolume);

	SetIniValue("Graphics", "Width", sgOptions.Graphics.nWidth);
	SetIniValue("Graphics", "Height", sgOptions.Graphics.nHeight);
	SetIniValue("Graphics", "Fullscreen", sgOptions.Graphics.bFullscreen);
	SetIniValue("Graphics", "Upscale", sgOptions.Graphics.bUpscale);
	SetIniValue("Graphics", "Fit to Screen", sgOptions.Graphics.bFitToScreen);
	SetIniValue("Graphics", "Scaling Quality", sgOptions.Graphics.szScaleQuality);
	SetIniValue("Graphics", "Integer Scaling", sgOptions.Graphics.bIntegerScaling);
	SetIniValue("Graphics", "Vertical Sync", sgOptions.Graphics.bVSync);
	SetIniValue("Graphics", "Gamma Correction", sgOptions.Graphics.nGammaCorrection);
	SetIniValue("Graphics", "FPS Limiter", sgOptions.Graphics.bFPSLimit);
	SetIniValue("Graphics", "Show FPS", sgOptions.Graphics.bShowFPS);

	SetIniValue("Game", "Speed", sgOptions.Gameplay.nTickRate);
	SetIniValue("Game", "Run in Town", sgOptions.Gameplay.bRunInTown);
	SetIniValue("Game", "Grab Input", sgOptions.Gameplay.bGrabInput);
	SetIniValue("Game", "Enemy Health Bar", sgOptions.Gameplay.bEnemyHealthBar);
	SetIniValue("Game", "Auto Equip Weapons", sgOptions.Gameplay.bAutoEquipWeapons);
	SetIniValue("Game", "Auto Equip Armor", sgOptions.Gameplay.bAutoEquipArmor);
	SetIniValue("Game", "Auto Equip Helms", sgOptions.Gameplay.bAutoEquipHelms);
	SetIniValue("Game", "Auto Equip Shields", sgOptions.Gameplay.bAutoEquipShields);
	SetIniValue("Game", "Auto Equip Jewelry", sgOptions.Gameplay.bAutoEquipJewelry);
	SetIniValue("Game", "Show Monster Type", sgOptions.Gameplay.bShowMonsterType);

	keymapper.Save();

	SaveIni();
}

} // namespace devilution
