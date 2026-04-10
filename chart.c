#include "chart.h"
#include "array.h"

float
chart_difficulty(note_t *chart)
{
	unsigned long long i;
	float difficulty, difference;
	uint64_t length;
	length = array_length(chart);
	difficulty = 0.0f;
	for (i = 1; i < length; i++) {
		difference = chart[i].time - chart[i - 1].time;
		if (difference == 0.0f) {
			difference = 1.0f;
		}
		difficulty += 0.001f / difference;
	}
	return difficulty;
}
