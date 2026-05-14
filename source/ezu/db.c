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
#include "array.h"
#include "directory.h"
#include "error.h"
#include "ezu.h"
#include "path.h"
#include "text.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define DB_CHART_FMT "c%zu.dat"
/* DB_STRING_LENGTH must be lower or equal to INI_STRING_LENGTH */
#define DB_STRING_LENGTH 128

static const uint16_t db_version = 1;
static FILE *global = NULL;
static uint64_t entries = 0;
static char *_directory = NULL;

int
ezu_db_open(const char *directory)
{
	uint16_t version;
	char *filename;
	if (global != NULL) {
		ERROR("Database already opened\n");
		return 1;
	}
	_directory = text_copy(directory);
	mkdirall(_directory, 0755);
	filename = path_join(_directory, "global.dat");
	if (filename == NULL) {
		ERROR("Failed to join global.dat path\n");
		return 1;
	}
	global = fopen(filename, "rb+");
	if (global == NULL) {
		global = fopen(filename, "wb+");
		if (global == NULL) {
			PERROR(filename);
			free(filename);
			filename = NULL;
			return 1;
		}
		fwrite(&db_version, sizeof(uint16_t), 1, global);
		fwrite(&entries, sizeof(uint64_t), 1, global);
		return 0;
	} else {
		fread(&version, sizeof(uint16_t), 1, global);
		if (version != db_version) {
			ERRORF("%s: Wrong DB version\n", filename);
			free(filename);
			filename = NULL;
			fclose(global);
			global = NULL;
			return 1;
		}
		fread(&entries, sizeof(uint64_t), 1, global);
	}
	free(filename);
	filename = NULL;
	return 0;
}

int
ezu_db_add(ezu_metadata_t *metadata, ezu_note_t *notes)
{
	FILE *f;
	uint64_t i;
	char *buffer = NULL;
	char *filename = NULL;
	buffer = text_format(DB_CHART_FMT, entries);
	if (buffer == NULL) {
		ERROR("Failed to get filename");
		return 1;
	}
	filename = path_join(_directory, buffer);
	if (buffer == NULL) {
		ERROR("Failed to join filename");
		free(buffer);
		buffer = NULL;
		return 1;
	}
	f = fopen(filename, "wb");
	if (f == NULL) {
		PERROR("Failed to allocate buffer");
		return 1;
	}
	uint64_t length = array_length(notes);
	fwrite(&length, sizeof(uint64_t), 1, f);
	for (i = 0; i < length; i++) {
		fwrite(&(notes[i].time), sizeof(int), 1, f);
		fwrite(&(notes[i].lane), sizeof(int), 1, f);
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
ezu_db_entries(void)
{
	return entries;
}

ezu_metadata_t *
ezu_db_metadata(uint64_t i)
{
	ezu_metadata_t *metadata;
	if (i >= entries) {
		ERRORF("%zu: Index out of bounds\n", i);
		return NULL;
	}
	metadata = malloc(sizeof(ezu_metadata_t));
	if (metadata == NULL) {
		PERROR("Failed to allocate buffer");
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
		PERROR("Failed to allocate buffer");
		ezu_metadata_free(metadata);
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

ezu_note_t *
ezu_db_notes(uint64_t i)
{
	FILE *f;
	char *buffer = NULL;
	char *filename = NULL;
	ezu_note_t *chart = NULL;
	uint64_t j, length;
	if (i >= entries) {
		ERRORF("%zu: Index out of bounds\n", i);
		return NULL;
	}
	buffer = text_format(DB_CHART_FMT, i);
	if (buffer == NULL) {
		ERROR("Failed to get filename");
		return NULL;
	}
	filename = path_join(_directory, buffer);
	if (buffer == NULL) {
		ERROR("Failed to join filename");
		free(buffer);
		buffer = NULL;
		return NULL;
	}
	f = fopen(filename, "rb");
	if (f == NULL) {
		PERROR(filename);
		free(buffer);
		free(filename);
		buffer = NULL;
		filename = NULL;
		return NULL;
	}
	fread(&length, sizeof(uint64_t), 1, f);
	array_init_capacity(chart, length);
	if (chart == NULL) {
		ERROR("Failed to initialize array\n");
		free(buffer);
		free(filename);
		buffer = NULL;
		filename = NULL;
		return NULL;
	}
	array_length_set(chart, length);
	for (j = 0; j < length; j++) {
		fread(&chart[j].time, sizeof(int), 1, f);
		fread(&chart[j].lane, sizeof(int), 1, f);
		chart[j].hit = 0;
	}
	fclose(f);
	free(buffer);
	free(filename);
	buffer = NULL;
	filename = NULL;
	return chart;
}

void
ezu_db_close(void)
{
	fclose(global);
	global = NULL;
	entries = 0;
	free(_directory);
	_directory = NULL;
}
