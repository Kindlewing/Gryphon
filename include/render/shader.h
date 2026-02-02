#ifndef SHADER_H
#define SHADER_H

#include "string8.h"
#include "typedefs.h"

typedef struct {
	u32 id;
	u32 vertex_id;
	u32 fragment_id;
} shader;

void shader_init(arena *a, shader *s, string8 vertex_path, string8 fragment_path);

#endif // SHADER_H
