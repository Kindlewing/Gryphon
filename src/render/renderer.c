#include "render/renderer.h"
#include "gryphon.h"
#include "render/shader.h"
#include "linalg.h"
#include "platform/platform.h"
#include "stdio.h"
#include "typedefs.h"
#include <glad.h>
#define COMMAND_BUFFER_BYTES MiB(16)
#define COMMAND_BUFFER_CAPACITY (COMMAND_BUFFER_BYTES / sizeof(render_command))

typedef enum {
	RENDER_PIPELINE_TRIANGLE,
	RENDER_PIPELINE_QUAD,
	// Add more here
	RENDER_PIPELINE_COUNT,
} render_command_type;

typedef struct {
	vector3f32 pos;
	render_command_type type;
	f32 w;
	f32 h;
	f32 rot;
} render_command;

typedef struct render_pipeline {
	u32 vertex_buffer;
	u32 vertex_array;
	u32 idx_buffer;
	u32 vertex_count;
	u32 shader_program;
} render_pipeline;

typedef struct render_data {
	string8 vertex_path;
	string8 fragment_path;
	mat4x4f32 projection;
	// other user-passed data at some point
} render_data;

struct renderer {
	arena *render_arena;
	render_command *commands;
	u32 command_count;
	u32 command_capacity;
	render_pipeline pipelines[RENDER_PIPELINE_COUNT];
	render_data data;

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

renderer *renderer_create(arena *a, gryphon_window *win) {
	renderer *r = arena_push_struct(a, renderer);
	r->win = win;
	u32 width = gryphon_window_width(r->win);
	u32 height = gryphon_window_height(r->win);
	mat4x4f32 projection = mat4f32_ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
	render_data data = {.vertex_path = string8_lit("shaders/vertex.glsl"),
						.fragment_path = string8_lit("shaders/fragment.glsl"),
						.projection = projection};

	r->data = data;
	// clang-format off
	vector3f32 triangle_vertices[] = {
		{-0.5f, -0.5f, 0.0f},  // bottom left;
		{ 0.0f,  0.5f, 0.0f}, // top middle,
		{ 0.5f, -0.5f, 0.0f} // bottom right
	};

	u32 triangle_indices[] = {
		0, 1, 2
	};
	// clang-format on

	r->pipelines[RENDER_PIPELINE_TRIANGLE] =
			pipeline_create(a, triangle_vertices, 9, triangle_indices, 3,
							r->data.vertex_path, r->data.fragment_path);
	// clang-format off
	vector3f32 quad_vertices[] = {
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
	r->pipelines[RENDER_PIPELINE_QUAD] =
			pipeline_create(a, quad_vertices, 12, quad_indices, 6, r->data.vertex_path,
							r->data.fragment_path);
	// set the default framebuffer
	render_set_framebuffer(r, 0, width, height);
	r->render_arena = arena_create(COMMAND_BUFFER_BYTES);
	r->commands =
			arena_push_array(r->render_arena, render_command, COMMAND_BUFFER_CAPACITY);
	r->command_count = 0;
	r->command_capacity = COMMAND_BUFFER_BYTES;

	return r;
}

void render_set_framebuffer(renderer *r, u32 framebuffer, u32 w, u32 h) {
	r->framebuffer = framebuffer;
	r->framebuffer_width = w;
	r->framebuffer_height = h;
}

void render_begin(renderer *r) {
	r->command_count = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, r->framebuffer);
	glViewport(0, 0, r->framebuffer_width, r->framebuffer_height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void render_push_cmd(renderer *r, render_command_type type, vector3f32 pos, f32 w,
							f32 h, f32 rot) {
	if(r->command_count >= r->command_capacity) {
		printf("Render command capacity reached: total count: %d, max cap: %d\n",
			   r->command_count, r->command_capacity);
		exit(-1);
	}
	render_command *cmd = &r->commands[r->command_count];
	r->command_count += 1;
	cmd->type = type;
	cmd->pos = pos;
	cmd->w = w;
	cmd->h = h;
	cmd->rot = rot;
}

void render_push_triangle(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot) {
	render_push_cmd(r, RENDER_PIPELINE_TRIANGLE, pos, w, h, rot);
}

void render_push_quad(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot) {
	render_push_cmd(r, RENDER_PIPELINE_QUAD, pos, w, h, rot);
}

void render_clear(vector4f32 color) {
	glClearColor(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
	glClear(GL_COLOR_BUFFER_BIT);
}

void render_end(renderer *r) {
	u32 count = r->command_count;

	for(usize i = 0; i < count; i += 1) {
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
		glUniformMatrix4fv(proj_loc, 1, GL_FALSE, r->data.projection.m);

		glBindVertexArray(p->vertex_array);
		glDrawElements(GL_TRIANGLES, p->vertex_count, GL_UNSIGNED_INT, 0);
	}

	platform_swap_buffers(r->win);
}
