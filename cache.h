#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>

enum replacement_schemes {
  LRU,
  FIFO,
  RANDOM
};

typedef enum cache_lookup {
  MISS = 1,
  HIT = 0
} cache_lookup_t;

typedef struct cache_block {
  uint8_t valid;
  uint32_t tag;
  void *data;
  uint32_t time_stamp;
} cache_block_t;

typedef struct cache {
  uint32_t blocks_per_set;
  uint32_t size; // In blocks
  enum replacement_schemes scheme;
  cache_block_t *blocks; // Block array
  uint32_t n_access; // Number of times memory has been accessed
} cache_t;

cache_t *cache_new(uint32_t size, uint32_t blocks_per_set, enum replacement_schemes scheme);
cache_t * init_cache(cache_t *cache, uint32_t size, uint32_t blocks_per_set, enum replacement_schemes scheme);
void free_cache(cache_t *cache);
cache_lookup_t read_block_addr(cache_t *cache, uint32_t block_addr, void **block_data);
cache_lookup_t write_block_addr(cache_t *cache, uint32_t block_addr, void *block_data);


void* load_block(uint32_t block_addr);


#endif
