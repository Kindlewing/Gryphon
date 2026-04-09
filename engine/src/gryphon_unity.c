#include "gryphon.c"

// Platform-specific stuff
#ifdef _WIN32

#elif defined(__linux__)
#include "core/arena_linux.c"
#endif
