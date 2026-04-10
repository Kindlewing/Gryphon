#include "arena.h"
#include <assert.h>

b8 is_power_of_two(uptr addr) {
	return (addr & (addr - 1)) == 0;
}

uptr align_forward(usize addr, usize align) {
	assert(is_power_of_two(align));
	return (addr + (align - 1)) & ~(align - 1);
}
