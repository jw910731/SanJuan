//
// Created by jw910731 on 2021/6/14.
//

#include "util.h"

#include <stdio.h>
#include <time.h>
#include <stdbool.h>

static bool rand_setup = false;

int32_t sanjuan_urandom(){
    if(rand_setup) return rand();

    rand_setup = true;
    FILE *f = fopen("/dev/urandom", "rb");
    if(f == NULL){ // fail to open urandom
        srand(time(NULL));
    }
    else{
        unsigned int seed;
        fread(&seed, 1, sizeof(seed), f);
        srand(seed);
        fclose(f);
    }
    return rand();
}
