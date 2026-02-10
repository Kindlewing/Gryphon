#include "arena.h"

typedef struct renderer {

	arena frame_arena;
} renderer;

renderer renderer_create();
void render_begin(renderer *r);
void renderer_draw_triangle(renderer *r);
void render_end(renderer *r);
