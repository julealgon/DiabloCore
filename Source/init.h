/**
 * @file init.h
 *
 * Interface of routines for initializing the environment, disable screen saver, load MPQ.
 */
#pragma once

#include "miniwin/miniwin.h"

namespace devilution {

extern bool gbActive;
extern HANDLE hellfire_mpq;
extern WNDPROC CurrentProc;
extern HANDLE diabdat_mpq;
extern HANDLE patch_rt_mpq;
extern HANDLE hfmonk_mpq;
extern HANDLE hfmusic_mpq;
extern HANDLE hfvoice_mpq;
extern HANDLE diablocore_mpq;

void init_cleanup();
void init_archives();
void init_create_window();
void MainWndProc(uint32_t Msg);
WNDPROC SetWindowProc(WNDPROC NewProc);

} // namespace devilution
