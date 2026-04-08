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
#include "beatmap.h"
#include "chart.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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
db_add(beatmap_t *beatmap, chart_t *chart)
{
	FILE *f;
	uint64_t i;
	char buffer[DB_STRING_LENGTH];
	snprintf(buffer, DB_STRING_LENGTH, DB_CHART_FMT, entries);
	f = fopen(buffer, "wb");
	if (f == NULL) {
		return 1;
	}
	fwrite(&chart->length, sizeof(uint64_t), 1, f);
	for (i = 0; i < chart->length; i++) {
		fwrite(&(chart->notes[i].time), sizeof(float), 1, f);
		fwrite(&(chart->notes[i].lane), sizeof(char), 1, f);
	}
	fclose(f);

	fseek(global, 0, SEEK_END);

	fwrite(beatmap->music, sizeof(char), DB_STRING_LENGTH, global);
	fwrite(beatmap->artist, sizeof(char), DB_STRING_LENGTH, global);
	fwrite(beatmap->title, sizeof(char), DB_STRING_LENGTH, global);
	fwrite(beatmap->creator, sizeof(char), DB_STRING_LENGTH, global);
	fwrite(beatmap->version, sizeof(char), DB_STRING_LENGTH, global);

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

beatmap_t *
db_beatmap(uint64_t i)
{
	beatmap_t *beatmap;
	if (i >= entries) {
		return NULL;
	}
	beatmap = malloc(sizeof(beatmap_t));
	if (beatmap == NULL) {
		return NULL;
	}
	beatmap->music = malloc(sizeof(char) * DB_STRING_LENGTH);
	beatmap->artist = malloc(sizeof(char) * DB_STRING_LENGTH);
	beatmap->title = malloc(sizeof(char) * DB_STRING_LENGTH);
	beatmap->creator = malloc(sizeof(char) * DB_STRING_LENGTH);
	beatmap->version = malloc(sizeof(char) * DB_STRING_LENGTH);
	if (beatmap->music == NULL || beatmap->artist == NULL ||
	    beatmap->title == NULL || beatmap->creator == NULL ||
	    beatmap->version == NULL) {
		beatmap_free(beatmap);
		free(beatmap);
		return NULL;
	}

	fseek(global,
	      sizeof(uint16_t) + sizeof(uint64_t) +
		  sizeof(char) * DB_STRING_LENGTH * 5 * i,
	      SEEK_SET);

	fread(beatmap->music, sizeof(char), DB_STRING_LENGTH, global);
	fread(beatmap->artist, sizeof(char), DB_STRING_LENGTH, global);
	fread(beatmap->title, sizeof(char), DB_STRING_LENGTH, global);
	fread(beatmap->creator, sizeof(char), DB_STRING_LENGTH, global);
	fread(beatmap->version, sizeof(char), DB_STRING_LENGTH, global);
	return beatmap;
}

chart_t *
db_chart(uint64_t i)
{
	FILE *f;
	char buffer[DB_STRING_LENGTH];
	chart_t *chart;
	uint64_t j;
	if (i >= entries) {
		return NULL;
	}
	snprintf(buffer, DB_STRING_LENGTH, DB_CHART_FMT, i);
	f = fopen(buffer, "rb");
	if (f == NULL) {
		puts(buffer);
		return NULL;
	}
	chart = malloc(sizeof(chart_t));
	if (chart == NULL) {
		fclose(f);
		return NULL;
	}
	fread(&chart->length, sizeof(uint64_t), 1, f);
	chart->notes = malloc(sizeof(note_t) * chart->length);
	if (chart->notes == NULL) {
		free(chart);
		fclose(f);
		return NULL;
	}
	for (j = 0; j < chart->length; j++) {
		fread(&chart->notes[j].time, sizeof(float), 1, f);
		fread(&chart->notes[j].lane, sizeof(char), 1, f);
		chart->notes[j].hit = 0;
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
