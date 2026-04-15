/* global.dat
 *         [version]
 *         [beatmap count]
 *         [beatmap 1]
 *         ...
 *         [beatmap n]
 *
 * chart structure
 *         [chart length]
 *         [chart] */
#include "db.h"
#include "array.h"
#include "beatmap.h"
#include "log.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DB_CHART_FMT "c%zu.dat"
/* DB_STRING_LENGTH must be lower or equal to INI_STRING_LENGTH */
#define DB_STRING_LENGTH 128

static const uint16_t db_version = 1;
static FILE *global = NULL;
static uint64_t entries = 0;

int
db_open(void)
{
	uint16_t version;
	if (global != NULL) {
		return 1;
	}
	global = fopen("global.dat", "rb+");
	if (global == NULL) {
		global = fopen("global.dat", "wb+");
		if (global == NULL) {
			return 1;
		}
		fwrite(&db_version, sizeof(uint16_t), 1, global);
		fwrite(&entries, sizeof(uint64_t), 1, global);
		return 0;
	} else {
		fread(&version, sizeof(uint16_t), 1, global);
		if (version != db_version) {
			fclose(global);
			global = NULL;
			return 1;
		}
		fread(&entries, sizeof(uint64_t), 1, global);
	}
	return 0;
}

int
db_add(metadata_t *metadata, note_t *notes)
{
	FILE *f;
	uint64_t i;
	char buffer[DB_STRING_LENGTH];
	snprintf(buffer, DB_STRING_LENGTH, DB_CHART_FMT, entries);
	f = fopen(buffer, "wb");
	if (f == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		return 1;
	}
	uint64_t length = array_length(notes);
	fwrite(&length, sizeof(uint64_t), 1, f);
	for (i = 0; i < length; i++) {
		fwrite(&(notes[i].time), sizeof(float), 1, f);
		fwrite(&(notes[i].lane), sizeof(char), 1, f);
	}
	fclose(f);

	fseek(global, 0, SEEK_END);

	fwrite(metadata->music, sizeof(char), DB_STRING_LENGTH, global);
	fwrite(metadata->artist, sizeof(char), DB_STRING_LENGTH, global);
	fwrite(metadata->title, sizeof(char), DB_STRING_LENGTH, global);
	fwrite(metadata->creator, sizeof(char), DB_STRING_LENGTH, global);
	fwrite(metadata->version, sizeof(char), DB_STRING_LENGTH, global);

	entries++;

	fseek(global, sizeof(uint16_t), SEEK_SET);
	fwrite(&entries, sizeof(uint64_t), 1, global);

	return 0;
}

uint64_t
db_entries(void)
{
	return entries;
}

metadata_t *
db_metadata(uint64_t i)
{
	metadata_t *metadata;
	if (i >= entries) {
		ERRORF("%zu: Index out of bounds\n", i);
		return NULL;
	}
	metadata = malloc(sizeof(metadata_t));
	if (metadata == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		return NULL;
	}
	metadata->music = malloc(sizeof(char) * DB_STRING_LENGTH);
	metadata->artist = malloc(sizeof(char) * DB_STRING_LENGTH);
	metadata->title = malloc(sizeof(char) * DB_STRING_LENGTH);
	metadata->creator = malloc(sizeof(char) * DB_STRING_LENGTH);
	metadata->version = malloc(sizeof(char) * DB_STRING_LENGTH);
	if (metadata->music == NULL || metadata->artist == NULL ||
	    metadata->title == NULL || metadata->creator == NULL ||
	    metadata->version == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		metadata_free(metadata);
		return NULL;
	}
	fseek(global,
	      sizeof(uint16_t) + sizeof(uint64_t) +
		  sizeof(char) * DB_STRING_LENGTH * 5 * i,
	      SEEK_SET);

	fread(metadata->music, sizeof(char), DB_STRING_LENGTH, global);
	fread(metadata->artist, sizeof(char), DB_STRING_LENGTH, global);
	fread(metadata->title, sizeof(char), DB_STRING_LENGTH, global);
	fread(metadata->creator, sizeof(char), DB_STRING_LENGTH, global);
	fread(metadata->version, sizeof(char), DB_STRING_LENGTH, global);
	return metadata;
}

note_t *
db_notes(uint64_t i)
{
	FILE *f;
	char buffer[DB_STRING_LENGTH];
	note_t *chart = NULL;
	uint64_t j, length;
	if (i >= entries) {
		ERRORF("%zu: Index out of bounds\n", i);
		return NULL;
	}
	snprintf(buffer, DB_STRING_LENGTH, DB_CHART_FMT, i);
	f = fopen(buffer, "rb");
	if (f == NULL) {
		ERRORF("%s: %s", buffer, strerror(errno));
		puts(buffer);
		return NULL;
	}
	fread(&length, sizeof(uint64_t), 1, f);
	array_init_capacity(chart, length);
	if (chart == NULL) {
		ERROR("Failed to initialize array\n");
		return NULL;
	}
	array_length_set(chart, length);
	for (j = 0; j < length; j++) {
		fread(&chart[j].time, sizeof(float), 1, f);
		fread(&chart[j].lane, sizeof(char), 1, f);
		chart[j].hit = 0;
	}
	fclose(f);
	return chart;
}

void
db_close(void)
{
	fclose(global);
	global = NULL;
	entries = 0;
}
