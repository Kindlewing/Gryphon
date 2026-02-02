#include "glad.h"
#include "arena.h"
#include "macros.h"
#include "string8.h"
#include "profile.h"
#include "base_math.h"
#include "gryphon.h"
#include "render/shader.h"

#define WINDOW_W 1200
#define WINDOW_H 1200

int main(void) {
	begin_profile();
	begin_time_function;
	arena *engine_arena = arena_create(KiB(64));
	string8 title = string8_lit("Spark Engine");
	gry_window *window = gry_create_window(engine_arena, WINDOW_W, WINDOW_H, title);

	if(window == NULL) {
		string8 err = string8_lit("An error occured: cannot open display.\n");
		write(1, err.data, err.length);
		return -1;
	}

	// clang-format off
	f32 vertices[] = {
		-0.5f, -0.5f, 0.0f,  // bottom left;
		 0.0f,  0.5f, 0.0f, // top middle,
		 0.5f, -0.5f, 0.0f // bottom right

	};
	u32 indices[] = {
		0, 1, 2
	};
	// clang-format on
	u32 vertex_array;
	glGenVertexArrays(1, &vertex_array);
	glBindVertexArray(vertex_array);

	u32 vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void *)0);
	glEnableVertexAttribArray(0);

	u32 index_buffer;
	glGenBuffers(1, &index_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// shaders
	arena *shader_arena = arena_create(KiB(500));
	shader s;
	string8 v_path = string8_lit("assets/shaders/vertex.glsl");
	string8 f_path = string8_lit("assets/shaders/fragment.glsl");
	shader_init(shader_arena, &s, v_path, f_path);
	glUseProgram(s.id);

	while(!gry_window_should_close(window)) {
		begin_time_block("Game Loop");
		gry_poll_events(window);
		glClearColor(33.0 / 255.0, 33.0 / 255.0, 33.0 / 255.0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(s.id);
		glBindVertexArray(vertex_array);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
		gry_swap_buffers(window);
		end_time_block;
	}

	gry_close_window(window);
	end_time_function;
	end_profile();
	return 0;
}
