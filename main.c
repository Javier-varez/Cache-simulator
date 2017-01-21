#include <stdio.h>
#include <stdint.h>
#include "cache.h"

int access[] = {0, 3, 5, 7, 7, 1, 2, 8, 3, 0, 4, 2, 1, 4, 5, 3, 8, 5, 2, 5, 3, 5, 2, -1};
int misses[100];

int main(int argc, char * argv[]) {

    cache_t *cache = cache_new(8, 2, LRU);

    uint32_t miss_count = 0;

    for (uint32_t i = 0; 1; i++) {
        if (access[i] < 0) break;
        cache_lookup_t miss = read_block_addr(cache, access[i], NULL);
	misses[i] = miss;
        miss_count += miss;
    }

    printf("Miss count is %d\n", miss_count);

    for (uint32_t i = 0; 1; i++) {
        if (access[i] < 0) break;
        printf("\taccess[%d] = %d\n", i, misses[i]);
    }

    free_cache(cache);
    return 0;
}
