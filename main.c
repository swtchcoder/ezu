#include "beatmap.h"
#include "chart.h"
#include "osu.h"
#include "db.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

static int
osz_import(char *path);

int main(int argc, char *argv[])
{
	uint64_t entries, i;
	beatmap_t *beatmap;
	chart_t *chart;
	float difficulty;
	if (db_open() != 0) {
		fprintf(stderr, "Failed to open database\n");
		return 1;
	}
	if (argc != 1) {
		if (osz_import(argv[1]) != 0) {
			db_close();
			return 1;
		}
		return 0;
	}
	entries = db_entries();
	for (i = 0; i < entries; i++) {
		beatmap = db_beatmap(i);
		if (beatmap == NULL) {
			fprintf(stderr, "%lld: Could not parse osu beatmap\n",
				i);
			continue;
		}
		chart = db_chart(i);
		if (chart == NULL) {
			fprintf(stderr, "%lld: Could not parse chart\n",
				i);;
			continue;
		}
		difficulty = chart_difficulty(chart);

		printf("%s - %s (%s) [%s]\n", beatmap->artist, beatmap->title,
		       beatmap->creator, beatmap->version);
		printf("* music: %s\n", beatmap->music);
		printf("* length: %llu notes\n", chart->length);
		printf("* difficulty: %.2f stars\n\n", difficulty);
	}
	db_close();
}

static int
osz_import(char *path)
{
	int err = 0;
	zip_t *zip;
	zip_int64_t entries, i;
	struct zip_stat stat;
	char *c;
	zip_file_t *zf;
	char *buffer;
	FILE *f;
	beatmap_t *beatmap;
	chart_t *chart;
	float difficulty;
	zip = zip_open(path, 0, &err);
	if (zip == NULL) {
		fprintf(stderr, "%s: Failed to open zip file\n", path);
		return 1;
	}
	entries = zip_get_num_entries(zip, 0);
	for (i = 0; i < entries; i++) {
		zip_stat_index(zip, i, 0, &stat);

		c = strrchr(stat.name, '.');
		if (c == NULL) {
			continue;
		}
		if (strcmp(c, ".osu") != 0) {
			continue;
		}

		zf = zip_fopen_index(zip, i, 0);
		if (zf == NULL) {
			fprintf(stderr, "%s: Failed to open file\n", stat.name);
			continue;
		}

		buffer = malloc(stat.size);
		if (buffer == NULL) {
			zip_fclose(zf);
			continue;
		}
		zip_fread(zf, buffer, stat.size);
#ifdef _WIN32
		f = tmpfile();
		if (f == NULL) {
			fprintf(stderr, "Failed to create temporary file\n");
			zip_fclose(zf);
			free(buffer);
			continue;
		}
		fwrite(buffer, 1, stat.size, f);
		rewind(f);
#else
		f = fmemopen(buffer, stat.size, "rb");
#endif /* _WIN32 */
		zip_fclose(zf);

		beatmap = osu_beatmap(f);
		if (beatmap == NULL) {
			fprintf(stderr, "%s: Could not parse osu beatmap\n",
				stat.name);
			fclose(f);
			free(buffer);
			continue;
		}
		chart = osu_chart(f);
		if (chart == NULL) {
			fprintf(stderr, "%s: Could not parse chart\n",
				stat.name);
			fclose(f);
			free(buffer);
			beatmap_free(beatmap);
			free(beatmap);
			continue;
		}
		difficulty = chart_difficulty(chart);

		printf("%s - %s (%s) [%s]\n", beatmap->artist, beatmap->title,
		       beatmap->creator, beatmap->version);
		printf("* music: %s\n", beatmap->music);
		printf("* length: %llu notes\n", chart->length);
		printf("* difficulty: %.2f stars\n\n", difficulty);

		if (db_add(beatmap, chart) != 0) {
			fprintf(stderr, "%s: Could not save beatmap or chart\n",
				stat.name);
			free(buffer);
			beatmap_free(beatmap);
			free(beatmap);
			chart_free(chart);
			continue;
		}

		beatmap_free(beatmap);
		free(beatmap);
		chart_free(chart);

		fclose(f);
	}
	zip_close(zip);
	return 0;
}