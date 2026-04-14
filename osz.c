#include "osz.h"
#include "db.h"
#include "log.h"
#include "osu.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

int
osz_import_path(const char *path)
{
	zip_t *z;
	z = zip_open(path, 0, NULL);
	if (z == NULL) {
		ERRORF("%s: Failed to open zip file\n", path);
		return 1;
	}
	if (osz_import(z) <= 0) {
		ERRORF("%s: Failed to import osz file\n", path);
		zip_close(z);
		return 1;
	}
	zip_close(z);
	return 0;
}

int
osz_import(zip_t *z)
{
	zip_int64_t entries, i;
	ini_t *ini;
	beatmap_t *beatmap;
	note_t *chart;
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
		ini = osz_osu(z, zs.size, i);
		if (ini == NULL) {
			continue;
		}
		beatmap = osu_beatmap(ini);
		if (beatmap == NULL) {
			ERRORF("%s: Failed to parse beatmap\n", zs.name);
			ini_free(ini);
			continue;
		}
		chart = osu_chart(ini);
		if (chart == NULL) {
			ERRORF("%s: Failed to parse chart\n", zs.name);
			ini_free(ini);
			beatmap_free(beatmap);
			free(beatmap);
			continue;
		}
		if (db_add(beatmap, chart) != 0) {
			ERRORF("%s: Failed to add to database\n", zs.name);
			beatmap_free(beatmap);
			free(beatmap);
			array_free(chart);
			continue;
		}
		beatmap_free(beatmap);
		free(beatmap);
		array_free(chart);
		ini_free(ini);
		count++;
	}
	return count;
}

ini_t *
osz_osu(zip_t *z, size_t size, uint64_t i)
{
	zip_file_t *zf;
	char *buf;
	ini_t *ini;
	zf = zip_fopen_index(z, i, 0);
	if (zf == NULL) {
		ERRORF("Failed to open archived file: %s\n", strerror(errno));
		return NULL;
	}
	buf = malloc(size + 1);
	if (buf == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		zip_fclose(zf);
		return NULL;
	}
	zip_fread(zf, buf, size);
	buf[size] = '\0';
	zip_fclose(zf);
	ini = ini_init(buf);
	return ini;
}
