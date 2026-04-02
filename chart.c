#include "chart.h"
#include <stdlib.h>

float
chart_difficulty(chart_t *chart)
{
	unsigned long long i;
	float difficulty, difference;
	difficulty = 0.0f;
	for (i = 1; i < chart->length; i++) {
		difference = chart->notes[i].time - chart->notes[i - 1].time;
		if (difference == 0.0f) {
			difference = 1.0f;
		}
		difficulty += 0.001f / difference;
	}
	return difficulty;
}

void
chart_free(chart_t *chart)
{
	free(chart->notes);
	chart->notes = NULL;
	free(chart);
}