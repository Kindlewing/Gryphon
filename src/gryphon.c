#include "gryphon.h"
#include "string8.h"
#include "typedefs.h"
#include "glad.h"

#include "platform/platform.h"

struct gryphon_state {
	f64 starting_time;
	b8 init_success;
};

static struct gryphon_state global_state;

void gryphon_init(void) {
}

f64 gryphon_get_time(void) {
	return time_now() - global_state.starting_time;
}

gryphon_window *gryphon_create_window(arena *a, u32 width, u32 height, string8 title) {
	return platform_create_window(a, width, height, title);
}

u32 gryphon_window_width(gryphon_window *win) {
	return platform_window_width(win);
}

u32 gryphon_window_height(gryphon_window *win) {
	return platform_window_height(win);
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
