#ifndef SHADER_H
#define SHADER_H

#include "string8.h"
#include "typedefs.h"

typedef struct {
	u32 id;
} shader;

b8 shader_init(arena *a, shader *s, string8 vertex_path, string8 fragment_path);

#endif // SHADER_H
