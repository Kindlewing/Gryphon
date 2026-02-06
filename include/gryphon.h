#ifndef GRYPHON_H
#define GRYPHON_H
#include "arena.h"
#include "string8.h"

typedef struct gry_window gry_window;

gry_window *gry_create_window(arena *a, u32 width, u32 height, string8 title);
b8 gry_window_should_close(gry_window *win);
void gry_swap_buffers(gry_window *window);
void gry_poll_events(gry_window *win);
void gry_close_window(gry_window *win);

#endif // !GRYPHON_H
