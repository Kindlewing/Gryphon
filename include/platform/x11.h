#include "arena.h"
#include "string8.h"
#include "typedefs.h"
#include "gryphon.h"
#include <X11/X.h>
#include <X11/Xlib.h>

gryphon_window *platform_create_window(arena *a, u32 w, u32 h, string8 title);
b8 platform_window_should_close(gryphon_window *win);

void platform_poll_events(gryphon_window *win);
void platform_swap_buffers(gryphon_window *win);
void platform_close_window(gryphon_window *win);
