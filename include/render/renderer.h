#ifndef RENDERER_H
#define RENDERER_H

#include "arena.h"
#include "gryphon.h"
#include "linalg.h"

typedef struct renderer renderer;

renderer *renderer_create(arena *a, gryphon_window *win);
void render_begin(renderer *r);
void render_push_clear(renderer *r, vector4f32 color);
void render_push_triangle(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot);
void render_push_quad(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot);
void render_end(renderer *r);
#endif
