#include "render/renderer.h"
#include "gryphon.h"
#include "render/shader.h"
#include "linalg.h"
#include <stdio.h>
#include <glad.h>

#define COMMAND_BUFFER_CAPACITY (1 << 16)

typedef enum {
	RENDER_CMD_CLEAR,
	RENDER_CMD_TRIANGLE,
	RENDER_CMD_QUAD,
} render_command_type;

typedef struct {
	render_command_type type;
	union {
		vector4f32 color;
		struct {
			vector3f32 pos;
			f32 w, h, rot;
		} draw;
	};
} render_command;

struct renderer {
	arena *render_arena;
	render_command *commands;
	u32 command_count;

	u32 shader;
	u32 triangle_vao, triangle_count;
	u32 quad_vao, quad_count;

	mat4x4f32 projection;
	u32 framebuffer;
	u32 framebuffer_width;
	u32 framebuffer_height;
	gryphon_window *win;
};

static u32 upload_geometry(vector3f32 *verts, u32 vert_floats, u32 *indices,
						   u32 index_count) {
	u32 vao, vbo, ebo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(f32) * vert_floats, verts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(f32), (void *)0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * index_count, indices,
				 GL_STATIC_DRAW);
	return vao;
}

renderer *renderer_create(arena *a, gryphon_window *win) {
	renderer *r = arena_push_struct(a, renderer);
	r->win = win;
	u32 width = platform_window_width(win);
	u32 height = platform_window_height(win);
	r->projection = mat4f32_ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);

	shader s;
	if(!shader_init(a, &s, string8_lit("shaders/vertex.glsl"),
					string8_lit("shaders/fragment.glsl"))) {
		fprintf(stderr, "Failed to load shaders!\n");
		exit(EXIT_FAILURE);
	}
	r->shader = s.id;

	// clang-format off
	vector3f32 tri_verts[] = {
		{-0.5f, -0.5f, 0.0f},
		{ 0.0f,  0.5f, 0.0f},
		{ 0.5f, -0.5f, 0.0f},
	};
	u32 tri_idx[] = { 0, 1, 2 };

	vector3f32 quad_verts[] = {
		{ 0.5f,  0.5f, 0.0f},
		{ 0.5f, -0.5f, 0.0f},
		{-0.5f, -0.5f, 0.0f},
		{-0.5f,  0.5f, 0.0f},
	};
	u32 quad_idx[] = { 0, 1, 3, 1, 2, 3 };
	// clang-format on
	r->triangle_vao = upload_geometry(tri_verts, 9, tri_idx, 3);
	r->triangle_count = 3;
	r->quad_vao = upload_geometry(quad_verts, 12, quad_idx, 6);
	r->quad_count = 6;

	r->framebuffer = 0;
	r->framebuffer_width = width;
	r->framebuffer_height = height;

	r->render_arena = arena_create(sizeof(render_command) * COMMAND_BUFFER_CAPACITY);
	r->commands =
			arena_push_array(r->render_arena, render_command, COMMAND_BUFFER_CAPACITY);
	r->command_count = 0;
	return r;
}

void render_begin(renderer *r) {
	r->command_count = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, r->framebuffer);
	glViewport(0, 0, r->framebuffer_width, r->framebuffer_height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

static void push_cmd(renderer *r, render_command cmd) {
	if(r->command_count >= COMMAND_BUFFER_CAPACITY) {
		fprintf(stderr, "Render command buffer full\n");
		exit(-1);
	}
	r->commands[r->command_count] = cmd;
	r->command_count += 1;
}

void render_push_clear(renderer *r, vector4f32 color) {
	push_cmd(r, (render_command){
						.type = RENDER_CMD_CLEAR,
						.color = {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f,
								  color.a / 255.0f},
				});
}

void render_push_triangle(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot) {
	push_cmd(r, (render_command){.type = RENDER_CMD_TRIANGLE, .draw = {pos, w, h, rot}});
}

void render_push_quad(renderer *r, vector3f32 pos, f32 w, f32 h, f32 rot) {
	push_cmd(r, (render_command){.type = RENDER_CMD_QUAD, .draw = {pos, w, h, rot}});
}

void render_end(renderer *r) {
	glUseProgram(r->shader);
	u32 transform_loc = glGetUniformLocation(r->shader, "transform");
	u32 proj_loc = glGetUniformLocation(r->shader, "projection");
	glUniformMatrix4fv(proj_loc, 1, GL_FALSE, r->projection.m);

	for(u32 i = 0; i < r->command_count; i++) {
		render_command *cmd = &r->commands[i];
		switch(cmd->type) {
		case RENDER_CMD_CLEAR:
			glClearColor(cmd->color.r, cmd->color.g, cmd->color.b, cmd->color.a);
			glClear(GL_COLOR_BUFFER_BIT);
			break;
		case RENDER_CMD_TRIANGLE:
		case RENDER_CMD_QUAD: {
			mat4x4f32 trans = mat4f32_identity();
			trans = mat4f32_translate(trans, cmd->draw.pos);
			trans = mat4f32_rotate(trans, (vector3f32){0.0f, 0.0f, 1.0f}, cmd->draw.rot);
			trans = mat4f32_scale(trans, (vector3f32){cmd->draw.w, cmd->draw.h, 1.0f});
			glUniformMatrix4fv(transform_loc, 1, GL_FALSE, trans.m);

			u32 vao = cmd->type == RENDER_CMD_TRIANGLE ? r->triangle_vao : r->quad_vao;
			u32 count =
					cmd->type == RENDER_CMD_TRIANGLE ? r->triangle_count : r->quad_count;
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, 0);
			break;
		}
		}
	}

	platform_swap_buffers(r->win);
}
