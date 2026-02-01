#include "base/arena.h"
#include "base/string8.h"
#include "base/typedefs.h"
#include "gryphon/gryphon.h"
#include <X11/X.h>
#include <X11/Xlib.h>

gry_window *platform_create_window(arena *a, u32 w, u32 h, string8 title);
b8 platform_window_should_close(gry_window *win);

void platform_poll_events(gry_window *win);
void platform_swap_buffers(gry_window *win);
void platform_close_window(gry_window *win);
