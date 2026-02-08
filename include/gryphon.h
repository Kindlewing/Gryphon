#ifndef GRYPHON_H
#define GRYPHON_H
#include "arena.h"
#include "string8.h"
#include "render/renderer.h"
#include "render/shader.h"
#include "macros.h"

typedef struct gryphon_window gryphon_window;

gryphon_window *gryphon_create_window(arena *a, u32 width, u32 height, string8 title);
b8 gryphon_window_should_close(gryphon_window *win);
void gryphon_swap_buffers(gryphon_window *window);
void gryphon_poll_events(gryphon_window *win);
void gryphon_close_window(gryphon_window *win);

#endif // !GRYPHON_H
