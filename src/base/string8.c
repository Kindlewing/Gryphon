#include "base/string8.h"
#include "base/arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

string8 string8_lit(const char *str) {
	string8 s;

	s.data = (u8 *)str;
	s.length = 0;
	while(str[s.length] != 0) {
		s.length++;
	}
	return s;
}

string8 string8_make(arena *a, const char *cstring) {
	string8 s;

	s.length = 0;
	while(cstring[s.length]) {
		s.length++;
	}

	s.data = arena_push_array(a, u8, s.length);
	for(size_t i = 0; i < s.length; i++) {
		s.data[i] = (u8)cstring[i];
	}
	return s;
}

string8 string8_read_file(arena *a, int fd, i64 size) {
	string8 contents = {0};
	contents.data = arena_push_array(a, u8, size);
	ssize_t bytes_read = read(fd, contents.data, size);
	if(bytes_read < 0) {
		perror("Failed to read shader file");
		exit(-1);
	}
	contents.length = bytes_read;
	return contents;
}

b8 string8_eq(string8 *s1, string8 *s2) {
	if(s1->length != s2->length) {
		return false;
	}

	for(u64 i = 0; i < s1->length; i += 1) {
		if(s1->data[i] != s2->data[i]) {
			return false;
		}
	}
	return true;
}
