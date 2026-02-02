#ifndef PROFILE_H
#define PROFILE_H
#include "string8.h"
#include "typedefs.h"

#define PROFILE_MAX_ANCHORS 4096

#define begin_time_function begin_time_block(__func__)

#define end_time_function destroy_profile_block(&block)

#define begin_time_block(name)                                                           \
	profile_block block;                                                                 \
	do {                                                                                 \
		init_profile_block(&block, string8_lit(name));                                   \
	} while(0)

#define end_time_block destroy_profile_block(&block)

typedef struct {
	u64 tsc_elapsed;		  // how much time has elapsed for this anchor
	u64 tsc_elapsed_children; // how much time has elapsed for this anchor's children?
	u64 times_hit;			  // how many times have we called this?

	string8 label;
} profile_anchor;

typedef struct {
	u64 start_tsc;
	u32 parent_idx; // the caller's scope
	u32 anchor_idx; // function/scope this lives in
} profile_block;

typedef struct {
	profile_anchor anchors[PROFILE_MAX_ANCHORS]; // a list of scopes
	u64 start_tsc;
	u64 end_tsc;
} profiler;

void begin_profile(void);
void init_profile_block(profile_block *block, string8 name);
void destroy_profile_block(profile_block *block);
void end_profile(void);
#endif
