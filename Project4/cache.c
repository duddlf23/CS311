#include "cache.h"
#include "util.h"

/* cache.c : Implement your functions declared in cache.h */


/***************************************************************/
/*                                                             */
/* Procedure: setupCache                  		       */
/*                                                             */
/* Purpose: Allocates memory for your cache                    */
/*                                                             */
/***************************************************************/

void write_back (int i, int j){
	uint32_t address = (Cache[i][j] -> tag) << 4;
	address |= i << 3;
	mem_write_block(address, Cache[i][j] -> value);
}

 unsigned char write_cache(uint32_t address, uint32_t value){
 	uint32_t tag = (address) >> 4;
 	uint32_t i = (address >> 3) & 1;
 	uint32_t k = (address >> 2) & 1;
 	int j;
 	int cnt = 0;
 	for (j = 0; j < 4; j++){
 		if (Cache[i][j] -> valid){
 			if (Cache[i][j] -> tag == tag){
 				cnt = 1;
 				Cache[i][j] -> value[k] = value;
 				Cache[i][j] -> num = CYCLE_COUNT;
 				break;
 			}
 		}
 	}
 	if (cnt == 1) return 1;

 	cnt = -1;

 	for (j = 0; j < 4; j++){
 		if (!Cache[i][j] -> valid){
 			cnt = j;
 			break;
 		}
 	}

 	if (cnt > -1){
 		Cache[i][cnt] -> valid = 1;
 		Cache[i][cnt] -> tag = tag;
 		Cache[i][cnt] -> num = CYCLE_COUNT;
 		mem_read_block((tag << 4) | (i << 3), Cache[i][cnt] -> value);
 		Cache[i][cnt] -> value[k] = value;
 		return 0;
 	}


 	uint64_t minn = CYCLE_COUNT;
 	cnt = -1;

 	for (j = 0; j < 4; j++){
 		if (Cache[i][j] -> num < minn){
 			minn = Cache[i][j] -> num;
 			cnt = j;
 		}
 	}

 	write_back (i, cnt);
 	Cache[i][cnt] -> tag = tag;
 	Cache[i][cnt] -> num = CYCLE_COUNT;
 	mem_read_block((tag << 4) | (i << 3), Cache[i][cnt] -> value);
 	Cache[i][cnt] -> value[k] = value;

 	return 0;
}

unsigned char hit_cache(uint32_t address){

 	uint32_t tag = (address) >> 4;
 	uint32_t i = (address >> 3) & 1;
 	uint32_t k = (address >> 2) & 1;
 	int j;
 	int cnt = 0;
 	for (j = 0; j < 4; j++){
 		if (Cache[i][j] -> valid){
 			if (Cache[i][j] -> tag == tag){
 				cnt = 1;
 				Cache[i][j] -> num = CYCLE_COUNT;
 				break;
 			}
 		}
 	}
 	if (cnt == 1) return 1;

 	cnt = -1;

 	for (j = 0; j < 4; j++){
 		if (!Cache[i][j] -> valid){
 			cnt = j;
 			break;
 		}
 	}

 	if (cnt > -1){
 		Cache[i][cnt] -> valid = 1;
 		Cache[i][cnt] -> tag = tag;
 		Cache[i][cnt] -> num = CYCLE_COUNT;
 		mem_read_block((tag << 4) | (i << 3), Cache[i][cnt] -> value);
 		return 0;
 	}


 	uint64_t minn = CYCLE_COUNT;
 	cnt = -1;

 	for (j = 0; j < 4; j++){
 		if (Cache[i][j] -> num < minn){
 			minn = Cache[i][j] -> num;
 			cnt = j;
 		}
 	}

 	write_back (i, cnt);
 	Cache[i][cnt] -> tag = tag;
 	Cache[i][cnt] -> num = CYCLE_COUNT;
 	mem_read_block((tag << 4) | (i << 3), Cache[i][cnt] -> value);
 	
 	return 0;
}
uint32_t find_cache(uint32_t address){ 	

	uint32_t tag = (address) >> 4;
 	uint32_t i = (address >> 3) & 1;
 	uint32_t k = (address >> 2) & 1;
 	int j;
 	for (j = 0; j < 4; j++){
 		if (Cache[i][j] -> valid){
 			if (Cache[i][j] -> tag == tag){
 				Cache[i][j] -> num = CYCLE_COUNT;
 				return Cache[i][j] -> value[k];
 			}
 		}
 	}

}
void setupCache(int capacity, int num_way, int block_size)
{
/*	code for initializing and setting up your cache	*/
/*	You may add additional code if you need to	*/
	
	int i,j; //counter
	int nset=0; // number of sets
	int _wpb=0; //words per block   
	nset=capacity/(block_size*num_way);
	_wpb = block_size/BYTES_PER_WORD;

	Cache = (cache_state***)malloc(nset*sizeof(cache_state**));
	
	for (i=0;i<nset;i++) {
		Cache[i] = (cache_state** )malloc(num_way*sizeof(cache_state*));
	}
	for (i=0; i<nset; i++){	
		for (j=0; j<num_way; j++){
			Cache[i][j]=(cache_state*)malloc(sizeof(cache_state));
			Cache[i][j] -> value = (uint32_t*)malloc(sizeof(uint32_t)*(_wpb));
			Cache[i][j] -> valid = 0;
		}
	}

}


/***************************************************************/
/*                                                             */
/* Procedure: setCacheMissPenalty                  	       */
/*                                                             */
/* Purpose: Sets how many cycles your pipline will stall       */
/*                                                             */
/***************************************************************/

void setCacheMissPenalty(int penalty_cycles)
{
/*	code for setting up miss penaly			*/
/*	You may add additional code if you need to	*/	
	miss_penalty = penalty_cycles;

}

/* Please declare and implement additional functions for your cache */
