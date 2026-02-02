#include "profile.h"
#include "typedefs.h"
#include <stdio.h>

static profiler global_prof;
static u32 global_parent_idx;
static u32 global_anchor_count;

#if _WIN32
#include <intrin.h>
#include <windows.h>

static u64 get_os_timer_freq(void) {
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	return freq.QuadPart;
}

static u64 read_os_timer(void) {
	LARGE_INTEGER value;
	QueryPerformanceCounter(&value);
	return value.QuadPart;
}

#else
#include <sys/time.h>
#include <x86intrin.h>

static u64 get_os_timer_freq(void) {
	return 1000000;
}

static u64 read_os_timer(void) {
	struct timeval time_value;
	gettimeofday(&time_value, 0);
	u64 res = get_os_timer_freq() * (u64)time_value.tv_sec + (u64)time_value.tv_usec;
	return res;
}

#endif

u32 profile_get_anchor(string8 label) {
	for(u32 i = 0; i < global_anchor_count; ++i) {
		if(string8_eq(&global_prof.anchors[i].label, &label))
			return i;
	}

	u32 idx = global_anchor_count++;
	global_prof.anchors[idx].label = label;
	return idx;
}

void begin_profile() {
	global_prof.start_tsc = __rdtsc();
}

static u64 estimate_cpu_timer_freq(void) {
	u64 ms_to_wait = 100;
	u64 os_freq = get_os_timer_freq();
	u64 cpu_start = __rdtsc();
	u64 os_start = read_os_timer();
	u64 os_elapsed = 0;
	u64 os_wait = os_freq * ms_to_wait / 1000;
	while(os_elapsed < os_wait) {
		u64 os_now = read_os_timer();
		os_elapsed = os_now - os_start;
	}
	u64 cpu_end = __rdtsc();
	u64 cpu_elapsed = cpu_end - cpu_start;
	u64 cpu_freq = 0;
	if(os_elapsed) {
		cpu_freq = os_freq * cpu_elapsed / os_elapsed;
	}
	return cpu_freq;
}

void init_profile_block(profile_block *block, string8 name) {
	block->start_tsc = __rdtsc();
	u32 idx = profile_get_anchor(name);
	block->anchor_idx = idx;
	block->parent_idx = global_parent_idx;
	global_parent_idx = idx;
	profile_anchor *a = &global_prof.anchors[idx];
	a->label = name;
}

void destroy_profile_block(profile_block *block) {
	u64 elapsed = __rdtsc() - block->start_tsc;

	profile_anchor *a = &global_prof.anchors[block->anchor_idx];

	if(block->parent_idx != block->anchor_idx) { // avoid root adding to self
		profile_anchor *parent = &global_prof.anchors[block->parent_idx];
		parent->tsc_elapsed_children += elapsed;
	}

	global_parent_idx = block->parent_idx;
	a->tsc_elapsed += elapsed;
	a->times_hit += 1;
}

void end_profile() {
	global_prof.end_tsc = __rdtsc();
	u64 cpu_freq = estimate_cpu_timer_freq();
	if(cpu_freq == 0) {
		return;
	}
	u64 total_cycles = global_prof.end_tsc - global_prof.start_tsc;
	f64 total_seconds = (f64)total_cycles / (f64)cpu_freq;
	printf("Total time: %.3f ms\n\n", total_seconds * 1000.0);

	for(u32 i = 0; i < PROFILE_MAX_ANCHORS; ++i) {
		profile_anchor *a = &global_prof.anchors[i];
		u64 self_time = a->tsc_elapsed - a->tsc_elapsed_children;
		if(a->times_hit == 0) {
			continue;
		}

		f64 self_seconds = (f64)self_time / (f64)cpu_freq;
		f64 average = (self_seconds / (f64)a->times_hit) * 1000.0;
		printf("%-30.*s | hits: %6llu | time (self): %8.3f ms | average (self): %8.3f ms",
			   (int)a->label.length, a->label.data, a->times_hit, self_seconds * 1000.0,
			   average);
		if(a->tsc_elapsed_children > 0) {
			f64 seconds_with_children = (f64)a->tsc_elapsed / (f64)cpu_freq;
			printf(" | with children: %8.3f ms\n", seconds_with_children * 1000.0);
		} else {
			printf("\n");
		}
	}
}
