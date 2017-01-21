#include "cache.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t process_cache_miss(cache_t * cache, uint32_t set_addr, uint32_t tag, uint32_t block_addr);

///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////
//// Name: cache_new           ////
//// Arg 1: size               ////
//// Arg 2: blocks_per_set     ////
//// Arg 3: replacement_scheme ////
///////////////////////////////////

cache_t *cache_new(uint32_t size, uint32_t blocks_per_set, enum replacement_schemes scheme) {
  cache_t * cache = malloc(sizeof(cache_t));
  if (cache == NULL) return NULL;

  return init_cache(cache, size, blocks_per_set, scheme);
}

///////////////////////////////////
//// Name: init_cache          ////
//// Arg 1: cache              ////
//// Arg 2: size               ////
//// Arg 3: blocks_per_set     ////
//// Arg 4: replacement_scheme ////
///////////////////////////////////

cache_t * init_cache(cache_t *cache, uint32_t size, uint32_t blocks_per_set, enum replacement_schemes scheme) {

  cache->blocks_per_set = blocks_per_set;
  cache->size = size;
  cache->scheme = scheme;
  cache->n_access = 0;

  cache->blocks = malloc(size*sizeof(cache_block_t));
  if (cache->blocks == NULL) {
    // Perform cleanup
    free(cache);
  }

  for (int i = 0; i < size; i++) {
    cache->blocks[i].valid = 0; // Cache is initially empty
    cache->blocks[i].tag = 0;
  }

  return cache;
}

void free_cache(cache_t *cache) {
  if (cache != NULL) {
    if (cache->blocks != NULL) free(cache->blocks);
    free(cache);
  }
}

//////////////////////////////////
//// Name: read_block_addr    ////
//// Arg 1: cache             ////
//// Arg 2: block address     ////
//// Arg 3: block data        ////
//////////////////////////////////

cache_lookup_t read_block_addr(cache_t *cache, uint32_t block_addr, void **block_data) {
  uint32_t num_sets = cache->size / cache->blocks_per_set;
  uint32_t set_addr = block_addr % num_sets;

  uint32_t tag = block_addr / num_sets;


  // Search the cache for a HIT
  for (uint32_t i = set_addr; i < set_addr + cache->blocks_per_set; i++) {
    if ((cache->blocks[i].tag == tag) && (cache->blocks[i].valid == 1)) {
      if (block_data != NULL) *block_data = cache->blocks[i].data;
      
      if (cache->scheme == LRU)
        cache->blocks[i].time_stamp = cache->n_access;

      return HIT;
    }
  }

  // There was a MISS! We will have to read the block from the lower level
  uint32_t block_index = process_cache_miss(cache, set_addr, tag, block_addr);
  if (block_data != NULL) *block_data = cache->blocks[block_index].data;

  // Updata time stamp
  cache->n_access++;
  return MISS;
}

//////////////////////////////////
//// Name: write_block_addr   ////
//// Arg 1: cache             ////
//// Arg 2: block address     ////
//// Arg 3: block data        ////
//////////////////////////////////

cache_lookup_t write_block_addr(cache_t *cache, uint32_t block_addr, void *block_data) {
  uint32_t num_sets = cache->size / cache->blocks_per_set;
  uint32_t set_addr = block_addr % num_sets;

  uint32_t tag = block_addr / num_sets;


  // Search the cache for a HIT
  for (uint32_t i = set_addr; i < set_addr + cache->blocks_per_set; i++) {
    if ((cache->blocks[i].tag == tag) && (cache->blocks[i].valid == 1)) {
      if (block_data != NULL) cache->blocks[i].data = block_data;

      if (cache->scheme == LRU)
        cache->blocks[i].time_stamp = cache->n_access;

      return HIT;
    }
  }

  // There was a MISS! We will have to read the block from the lower level
  uint32_t block_index = process_cache_miss(cache, set_addr, tag, block_addr);
  if (block_data != NULL) cache->blocks[block_index].data = block_data;

  // Updata time stamp
  cache->n_access++;
  return MISS;
}

//////////////////////////////////
//// Name: process_cache_miss ////
//// Arg 1: cache             ////
//// Arg 2: set address       ////
//// Arg 3: tag               ////
//// Arg 4: block address     ////
//////////////////////////////////

uint32_t process_cache_miss(cache_t * cache, uint32_t set_addr, uint32_t tag, uint32_t block_addr) {
  enum replacement_schemes policy = cache->scheme;

  // Search for smallest time stamp (block to replace inside set)
  uint32_t ts_min = 0xFFFFFFFF;
  uint32_t block_index = 0;
  for (uint32_t i = set_addr; i < set_addr + cache->blocks_per_set; i++) {
    if (cache->blocks[i].valid == 0) {
      block_index = i;
      break;
    }
    else if (cache->blocks[i].time_stamp < ts_min) {
      block_index = i;
      ts_min = cache->blocks[i].time_stamp;
    }
  }

  // Replace block
  cache->blocks[block_index].data = load_block(block_addr);
  cache->blocks[block_index].tag = tag;
  cache->blocks[block_index].valid = 1;
  cache->blocks[block_index].time_stamp = cache->n_access;

  return block_index;
}

//////////////////////////////////
//// Name: load_block         ////
//// Arg 1: block address     ////
//////////////////////////////////

__attribute__((__weak__)) void* load_block(uint32_t block_addr) {
  return NULL;
}
