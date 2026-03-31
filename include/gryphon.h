#ifndef GRYPHON_H
#define GRYPHON_H
#include "arena.h"
#include "string8.h"
#include "macros.h"
#include "typedefs.h"
#include "linalg.h"

typedef struct gryphon_window gryphon_window;
typedef struct renderer renderer;

void gryphon_init(void);
f64  gryphon_get_time(void);

gryphon_window *gryphon_create_window(arena *a, u32 w, u32 h, string8 title);
b8   gryphon_window_should_close(gryphon_window *win);
u32  gryphon_window_width(gryphon_window *win);
u32  gryphon_window_height(gryphon_window *win);
void gryphon_poll_events(gryphon_window *win);
void gryphon_close_window(gryphon_window *win);

renderer *renderer_create(arena *a, gryphon_window *win);
void render_begin(renderer *r);
void render_push_clear(renderer *r, vector4f32 color);
void render_push_triangle(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot);
void render_push_quad(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot);
void render_end(renderer *r);

#endif // !GRYPHON_H
