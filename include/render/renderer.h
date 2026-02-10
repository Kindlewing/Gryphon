#include "arena.h"
#include "base_math.h"

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
} renderer;

renderer renderer_create(arena *a);
void render_begin(renderer *r);
void render_triangle(renderer *r, vector2f32 pos, f32 w, f32 h);
void render_clear(renderer *r, vector4f32 color);
void render_end(renderer *r);
