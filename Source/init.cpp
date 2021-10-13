/**
 * @file init.cpp
 *
 * Implementation of routines for initializing the environment, disable screen saver, load MPQ.
 */
#include <SDL.h>
#include <config.h>
#include <find_steam_game.h>
#include <string>
#include <vector>

#include "DiabloUI/diabloui.h"
#include "dx.h"
#include "pfile.h"
#include "storm/storm.h"
#include "utils/log.hpp"
#include "utils/paths.h"
#include "utils/ui_fwd.h"

namespace devilution {

/** True if the game is the current active window */
bool gbActive;
/** A handle to an hellfire.mpq archive. */
HANDLE hellfire_mpq;
/** The current input handler function */
WNDPROC CurrentProc;
/** A handle to the diabdat.mpq archive. */
HANDLE diabdat_mpq;
/** A handle to the patch_rt.mpq archive. */
HANDLE patch_rt_mpq;
HANDLE hfmonk_mpq;
HANDLE hfbard_mpq;
HANDLE hfbarb_mpq;
HANDLE hfmusic_mpq;
HANDLE hfvoice_mpq;
HANDLE hfopt1_mpq;
HANDLE hfopt2_mpq;
HANDLE devilutionx_mpq;

namespace {

HANDLE LoadMPQ(const std::vector<std::string> &paths, const char *mpqName)
{
	HANDLE archive;
	std::string mpqAbsPath;
	for (const auto &path : paths) {
		mpqAbsPath = path + mpqName;
		if (SFileOpenArchive(mpqAbsPath.c_str(), 0, MPQ_OPEN_READ_ONLY, &archive)) {
			LogVerbose("  Found: {} in {}", mpqName, path);
			SFileSetBasePath(path);
			return archive;
		}
		if (SErrGetLastError() != STORM_ERROR_FILE_NOT_FOUND) {
			LogError("Open error {}: {}", SErrGetLastError(), mpqAbsPath);
		}
	}
	if (SErrGetLastError() == STORM_ERROR_FILE_NOT_FOUND) {
		LogVerbose("Missing: {}", mpqName);
	}

	return nullptr;
}

} // namespace

void init_cleanup()
{
	if (diabdat_mpq != nullptr) {
		SFileCloseArchive(diabdat_mpq);
		diabdat_mpq = nullptr;
	}
	if (patch_rt_mpq != nullptr) {
		SFileCloseArchive(patch_rt_mpq);
		patch_rt_mpq = nullptr;
	}
	if (hellfire_mpq != nullptr) {
		SFileCloseArchive(hellfire_mpq);
		hellfire_mpq = nullptr;
	}
	if (hfmonk_mpq != nullptr) {
		SFileCloseArchive(hfmonk_mpq);
		hfmonk_mpq = nullptr;
	}
	if (hfbard_mpq != nullptr) {
		SFileCloseArchive(hfbard_mpq);
		hfbard_mpq = nullptr;
	}
	if (hfbarb_mpq != nullptr) {
		SFileCloseArchive(hfbarb_mpq);
		hfbarb_mpq = nullptr;
	}
	if (hfmusic_mpq != nullptr) {
		SFileCloseArchive(hfmusic_mpq);
		hfmusic_mpq = nullptr;
	}
	if (hfvoice_mpq != nullptr) {
		SFileCloseArchive(hfvoice_mpq);
		hfvoice_mpq = nullptr;
	}
	if (hfopt1_mpq != nullptr) {
		SFileCloseArchive(hfopt1_mpq);
		hfopt1_mpq = nullptr;
	}
	if (hfopt2_mpq != nullptr) {
		SFileCloseArchive(hfopt2_mpq);
		hfopt2_mpq = nullptr;
	}
	if (devilutionx_mpq != nullptr) {
		SFileCloseArchive(devilutionx_mpq);
		devilutionx_mpq = nullptr;
	}

	NetClose();
}

void init_archives()
{
	std::vector<std::string> paths;
	paths.push_back(paths::BasePath());
	paths.push_back(paths::PrefPath());
	if (paths[0] == paths[1])
		paths.pop_back();

	char gogpath[_FSG_PATH_MAX];
	fsg_get_gog_game_path(gogpath, "1412601690");
	if (strlen(gogpath) > 0) {
		paths.emplace_back(std::string(gogpath) + "/");
		paths.emplace_back(std::string(gogpath) + "/hellfire/");
	}

	paths.emplace_back(""); // PWD

	if (SDL_LOG_PRIORITY_VERBOSE >= SDL_LogGetPriority(SDL_LOG_CATEGORY_APPLICATION)) {
		std::string message;
		for (std::size_t i = 0; i < paths.size(); ++i) {
			char prefix[32];
			std::snprintf(prefix, sizeof(prefix), "\n%6u. '", static_cast<unsigned>(i + 1));
			message.append(prefix);
			message.append(paths[i]);
			message += '\'';
		}
		LogVerbose("MPQ search paths:{}", message);
	}

	diabdat_mpq = LoadMPQ(paths, "DIABDAT.MPQ");
	if (diabdat_mpq == nullptr) {
		// DIABDAT.MPQ is uppercase on the original CD and the GOG version.
		diabdat_mpq = LoadMPQ(paths, "diabdat.mpq");
	}

	HANDLE fh = nullptr;
	if (!SFileOpenFile("ui_art\\title.pcx", &fh))
		InsertCDDlg();
	SFileCloseFileThreadSafe(fh);

	patch_rt_mpq = LoadMPQ(paths, "patch_rt.mpq");
	if (patch_rt_mpq == nullptr)
		patch_rt_mpq = LoadMPQ(paths, "patch_sh.mpq");

	hellfire_mpq = LoadMPQ(paths, "hellfire.mpq");
	hfmonk_mpq = LoadMPQ(paths, "hfmonk.mpq");
	hfbard_mpq = LoadMPQ(paths, "hfbard.mpq");
	if (hfbard_mpq != nullptr)
		gbBard = true;
	hfbarb_mpq = LoadMPQ(paths, "hfbarb.mpq");
	if (hfbarb_mpq != nullptr)
		gbBarbarian = true;
	hfmusic_mpq = LoadMPQ(paths, "hfmusic.mpq");
	hfvoice_mpq = LoadMPQ(paths, "hfvoice.mpq");
	hfopt1_mpq = LoadMPQ(paths, "hfopt1.mpq");
	hfopt2_mpq = LoadMPQ(paths, "hfopt2.mpq");

	devilutionx_mpq = LoadMPQ(paths, "devilutionx.mpq");
}

void init_create_window()
{
	if (!SpawnWindow(PROJECT_NAME))
		app_fatal("%s", "Unable to create main window");
	dx_init();
	gbActive = true;
	SDL_DisableScreenSaver();
}

void MainWndProc(uint32_t msg)
{
	switch (msg) {
	case DVL_WM_PAINT:
		force_redraw = 255;
		break;
	case DVL_WM_QUERYENDSESSION:
		diablo_quit(0);
	}
}

WNDPROC SetWindowProc(WNDPROC newProc)
{
	WNDPROC oldProc;

	oldProc = CurrentProc;
	CurrentProc = newProc;
	return oldProc;
}

} // namespace devilution
