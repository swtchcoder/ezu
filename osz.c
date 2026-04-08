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
	struct zip_stat zs;
	char *c;
	entries = zip_get_num_entries(z, 0);
	for (i = 0; i < entries; i++) {
		zip_stat_index(z, i, 0, &zs);
		c = strrchr(zs.name, '.');
		if (c == NULL) {
			continue;
		}
		if (strcmp(c, ".osu") != 0) {
			continue;
		}
		f = osz_file(z, zs.size, i);
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
osz_file(zip_t *z, size_t size, uint64_t i)
{
	zip_file_t *zf;
	char *buf;
	FILE *f;
	zf = zip_fopen_index(z, i, 0);
	if (zf == NULL) {
		return NULL;
	}
	buf = malloc(size);
	if (buf == NULL) {
		zip_fclose(zf);
		return NULL;
	}
	zip_fread(zf, buf, size);
	f = tmpfile();
	if (f == NULL) {
		zip_fclose(zf);
		free(buf);
		return NULL;
	}
	fwrite(buf, 1, size, f);
	rewind(f);
	zip_fclose(zf);
	return f;
}
