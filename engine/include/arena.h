#ifndef ARENA_H
#define ARENA_H
#include "typedefs.h"

#define ARENA_DEFAULT_ALIGNMENT 16

typedef struct {
	u8 *data;
	u64 capacity;
	u64 offset;
} arena;

b8 is_power_of_two(usize addr);
uptr align_forward(usize addr, usize align);

#endif // !ARENA_H
