#ifndef CHART_H
#define CHART_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	float time;
	int lane;
	int hit;
} note_t;

typedef struct {
	note_t *notes;
	unsigned long long length;
} chart_t;

float
chart_difficulty(chart_t *chart);
void
chart_free(chart_t *chart);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CHART_H */