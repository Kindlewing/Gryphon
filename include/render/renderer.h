#ifndef RENDERER_H
#define RENDERER_H

#include "arena.h"
#include "gryphon.h"
#include "linalg.h"

typedef struct renderer renderer;

renderer *renderer_create(arena *a, gryphon_window *win);
void render_begin(arena *frame_arena, renderer *r);
void render_set_clear_color(renderer *r);
void render_set_framebuffer(renderer *r, u32 framebuffer, u32 w, u32 h);
void render_push_triangle(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot);
void render_push_quad(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot);
void render_clear(vector4f32 color);
void render_end(renderer *r);
#endif
