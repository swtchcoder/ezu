#include "osz.h"
#include "db.h"
#include "osu.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

int
osz_import(zip_t *z)
{
	zip_int64_t entries, i;
	FILE *f;
	beatmap_t *beatmap;
	chart_t *chart;
	int count = 0;
	entries = zip_get_num_entries(z, 0);
	for (i = 0; i < entries; i++) {
		f = osz_osu(z, i);
		if (f == NULL) {
			continue;
		}
		beatmap = osu_beatmap(f);
		if (beatmap == NULL) {
			fclose(f);
			continue;
		}
		chart = osu_chart(f);
		if (chart == NULL) {
			fclose(f);
			beatmap_free(beatmap);
			free(beatmap);
			continue;
		}
		if (db_add(beatmap, chart) != 0) {
			beatmap_free(beatmap);
			free(beatmap);
			chart_free(chart);
			continue;
		}
		beatmap_free(beatmap);
		free(beatmap);
		chart_free(chart);
		fclose(f);
		count++;
	}
	return count;
}

FILE *
osz_osu(zip_t *z, uint64_t i)
{
	zip_file_t *zf;
	struct zip_stat zs;
	char *c;
	char *buf;
	FILE *f;
	zip_stat_index(z, i, 0, &zs);
	c = strrchr(zs.name, '.');
	if (c == NULL) {
		return NULL;
	}
	if (strcmp(c, ".osu") != 0) {
		return NULL;
	}
	zf = zip_fopen_index(z, i, 0);
	if (zf == NULL) {
		return NULL;
	}
	buf = malloc(zs.size);
	if (buf == NULL) {
		zip_fclose(zf);
		return NULL;
	}
	zip_fread(zf, buf, zs.size);
	f = tmpfile();
	if (f == NULL) {
		zip_fclose(zf);
		free(buf);
		return NULL;
	}
	fwrite(buf, 1, zs.size, f);
	rewind(f);
	zip_fclose(zf);
	return f;
}
