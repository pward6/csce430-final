#include <stdio.h>
#include <stdlib.h>
#include "cache.h"

int main(){
    Cache* cache = create_cache();


    free_cache(cache);
    return 0;
}