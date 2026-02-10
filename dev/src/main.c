#include "base_math.h"
#include "glad.h"
#include "profile.h"
#include "gryphon.h"
#include "string8.h"
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

	renderer renderer = renderer_create(engine_arena, W, H);

	while(!gryphon_window_should_close(window)) {
		begin_time_block("main loop");
		gryphon_poll_events(window);
		render_clear((vector4f32){33.0, 33.0, 33.0, 1.0});

		render_triangle(&renderer, (vector2f32){2.0f, 4.0f}, 20.0f, 20.0f);

		gryphon_swap_buffers(window);
		end_time_block;
	}

	gryphon_close_window(window);
	arena_free(engine_arena);
	end_profile();
	return 0;
}
