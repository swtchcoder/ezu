#ifndef CHART_H
#define CHART_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

typedef struct {
	float time;
	char lane;
	int hit;
} note_t;

typedef struct {
	note_t *notes;
	uint64_t length;
} chart_t;

float
chart_difficulty(chart_t *chart);
void
chart_free(chart_t *chart);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CHART_H */