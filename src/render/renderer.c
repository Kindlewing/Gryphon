#include "render/renderer.h"
#include "render/shader.h"
#include "stdio.h"
#include <glad.h>

static render_pipeline pipeline_create(arena *a, f32 *vertices, u32 vertex_count,
									   u32 *indices, u32 index_count, string8 vertex_path,
									   string8 fragment_path) {
	render_pipeline p = {0};

	u32 vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	u32 vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * vertex_count, vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void *)0);
	glEnableVertexAttribArray(0);

	u32 idx_buffer = 0;
	glGenBuffers(1, &idx_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, idx_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices,
				 GL_STATIC_DRAW);

	shader s;
	if(!shader_init(a, &s, vertex_path, fragment_path)) {
		fprintf(stderr, "Failed to load shader!\n");
		exit(EXIT_FAILURE);
	}

	p.vertex_array = vao;
	p.vertex_buffer = vbo;
	p.idx_buffer = idx_buffer;
	p.vertex_count = index_count;
	p.shader_program = s.id;
	return p;
}

renderer renderer_create(arena *a, u32 fb_width, u32 fb_height) {
	renderer r = {0};
	// clang-format off
	f32 triangle_vertices[] = {
		-0.5f, -0.5f, 0.0f,  // bottom left;
		 0.0f,  0.5f, 0.0f, // top middle,
		 0.5f, -0.5f, 0.0f // bottom right
	};

	u32 triangle_indices[] = {
		0, 1, 2
	};
	// clang-format on
	string8 vertex_shader_source = string8_lit("shaders/vertex.glsl");
	string8 frag_shader_source = string8_lit("shaders/fragment.glsl");

	r.triangle_pipeline = pipeline_create(a, triangle_vertices, 9, triangle_indices, 3,
										  vertex_shader_source, frag_shader_source);
	// clang-format off
	f32 quad_vertices[] = {
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left 		   
	};

	u32 quad_indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	// clang-format on
	r.quad_pipeline = pipeline_create(a, quad_vertices, 12, quad_indices, 6,
									  vertex_shader_source, frag_shader_source);
	r.width = fb_width;
	r.height = fb_height;
	return r;
}

void render_begin(renderer *r) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, r->width, r->height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void render_triangle(renderer *r, vector2f32 pos, f32 w, f32 h) {
	glUseProgram(r->triangle_pipeline.shader_program);
	glBindVertexArray(r->triangle_pipeline.vertex_array);
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
}

void render_clear(vector4f32 color) {
	glClearColor(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void render_end(renderer *r) {
}
