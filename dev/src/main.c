#include "profile.h"
#include "linalg.h"
#include "gryphon.h"
#include "render/renderer.h"
#include "string8.h"
#include <stdio.h>
#include <unistd.h>

#define W 1200
#define H 1200

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

	arena *frame_arena = arena_create(KiB(8));
	while(!gryphon_window_should_close(window)) {
		begin_time_block("main loop");

		arena_clear(frame_arena);
		gryphon_poll_events(window);
		render_begin(&renderer);

		render_clear((vector4f32){33.0, 33.0, 33.0, 1.0});
		render_triangle(&renderer, (vector2f32){2.0f, 4.0f}, 20.0f, 20.0f);
		render_quad(&renderer, (vector2f32){4.0f, 3.0f}, 20.0f, 20.0f);

		render_end(&renderer);
		end_time_block;
	}

	gryphon_close_window(window);
	arena_free(engine_arena);
	arena_free(frame_arena);
	end_profile(true);
	return 0;
}
