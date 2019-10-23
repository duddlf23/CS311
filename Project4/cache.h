#ifndef _CACHE_H_
#define _CACHE_H_



#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/* cache.h : Declare functions and data necessary for your project*/

typedef struct cac_st {
	unsigned char valid;
	uint32_t tag;
	uint32_t *value;
	uint64_t num; // cycle_count
} cache_state;

int miss_penalty; // number of cycles to stall when a cache miss occurs

cache_state ***Cache; 

void setupCache(int, int, int);
void setCacheMissPenalty(int);
unsigned char write_cache(uint32_t, uint32_t);
unsigned char hit_cache(uint32_t);
uint32_t find_cache(uint32_t);

#endif