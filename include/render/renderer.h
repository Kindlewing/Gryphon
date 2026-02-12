#ifndef RENDERER_H
#define RENDERER_H

#include "arena.h"
#include "gryphon.h"
#include "linalg.h"

typedef struct render_pipeline {
	u32 vertex_buffer;
	u32 vertex_array;
	u32 idx_buffer;
	u32 vertex_count;
	u32 shader_program;
} render_pipeline;

typedef struct renderer {
	render_pipeline triangle_pipeline;
	render_pipeline quad_pipeline;

	vector4f32 clear_color;

	u32 framebuffer;
	u32 framebuffer_width;
	u32 framebuffer_height;

	gryphon_window *win;
} renderer;

typedef struct vertex {
	f32 x;
	f32 y;
	f32 z;
} vertex;

renderer renderer_create(arena *a, gryphon_window *win);
void render_begin(renderer *r);
void render_set_clear_color(renderer *r);
void render_set_framebuffer(renderer *r, u32 framebuffer, u32 w, u32 h);
void render_triangle(renderer *r, vector2f32 pos, f32 w, f32 h, f32 rot);
void render_quad(renderer *r, vector2f32 pos, f32 w, f32 h);
void render_clear(vector4f32 color);
void render_end(renderer *r);
#endif
