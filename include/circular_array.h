#ifndef CIRCULAR_ARRAY_H
#define CIRCULAR_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "error.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#define circular_array_init(array, _capacity)                                  \
	do {                                                                   \
		circular_array_header_t *header =                              \
		    malloc(sizeof(*(array)) * (_capacity) +                    \
			   sizeof(circular_array_header_t));                   \
		if (header == NULL) {                                          \
			ERRORF("Failed to allocate buffer: %s\n",              \
			       strerror(errno));                               \
			exit(1);                                               \
		}                                                              \
		header->head = 0;                                              \
		header->count = 0;                                             \
		header->capacity = _capacity;                                  \
		(array) = (void *)(header + 1);                                \
	} while (0)

#define circular_array_push(array, item, _free)                                \
	do {                                                                   \
		circular_array_header_t *header =                              \
		    circular_array_header(array);                              \
		if (header->count < header->capacity) {                        \
			(array)[header->count] = (item);                       \
			header->count++;                                       \
		} else {                                                       \
			_free(&(array)[header->head]);                         \
			(array)[header->head] = (item);                        \
			header->head = (header->head + 1) % header->capacity;  \
		}                                                              \
	} while (0)

#define circular_array_get(array, i)                                           \
	(array)[(circular_array_head(array) + (i)) %                           \
		circular_array_length(array)]

#define circular_array_head(array) circular_array_header(array)->head

#define circular_array_length(array) circular_array_header(array)->count

#define circular_array_header(array) ((circular_array_header_t *)(array) - 1)

#define circular_array_free(array) free((circular_array_header_t *)(array) - 1)

typedef struct {
	size_t head;
	size_t count;
	size_t capacity;
} circular_array_header_t;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CIRCULAR_ARRAY_H */
