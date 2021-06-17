//
// Created by jw910731 on 2021/6/14.
//

#ifndef SANJUAN_UTIL_H
#define SANJUAN_UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * random that uses <code>/dev/urandom</code> as seed if available
 * otherwise fallback to use time(NULL) as seed
 */
int32_t sanjuan_urandom();

/**
 * shuffle given array
 * @param base pointer points to start of array
 * @param nmemb array element count
 * @param size each element size
 */
void sanjuan_shuffle(void *base, size_t nmemb, size_t size);

#endif //SANJUAN_UTIL_H
