
#ifndef CUTIL_H
#define CUTIL_H

#include <stdio.h>
#include <stdlib.h>

#define new(type, num, dst)    \
    do { \
        dst = malloc(sizeof(type) * num); \
        if (!dst) { \
            fprintf(stderr, "fatal: out of memory error\n"); \
            exit(1); \
        } \
    } while (0)

#define new_copy(type, num, dst)	\
	do { \
		dst = realloc((dst), sizeof(type) * num); \
		if (!dst) { \
			fprintf(stderr, "fatal: out of memory error\n"); \
			exit(1); \
		} \
	} while (0)

#endif

