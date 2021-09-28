/**
 * @file dthread.cpp
 *
 * Implementation of functions for updating game state from network commands.
 */

#include <list>
#include <mutex>

#include "nthread.h"
#include "utils/sdl_cond.h"
#include "utils/sdl_thread.h"

namespace devilution {

struct DThreadPkt {
	int pnum;
	_cmd_id cmd;
	std::unique_ptr<std::byte[]> data;
	uint32_t len;

	DThreadPkt(int pnum, _cmd_id(cmd), std::unique_ptr<std::byte[]> data, uint32_t len)
	    : pnum(pnum)
	    , cmd(cmd)
	    , data(std::move(data))
	    , len(len)
	{
	}
};

namespace {

std::optional<SdlMutex> DthreadMutex;
std::list<DThreadPkt> InfoList;
bool DthreadRunning;
std::optional<SdlCond> WorkToDo;

/* rdata */
SdlThread Thread;

} // namespace

void DThreadCleanup()
{
	if (!DthreadRunning)
		return;

	{
		std::lock_guard<SdlMutex> lock(*DthreadMutex);
		DthreadRunning = false;
		InfoList.clear();
		WorkToDo->signal();
	}

	Thread.join();
	DthreadMutex = std::nullopt;
	WorkToDo = std::nullopt;
}

} // namespace devilution
