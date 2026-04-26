#ifndef ARRAY_H
#define ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "error.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define ARRAY_CAPACITY 2

#define array_init(array) array_init_capacity(array, ARRAY_CAPACITY)

#define array_init_capacity(array, _capacity)                                  \
	do {                                                                   \
		array_header_t *header = malloc(                               \
		    sizeof(*(array)) * (_capacity) + sizeof(array_header_t));  \
		if (header == NULL) {                                          \
			ERRORF("Failed to allocate buffer: %s\n",              \
			       strerror(errno));                               \
			exit(1);                                               \
		}                                                              \
		header->count = 0;                                             \
		header->capacity = _capacity;                                  \
		array = (void *)(header + 1);                                  \
	} while (0)

#define array_append(array, item)                                              \
	do {                                                                   \
		array_header_t *header = array_header(array);                  \
		if (header->count >= header->capacity) {                       \
			header->capacity *= 2;                                 \
			array_header_t *tmp = realloc(                         \
			    header, sizeof(*(array)) * header->capacity +      \
					sizeof(array_header_t));               \
			if (tmp == NULL) {                                     \
				ERRORF("Failed to reallocate buffer: %s\n",    \
				       strerror(errno));                       \
				exit(1);                                       \
			}                                                      \
			header = tmp;                                          \
			array = (void *)(header + 1);                          \
		}                                                              \
		(array)[header->count++] = item;                               \
	} while (0)

#define array_length_set(array, length)                                        \
	((array_header_t *)(array) - 1)->count = length

#define array_length(array) array_header(array)->count

#define array_header(array) ((array_header_t *)(array) - 1)

#define array_free(array) free((array_header_t *)(array) - 1)

typedef struct {
	size_t count;
	size_t capacity;
} array_header_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ARRAY_H */
