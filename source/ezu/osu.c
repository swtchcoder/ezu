#include "array.h"
#include "error.h"
#include "ezu.h"
#include "ini.h"
#include <stdio.h>
#include <stdlib.h>

#define OSU_CAPACITY 2

ezu_metadata_t *
ezu_osu_metadata(ini_t *ini)
{
	char *value;
	ezu_metadata_t *metadata;

	metadata = malloc(sizeof(ezu_metadata_t));
	if (metadata == NULL) {
		PERROR("Failed to allocate buffer");
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

ezu_note_t *
ezu_osu_notes(ini_t *ini)
{
	int x, y, t, type;
	char lane;
	ezu_note_t *notes = NULL;
	if (ini_section(ini, "HitObjects") != 0) {
		return NULL;
	}
	array_init(notes);
	if (notes == NULL) {
		ERROR("Failed to initialize array\n");
		return NULL;
	}
	do {
		if (sscanf(ini->cursor, "%d,%d,%d,%d", &x, &y, &t, &type) !=
		    4) {
			ERROR("Failed to scan note data\n");
			continue;
		}
		lane = x * 4 / 512;
		if (lane < 0) {
			lane = 0;
		} else if (lane > 3) {
			lane = 3;
		}
		array_append(notes, ((ezu_note_t){.time = t, .lane = lane}));
		while (*ini->cursor != '\n' && *ini->cursor != '\0') {
			ini->cursor++;
		}
		if (*ini->cursor == '\n') {
			ini->cursor++;
		}
	} while (*ini->cursor != '\0' && *ini->cursor != '[');
	return notes;
}
