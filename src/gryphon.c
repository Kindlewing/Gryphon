#include "gryphon.h"
#include "string8.h"
#include "typedefs.h"
#include "glad.h"

#if defined(__linux__)
#include "platform/x11.h"
#else
#error "Unsupported platform"
#endif

gryphon_window *gryphon_create_window(arena *a, u32 width, u32 height, string8 title) {
	return platform_create_window(a, width, height, title);
}

void gryphon_poll_events(gryphon_window *win) {
	platform_poll_events(win);
}

b8 gryphon_window_should_close(gryphon_window *win) {
	return platform_window_should_close(win);
}

void gryphon_swap_buffers(gryphon_window *win) {
	platform_swap_buffers(win);
}

void gryphon_close_window(gryphon_window *win) {
	platform_close_window(win);
}
