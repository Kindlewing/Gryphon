#include "arena.h"
#include "profile.h"
#include "linalg.h"
#include "gryphon.h"
#include "render/renderer.h"
#include "string8.h"
#include <stdio.h>
#include <unistd.h>

#define W 800
#define H 600

typedef struct boid {
	vector2f32 pos;
} boid;

int main(void) {
	begin_profile();
	arena *engine_arena = arena_create(KiB(64));
	string8 title = string8_lit("Spark Engine");
	gryphon_window *window = gryphon_create_window(engine_arena, W, H, title);
	if(window == NULL) {
		string8 err = string8_lit("An error occured: cannot open display.\n");
		write(1, err.data, err.length);
		return -1;
	}

	renderer renderer = renderer_create(engine_arena, window);

	arena *boids_arena = arena_create(sizeof(boid) * 32);
	arena *frame_arena = arena_create(KiB(8));

	boid *boids = arena_push_array(boids_arena, boid, 32);

	for(i32 i = 0; i < 32; i += 1) {
		boids[i] = (boid){.pos = (vector2f32){i + 25, i + 20}};
	}

	while(!gryphon_window_should_close(window)) {
		begin_time_block("main loop");

		arena_clear(frame_arena);
		gryphon_poll_events(window);

		for(i32 i = 0; i < 32; i += 1) {
			boids[i].pos.x += 3;
			boids[i].pos.y -= 0.5f;
		}

		render_begin(&renderer);
		render_clear((vector4f32){33.0, 33.0, 33.0, 1.0});

		for(i32 i = 0; i < 32; i += 1) {
			render_triangle(&renderer, (vector3f32){boids[i].pos.x, boids[i].pos.y, 1.0},
							-20.0f, -40.0f, 0.0f);
		}

		render_end(&renderer);
		end_time_block;
	}

	gryphon_close_window(window);
	arena_free(engine_arena);
	arena_free(frame_arena);
	end_profile(true);
	return 0;
}
