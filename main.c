#include "beatmap.h"
#include "chart.h"
#include "osu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

int
main(int argc, char *argv[])
{
	char *path = NULL;
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
	if (argc == 1)
	{
		fprintf(stderr, "You need to pass an archive\n");
		return 1;
	}
	path = argv[1];
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
		if (zip == NULL) {
			fprintf(stderr, "%s: Failed to open file\n", stat.name);
			return 1;
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
			return 1;
		}
		fwrite(buffer, 1, stat.size, f);
		rewind(f);
#else
		f = fmemopen(buffer, stat.size, "rb");
#endif /* _WIN32 */

		beatmap = osu_beatmap(f);
		if (beatmap == NULL) {
			fprintf(stderr, "%s: Could not parse osu beatmap\n",
				stat.name);
			fclose(f);
			return 1;
		}
		chart = osu_chart(f);
		if (chart == NULL) {
			fprintf(stderr, "%s: Could not parse chart\n",
				stat.name);
			fclose(f);
			return 1;
		}
		difficulty = chart_difficulty(chart);

		printf("%s - %s (%s) [%s]\n", beatmap->artist, beatmap->title,
		       beatmap->creator, beatmap->version);
		printf("* music: %s stars\n", beatmap->music);
		printf("* length: %llu notes\n", chart->length);
		printf("* difficulty: %.2f stars\n\n", difficulty);

		beatmap_free(beatmap);
		chart_free(chart);

		fclose(f);
		zip_fclose(zf);
	}
	zip_close(zip);
	return 0;
}