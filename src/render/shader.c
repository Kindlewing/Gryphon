#include "render/shader.h"
#include "arena.h"
#include "typedefs.h"
#include "glad.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

static inline void gl_shader_source_string8(GLuint shader, string8 src) {
	const GLchar *s = (const GLchar *)src.data;
	GLint len = (GLint)src.length;
	glShaderSource(shader, 1, &s, &len);
}

b8 shader_init(arena *a, shader *s, string8 vertex_path, string8 fragment_path) {
	scratch_arena scr = scratch_begin(a);

	i32 vertex_fd = open((char *)vertex_path.data, O_RDONLY);
	i32 fragment_fd = open((char *)fragment_path.data, O_RDONLY);

	if(vertex_fd < 0 || fragment_fd < 0) {
		perror("Failed to open shader file");
		scratch_end(&scr);
		return false;
	}

	i64 vertex_size = lseek(vertex_fd, 0, SEEK_END);
	i64 fragment_size = lseek(fragment_fd, 0, SEEK_END);

	if(vertex_size < 0 || fragment_size < 0) {
		perror("Failed to lseek shader file");
		close(vertex_fd);
		close(fragment_fd);
		scratch_end(&scr);
		return false;
	}

	lseek(vertex_fd, 0, SEEK_SET);
	lseek(fragment_fd, 0, SEEK_SET);

	string8 vertex_src = string8_read_file(scr.parent, vertex_fd, vertex_size);
	string8 fragment_src = string8_read_file(scr.parent, fragment_fd, fragment_size);

	s->vertex_id = glCreateShader(GL_VERTEX_SHADER);
	s->fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

	gl_shader_source_string8(s->vertex_id, vertex_src);
	gl_shader_source_string8(s->fragment_id, fragment_src);

	glCompileShader(s->vertex_id);
	glCompileShader(s->fragment_id);
	s->id = glCreateProgram();

	GLint ok;
	glGetShaderiv(s->vertex_id, GL_COMPILE_STATUS, &ok);
	if(!ok) {
		char log[512];
		glGetShaderInfoLog(s->vertex_id, 512, NULL, log);
		printf("Vertex shader compile error:\n%s\n", log);
		scratch_end(&scr);
		return false;
	}

	glGetShaderiv(s->fragment_id, GL_COMPILE_STATUS, &ok);
	if(!ok) {
		char log[512];
		glGetShaderInfoLog(s->fragment_id, 512, NULL, log);
		printf("Fragment shader compile error:\n%s\n", log);
		scratch_end(&scr);
		return false;
	}

	glAttachShader(s->id, s->vertex_id);
	glAttachShader(s->id, s->fragment_id);
	glLinkProgram(s->id);
	glGetProgramiv(s->id, GL_LINK_STATUS, &ok);

	if(!ok) {
		char log[512];
		glGetProgramInfoLog(s->id, 512, NULL, log);
		printf("Shader program link error:\n%s\n", log);
		scratch_end(&scr);
		return false;
	}
	glDeleteShader(s->vertex_id);
	glDeleteShader(s->fragment_id);
	close(vertex_fd);
	close(fragment_fd);
	scratch_end(&scr);
	return true;
}
