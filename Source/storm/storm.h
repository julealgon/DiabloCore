#pragma once

#include <cerrno>
#include <cstdint>
#include <limits>
#include <string>
#include <cstdint>

#include "appfat.h"
#include "multi.h"

namespace devilution {

enum game_info : uint8_t {
	GAMEINFO_NAME,
	GAMEINFO_PASSWORD,
};

enum conn_type : uint8_t {
	SELCONN_LOOPBACK,
};

struct PCXHeader {
	uint8_t Manufacturer;
	uint8_t Version;
	uint8_t Encoding;
	uint8_t BitsPerPixel;
	uint16_t Xmin;
	uint16_t Ymin;
	uint16_t Xmax;
	uint16_t Ymax;
	uint16_t HDpi;
	uint16_t VDpi;
	uint8_t Colormap[48];
	uint8_t Reserved;
	uint8_t NPlanes;
	uint16_t BytesPerLine;
	uint16_t PaletteInfo;
	uint16_t HscreenSize;
	uint16_t VscreenSize;
	uint8_t Filler[54];
};

struct _SNETCAPS {
	uint32_t size;
	uint32_t flags;
	uint32_t maxmessagesize;
	uint32_t maxqueuesize;
	uint32_t maxplayers;
	uint32_t bytessec;
	uint32_t latencyms;
	uint32_t defaultturnssec;
	uint32_t defaultturnsintransit;
};

struct _SNETEVENT {
	uint32_t eventid;
	uint32_t playerid;
	void *data;
	uint32_t databytes;
};

// Note to self: Linker error => forgot a return value in cpp

// We declare the StormLib methods we use here.
// StormLib uses the Windows calling convention on Windows for these methods.
#define WINAPI __stdcall

// Game states
#define GAMESTATE_PRIVATE 0x01
#define GAMESTATE_FULL 0x02
#define GAMESTATE_ACTIVE 0x04
#define GAMESTATE_STARTED 0x08
#define GAMESTATE_REPLAY 0x80

#define PS_CONNECTED 0x10000
#define PS_TURN_ARRIVED 0x20000
#define PS_ACTIVE 0x40000

#define LEAVE_ENDING 0x40000004
#define LEAVE_DROP 0x40000006

#if defined(__cplusplus)
extern "C" {
#endif

bool SNetCreateGame(char *GameTemplateData, int GameTemplateSize);

bool SNetReceiveMessage(int *senderplayerid, void **data, uint32_t *databytes);
bool SNetReceiveTurns(int arraysize, char **arraydata, size_t *arraydatabytes);

/*  SNetSendMessage @ 127
 *
 *  Sends a message to a player given their player ID. Network message
 *  is sent using class 01 and is retrieved by the other client using
 *  SNetReceiveMessage().
 *
 *  playerID:   The player index of the player to receive the data.
 *              Conversely, this field can be one of the following constants:
 *                  SNPLAYER_ALL      | Sends the message to all players, including oneself.
 *  data:       A pointer to the data.
 *  databytes:  The amount of bytes that the data pointer contains.
 *
 *  Returns true if the function was called successfully and false otherwise.
 */
bool SNetSendMessage(int playerID, void *data, unsigned int databytes);

// Macro values to target specific players
#define SNPLAYER_ALL -1

#define MPQ_OPEN_READ_ONLY 0x00000100
#define SFILE_OPEN_FROM_MPQ 0
#define SFILE_OPEN_LOCAL_FILE 0xFFFFFFFF

bool SFileOpenFile(const char *filename, HANDLE *phFile);

// Functions implemented in StormLib
bool WINAPI SFileOpenArchive(const wchar_t *szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE *phMpq);
bool WINAPI SFileCloseArchive(HANDLE hArchive);
bool WINAPI SFileOpenFileEx(HANDLE hMpq, const char *szFileName, DWORD dwSearchScope, HANDLE *phFile);
bool WINAPI SFileReadFile(HANDLE hFile, void *buffer, size_t nNumberOfBytesToRead, size_t *read, int *lpDistanceToMoveHigh);
DWORD WINAPI SFileGetFileSize(HANDLE hFile, uint32_t *lpFileSizeHigh = nullptr);
DWORD WINAPI SFileSetFilePointer(HANDLE, int, int *, int);
bool WINAPI SFileCloseFile(HANDLE hFile);

// These error codes are used and returned by StormLib.
// See StormLib/src/StormPort.h
// https://docs.microsoft.com/en-us/windows/win32/debug/system-error-codes--0-499-
#define STORM_ERROR_FILE_NOT_FOUND 2
#define STORM_ERROR_HANDLE_EOF 38

/*  SErrGetLastError @ 463
 *
 *  Retrieves the last error that was specifically
 *  set for the Storm library.
 *
 *  Returns the last error set within the Storm library.
 */
uint32_t SErrGetLastError();

/*  SErrSetLastError @ 465
 *
 *  Sets the last error for the Storm library and the Kernel32 library.
 *
 *  dwErrCode:  The error code that will be set.
 */
void SErrSetLastError(uint32_t dwErrCode);

// Values for dwErrCode
#define STORM_ERROR_GAME_TERMINATED 0x85100069
#define STORM_ERROR_INVALID_PLAYER 0x8510006a
#define STORM_ERROR_NO_MESSAGES_WAITING 0x8510006b
#define STORM_ERROR_NOT_IN_GAME 0x85100070

/*  SStrCopy @ 501
 *
 *  Copies a string from src to dest (including NULL terminator)
 *  until the max_length is reached.
 *
 *  dest:         The destination array.
 *  src:          The source array.
 *  max_length:   The maximum length of dest.
 *
 */
void SStrCopy(char *dest, const char *src, int max_length);

void SFileSetBasePath(std::string_view path);
bool SNetGetOwnerTurnsWaiting(uint32_t *);
bool SNetInitializeProvider(uint32_t provider, struct GameData *gameData);
void SNetGetProviderCaps(struct _SNETCAPS *);
bool SFileEnableDirectAccess(bool enable);

#if defined(__cplusplus)
}

// Additions to Storm API:
// On Windows, handles wchar conversion and calls the wchar version of SFileOpenArchive.
bool SFileOpenArchive(const char *szMpqName, DWORD dwPriority, DWORD dwFlags, HANDLE *phMpq);

// Locks ReadFile and CloseFile under a mutex.
// See https://github.com/ladislav-zezula/StormLib/issues/175
bool SFileReadFileThreadSafe(HANDLE hFile, void *buffer, size_t nNumberOfBytesToRead, size_t *read = nullptr, int *lpDistanceToMoveHigh = nullptr);
bool SFileCloseFileThreadSafe(HANDLE hFile);

// Sets the file's 64-bit seek position.
inline std::uint64_t SFileSetFilePointer(HANDLE hFile, std::int64_t offset, int whence)
{
	int high = static_cast<std::uint64_t>(offset) >> 32;
	int low = static_cast<int>(offset);
	low = SFileSetFilePointer(hFile, low, &high, whence);
	return (static_cast<std::uint64_t>(high) << 32) | low;
}

// Returns the current 64-bit file seek position.
inline std::uint64_t SFileGetFilePointer(HANDLE hFile)
{
	// We use `SFileSetFilePointer` with offset 0 to get the current position
	// because there is no `SFileGetFilePointer`.
	return SFileSetFilePointer(hFile, 0, DVL_FILE_CURRENT);
}

#endif

} // namespace devilution
