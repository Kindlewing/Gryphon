#ifndef GRYPHON_H
#define GRYPHON_H
#include "arena.h"
#include "string8.h"
#include "macros.h"
#include "platform/platform.h"

void gryphon_init(void);
f64 gryphon_get_time(void);

#define gryphon_create_window        platform_create_window
#define gryphon_window_width         platform_window_width
#define gryphon_window_height        platform_window_height
#define gryphon_window_should_close  platform_window_should_close
#define gryphon_poll_events          platform_poll_events
#define gryphon_close_window         platform_close_window

#endif // !GRYPHON_H
