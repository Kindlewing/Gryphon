#include "arena.h"

b8 is_power_of_two(uptr x) {
	return (x & (x - 1)) == 0;
}
