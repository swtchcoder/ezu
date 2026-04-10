#ifndef CHART_H
#define CHART_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "array.h"
#include <stdint.h>

typedef struct {
	float time;
	char lane;
	int hit;
} note_t;

float
chart_difficulty(note_t *chart);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CHART_H */
