#include "osu.h"
#include "beatmap.h"
#include "chart.h"
#include "ini.h"
#include "log.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OSU_CAPACITY 2

beatmap_t *
osu_beatmap(FILE *file)
{
	long section;
	char *value;
	beatmap_t *beatmap;

	beatmap = malloc(sizeof(beatmap_t));
	if (beatmap == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		return NULL;
	}

	section = ini_section(file, "General");
	if (section < 0) {
		return NULL;
	}
	value = ini_value(file, section, "AudioFilename");
	if (value == NULL) {
		return NULL;
	}
	beatmap->music = value;

	value = ini_value(file, section, "Mode");
	if (value == NULL) {
		return NULL;
	}
	if (value[0] != '3') {
		free(value);
		return NULL;
	}
	free(value);

	section = ini_section(file, "Metadata");
	if (section < 0) {
		return NULL;
	}

	value = ini_value(file, section, "Title");
	if (value == NULL) {
		return NULL;
	}
	beatmap->title = value;

	value = ini_value(file, section, "Artist");
	if (value == NULL) {
		return NULL;
	}
	beatmap->artist = value;

	value = ini_value(file, section, "Creator");
	if (value == NULL) {
		return NULL;
	}
	beatmap->creator = value;

	value = ini_value(file, section, "Version");
	if (value == NULL) {
		return NULL;
	}
	beatmap->version = value;
	return beatmap;
}

chart_t *
osu_chart(FILE *file)
{
	long section;
	unsigned long long capacity;
	chart_t *chart;
	note_t *tmp;
	char buffer[256];
	int x, y, time_ms, type;
	float t;
	int lane;
	section = ini_section(file, "HitObjects");
	if (section < 0) {
		return NULL;
	}
	capacity = OSU_CAPACITY;
	chart = malloc(capacity * sizeof(chart_t));
	if (chart == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		return NULL;
	}
	chart->length = 0;
	chart->notes = malloc(capacity * sizeof(note_t));
	if (chart->notes == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		return NULL;
	}
	while (fgets(buffer, sizeof(buffer), file)) {
		if (chart->length == capacity) {
			capacity *= 2;
			tmp = realloc(chart->notes, capacity * sizeof(note_t));
			if (tmp == NULL) {
				ERRORF("Failed to reallocate buffer: %s\n",
				       strerror(errno));
				free(chart->notes);
				free(chart);
				return NULL;
			}
			chart->notes = tmp;
		}
		if (sscanf(buffer, "%d,%d,%d,%d", &x, &y, &time_ms, &type) !=
		    4) {
			ERROR("Failed to scan note data\n");
			continue;
		}
		t = time_ms / 1000.0f;
		lane = x * 4 / 512;
		if (lane < 0) {
			lane = 0;
		} else if (lane > 3) {
			lane = 3;
		}
		chart->notes[chart->length].time = t;
		chart->notes[chart->length].lane = lane;
		chart->length++;
	}
	if (chart->length != capacity) {
		tmp = realloc(chart->notes, chart->length * sizeof(chart_t));
		if (tmp == NULL) {
			ERRORF("Failed to reallocate buffer: %s\n",
			       strerror(errno));
			free(chart->notes);
			return NULL;
		}
		chart->notes = tmp;
	}
	return chart;
}
