#include "render/renderer.h"
#include "gryphon.h"
#include "render/shader.h"
#include "linalg.h"
#include <stdio.h>
#include <glad.h>

#define COMMAND_BUFFER_CAPACITY (1 << 16)

typedef enum {
	RENDER_TRIANGLE,
	RENDER_QUAD,
	RENDER_TYPE_COUNT,
} render_command_type;

typedef struct {
	render_command_type type;
	vector3f32 pos;
	f32 w, h, rot;
} render_command;

typedef struct {
	u32 vertex_buffer;
	u32 vertex_array;
	u32 idx_buffer;
	u32 vertex_count;
	u32 shader_program;
} render_pipeline;

struct renderer {
	arena *render_arena;
	render_command *commands;
	u32 command_count;
	render_pipeline pipelines[RENDER_TYPE_COUNT];
	mat4x4f32 projection;
	vector4f32 clear_color;
	u32 framebuffer;
	u32 framebuffer_width;
	u32 framebuffer_height;
	gryphon_window *win;
};

static render_pipeline pipeline_create(arena *a, vector3f32 *vertices, u32 vertex_count,
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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices, GL_STATIC_DRAW);

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

renderer *renderer_create(arena *a, gryphon_window *win) {
	renderer *r = arena_push_struct(a, renderer);
	r->win = win;
	u32 width = platform_window_width(win);
	u32 height = platform_window_height(win);
	r->projection = mat4f32_ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);

	string8 vert = string8_lit("shaders/vertex.glsl");
	string8 frag = string8_lit("shaders/fragment.glsl");

	// clang-format off
	vector3f32 triangle_vertices[] = {
		{-0.5f, -0.5f, 0.0f},
		{ 0.0f,  0.5f, 0.0f},
		{ 0.5f, -0.5f, 0.0f},
	};
	u32 triangle_indices[] = { 0, 1, 2 };

	vector3f32 quad_vertices[] = {
		{ 0.5f,  0.5f, 0.0f},
		{ 0.5f, -0.5f, 0.0f},
		{-0.5f, -0.5f, 0.0f},
		{-0.5f,  0.5f, 0.0f},
	};
	u32 quad_indices[] = { 0, 1, 3, 1, 2, 3 };
	// clang-format on

	r->pipelines[RENDER_TRIANGLE] = pipeline_create(a, triangle_vertices, 9, triangle_indices, 3, vert, frag);
	r->pipelines[RENDER_QUAD] = pipeline_create(a, quad_vertices, 12, quad_indices, 6, vert, frag);

	r->framebuffer = 0;
	r->framebuffer_width = width;
	r->framebuffer_height = height;

	r->render_arena = arena_create(sizeof(render_command) * COMMAND_BUFFER_CAPACITY);
	r->commands = arena_push_array(r->render_arena, render_command, COMMAND_BUFFER_CAPACITY);
	r->command_count = 0;

	return r;
}

void render_set_clear_color(renderer *r, vector4f32 color) {
	r->clear_color.r = color.r / 255.0f;
	r->clear_color.g = color.g / 255.0f;
	r->clear_color.b = color.b / 255.0f;
	r->clear_color.a = color.a / 255.0f;
}

void render_begin(renderer *r) {
	r->command_count = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, r->framebuffer);
	glViewport(0, 0, r->framebuffer_width, r->framebuffer_height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(r->clear_color.r, r->clear_color.g, r->clear_color.b, r->clear_color.a);
	glClear(GL_COLOR_BUFFER_BIT);
}

static void render_push_cmd(renderer *r, render_command_type type, vector3f32 pos, f32 w,
							f32 h, f32 rot) {
	if(r->command_count >= COMMAND_BUFFER_CAPACITY) {
		fprintf(stderr, "Render command buffer full\n");
		exit(-1);
	}
	render_command *cmd = &r->commands[r->command_count++];
	cmd->type = type;
	cmd->pos = pos;
	cmd->w = w;
	cmd->h = h;
	cmd->rot = rot;
}

void render_push_triangle(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot) {
	render_push_cmd(r, RENDER_TRIANGLE, pos, w, h, rot);
}

void render_push_quad(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot) {
	render_push_cmd(r, RENDER_QUAD, pos, w, h, rot);
}

void render_end(renderer *r) {
	for(u32 i = 0; i < r->command_count; i++) {
		render_command *cmd = &r->commands[i];
		render_pipeline *p = &r->pipelines[cmd->type];
		u32 shader_id = p->shader_program;
		glUseProgram(shader_id);

		u32 transform_loc = glGetUniformLocation(shader_id, "transform");
		u32 proj_loc = glGetUniformLocation(shader_id, "projection");

		mat4x4f32 trans = mat4f32_identity();
		trans = mat4f32_translate(trans, cmd->pos);
		trans = mat4f32_rotate(trans, (vector3f32){0.0f, 0.0f, 1.0f}, cmd->rot);
		trans = mat4f32_scale(trans, (vector3f32){cmd->w, cmd->h, 1.0f});

		glUniformMatrix4fv(transform_loc, 1, GL_FALSE, trans.m);
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, r->projection.m);

		glBindVertexArray(p->vertex_array);
		glDrawElements(GL_TRIANGLES, p->vertex_count, GL_UNSIGNED_INT, 0);
	}

	platform_swap_buffers(r->win);
}
