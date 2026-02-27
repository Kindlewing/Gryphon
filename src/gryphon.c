#include "gryphon.h"
#include "base_time.h"

static f64 start_time;

void gryphon_init(void) {
	start_time = time_now();
}

f64 gryphon_get_time(void) {
	return time_now() - start_time;
}
