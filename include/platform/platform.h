#ifndef PLATFORM_H
#define PLATFORM_H
#include "gryphon.h"

#if defined(__linux__)
#include "platform/x11.h"
#else
#error "Unsupported platform"
#endif

gryphon_window *platform_create_window(arena *a, u32 w, u32 h, string8 title);
b8 platform_window_should_close(gryphon_window *win);

u32 platform_window_width(gryphon_window *win);
u32 platform_window_height(gryphon_window *win);

void platform_poll_events(gryphon_window *win);
void platform_swap_buffers(gryphon_window *win);
void platform_close_window(gryphon_window *win);

#endif
