#include "arena.h"
#include "profile.h"
#include "linalg.h"
#include "gryphon.h"
#include "render/renderer.h"
#include "string8.h"
#include <time.h>
#include <unistd.h>

#define W 800
#define H 600
#define BOID_COUNT 60

typedef struct boid {
	vector2f32 pos;
	vector2f32 vel;
	f32 size_x;
	f32 size_y;
	f64 rotation;
} boid;

void draw_boid(renderer *r, boid *boid) {
	render_triangle(r, (vector3f32){boid->pos.x, boid->pos.y, 1.0}, boid->size_x,
					boid->size_y, boid->rotation);
}

int main(void) {
	begin_profile();

	gryphon_init();
	arena *engine_arena = arena_create(KiB(64));
	string8 title = string8_lit("Spark Engine");
	gryphon_window *window = gryphon_create_window(engine_arena, W, H, title);
	if(window == NULL) {
		string8 err = string8_lit("An error occured: cannot open display.\n");
		write(1, err.data, err.length);
		return -1;
	}

	renderer *renderer = renderer_create(engine_arena, window);

	arena *boids_arena = arena_create(sizeof(boid) * BOID_COUNT);
	arena *frame_arena = arena_create(KiB(8));

	boid *boids = arena_push_array(boids_arena, boid, BOID_COUNT);

	for(i32 i = 0; i < BOID_COUNT; i += 1) {
		boid *b = &boids[i];
		b->pos = vector2f32_make(W / 2.0f, H / 2.0f);
		b->vel = vector2f32_make(0.0f, 0.0f);
		b->size_x = 10.0f;
		b->size_y = 20.0f;
		b->rotation = 0.0f;
	}

	while(!gryphon_window_should_close(window)) {
		begin_time_block("main loop");

		arena_clear(frame_arena);
		gryphon_poll_events(window);

		render_begin(renderer);
		render_clear((vector4f32){33.0, 33.0, 33.0, 1.0});
		for(i32 i = 0; i < BOID_COUNT; i += 1) {
			draw_boid(renderer, &boids[i]);
		}
		render_end(renderer);
		end_time_block;
	}

	gryphon_close_window(window);
	arena_free(engine_arena);
	arena_free(frame_arena);
	end_profile(true);
	return 0;
}
