#include "gryphon.c"
#include "typedefs.h"

// Platform-specific stuff
#ifdef _WIN32
#include "core/win32_arena.c"
#elif defined(__linux__)
#include "core/linux_arena.c"
#endif
