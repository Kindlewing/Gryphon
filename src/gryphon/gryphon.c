#include "gryphon.h"
#include "string8.h"
#include "typedefs.h"
#include "glad.h"

#if defined(__linux__)
#include "gryphon/platform/x11.h"
#else
#error "Unsupported platform"
#endif

gry_window *gry_create_window(arena *a, u32 width, u32 height, string8 title) {
	return platform_create_window(a, width, height, title);
}

void gry_poll_events(gry_window *win) {
	platform_poll_events(win);
}

b8 gry_window_should_close(gry_window *win) {
	return platform_window_should_close(win);
}

void gry_swap_buffers(gry_window *window) {
	platform_swap_buffers(window);
}

void gry_close_window(gry_window *win) {
	platform_close_window(win);
}
