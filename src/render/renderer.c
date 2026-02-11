#include "render/renderer.h"
#include "gryphon.h"
#include "render/shader.h"
#include "linalg.h"
#include "platform/platform.h"
#include "stdio.h"
#include <glad.h>

static render_pipeline pipeline_create(arena *a, vertex *vertices, u32 vertex_count,
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

renderer renderer_create(arena *a, gryphon_window *win) {
	renderer r = {0};
	// clang-format off
	vertex triangle_vertices[] = {
		{-0.5f, -0.5f, 0.0f},  // bottom left;
		{ 0.0f,  0.5f, 0.0f}, // top middle,
		{ 0.5f, -0.5f, 0.0f} // bottom right
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
	vertex quad_vertices[] = {
		{ 0.5f,  0.5f, 0.0f },  // top right
		{ 0.5f, -0.5f, 0.0f },  // bottom right
		{-0.5f, -0.5f, 0.0f },  // bottom left
		{-0.5f,  0.5f, 0.0f }   // top left 		   
	};

	u32 quad_indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	// clang-format on
	r.quad_pipeline = pipeline_create(a, quad_vertices, 12, quad_indices, 6,
									  vertex_shader_source, frag_shader_source);
	r.win = win;
	u32 width = gryphon_window_width(r.win);
	u32 height = gryphon_window_height(r.win);

	// set the default framebuffer
	render_set_framebuffer(&r, 0, width, height);
	return r;
}

void render_set_framebuffer(renderer *r, u32 framebuffer, u32 w, u32 h) {
	r->framebuffer = framebuffer;
	r->framebuffer_width = w;
	r->framebuffer_height = h;
}

void render_begin(renderer *r) {
	glBindFramebuffer(GL_FRAMEBUFFER, r->framebuffer);
	glViewport(0, 0, r->framebuffer_width, r->framebuffer_height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void render_triangle(renderer *r, vector2f32 pos, f32 w, f32 h) {
	u32 shader_id = r->triangle_pipeline.shader_program;
	glUseProgram(shader_id);
	u32 transformm_loc = glGetUniformLocation(shader_id, "transform");

	mat4x4f32 trans = mat4f32_identity();
	trans = mat4f32_scale(trans, (vector3f32){w, h, 1.0});

	glUniformMatrix4fv(transformm_loc, 1, false, trans.m);
	glBindVertexArray(r->triangle_pipeline.vertex_array);
	glDrawElements(GL_TRIANGLES, r->triangle_pipeline.vertex_count, GL_UNSIGNED_INT, 0);
}

void render_quad(renderer *r, vector2f32 pos, f32 w, f32 h) {
	glUseProgram(r->quad_pipeline.shader_program);
	glBindVertexArray(r->quad_pipeline.vertex_array);
	glDrawElements(GL_TRIANGLES, r->quad_pipeline.vertex_count, GL_UNSIGNED_INT, 0);
}

void render_clear(vector4f32 color) {
	glClearColor(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void render_end(renderer *r) {
	platform_swap_buffers(r->win);
}
