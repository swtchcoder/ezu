#include "osu.h"
#include "array.h"
#include "beatmap.h"
#include "ini.h"
#include "log.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OSU_CAPACITY 2

metadata_t *
osu_metadata(ini_t *ini)
{
	char *value;
	metadata_t *metadata;

	metadata = malloc(sizeof(metadata_t));
	if (metadata == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		return NULL;
	}

	if (ini_section(ini, "General") != 0) {
		free(metadata);
		return NULL;
	}
	value = ini_value(ini, "AudioFilename");
	if (value == NULL) {
		free(metadata);
		return NULL;
	}
	metadata->music = value;

	value = ini_value(ini, "Mode");
	if (value == NULL) {
		free(metadata);
		return NULL;
	}
	if (value[0] != '3') {
		free(metadata);
		free(value);
		return NULL;
	}
	free(value);

	if (ini_section(ini, "Metadata") != 0) {
		free(metadata);
		return NULL;
	}

	value = ini_value(ini, "Title");
	if (value == NULL) {
		free(metadata);
		return NULL;
	}
	metadata->title = value;

	value = ini_value(ini, "Artist");
	if (value == NULL) {
		free(metadata);
		return NULL;
	}
	metadata->artist = value;

	value = ini_value(ini, "Creator");
	if (value == NULL) {
		free(metadata);
		return NULL;
	}
	metadata->creator = value;

	value = ini_value(ini, "Version");
	if (value == NULL) {
		free(metadata);
		return NULL;
	}
	metadata->version = value;
	return metadata;
}

note_t *
osu_notes(ini_t *ini)
{
	int x, y, time_ms, type;
	float t;
	char lane;
	note_t *notes = NULL;
	if (ini_section(ini, "HitObjects") != 0) {
		return NULL;
	}
	array_init(notes);
	if (notes == NULL) {
		ERROR("Failed to initialize array\n");
		return NULL;
	}
	do {
		if (sscanf(ini->cursor, "%d,%d,%d,%d", &x, &y, &time_ms,
			   &type) != 4) {
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
		array_append(notes, ((note_t){.time = t, .lane = lane}));
		while (*ini->cursor != '\n' && *ini->cursor != '\0') {
			ini->cursor++;
		}
		if (*ini->cursor == '\n') {
			ini->cursor++;
		}
	} while (*ini->cursor != '\0' && *ini->cursor != '[');
	return notes;
}
