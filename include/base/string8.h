#ifndef STRING8_H
#define STRING8_H
#include "base/arena.h"
#include "base/typedefs.h"

#define STRING8_MAX 1024

typedef struct {
	u8 *data;
	size_t length;
} string8;

string8 string8_lit(const char *str);
string8 string8_make(arena *a, const char *cstring);

string8 string8_read_file(arena *a, int fd, i64 size);
b8 string8_eq(string8 *s1, string8 *s2);

#endif // STRING8_H
