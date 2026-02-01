#ifndef ARENA_H
#define ARENA_H

#include <assert.h>
#include <unistd.h>

#include "typedefs.h"
#include <sys/mman.h>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

typedef struct {
    u8 *base;
    u64 capacity;
    u64 offset;
} arena;

arena *arena_create(u64 capacity);
void arena_clear(arena *arena);
void arena_free(arena *arena);

void *arena_push(arena *arena, u64 size);
void *arena_push_aligned(arena *arena, u64 size, size_t align);
void *arena_push_zero(arena *arena, u64 size);

#define arena_push_struct(a, type) \
    ((type *)arena_push_aligned((a), sizeof(type), DEFAULT_ALIGNMENT))

#define arena_push_array(a, type, count) \
    ((type *)arena_push_aligned((a), sizeof(type) * (count), DEFAULT_ALIGNMENT))

#define arena_push_struct_zero(a, type) \
    ((type *)arena_push_zero(a, sizeof(type)))

#endif // ARENA_H
