//
// Created by jw910731 on 2021/6/14.
//

#include "util.h"

#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

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

/*
 * Implementation is referenced from following page
 * https://stackoverflow.com/questions/6127503/shuffle-array-in-c
 * Give a big thank to the provider
 */
void sanjuan_shuffle(void *array, size_t n, size_t size){
    char tmp[size];
    char *arr = array;
    size_t stride = size * sizeof(char);

    if (n > 1) {
        for (size_t i = 0; i < n - 1; ++i) {
            size_t rnd = (size_t) sanjuan_urandom();
            size_t j = i + rnd / (RAND_MAX / (n - i) + 1);

            memcpy(tmp, arr + j * stride, size);
            memcpy(arr + j * stride, arr + i * stride, size);
            memcpy(arr + i * stride, tmp, size);
        }
    }
}