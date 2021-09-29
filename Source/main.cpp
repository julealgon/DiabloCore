#include <SDL.h>

#include "diablo.h"

extern "C" const char *__asan_default_options() // NOLINT(bugprone-reserved-identifier, readability-identifier-naming)
{
	return "halt_on_error=0";
}

int main(int argc, char **argv)
{
	return devilution::DiabloMain(argc, argv);
}
