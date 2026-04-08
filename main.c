#include "beatmap.h"
#include "chart.h"
#include "db.h"
#include "osz.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <zip.h>

int
main(int argc, char *argv[])
{
	uint64_t entries, i;
	beatmap_t *beatmap;
	chart_t *chart;
	float difficulty;
	zip_t *z;
	if (db_open() != 0) {
		fprintf(stderr, "Failed to open database\n");
		return 1;
	}
	if (argc != 1) {
		z = zip_open(argv[1], 0, NULL);
		if (z == NULL) {
			fprintf(stderr, "%s: Failed to open zip file\n",
				argv[1]);
		} else {
			if (osz_import(z) == 0) {
				fprintf(stderr,
					"%s: Failed to import osz file\n",
					argv[1]);
			}
			zip_close(z);
		}
	}
	entries = db_entries();
	for (i = 0; i < entries; i++) {
		beatmap = db_beatmap(i);
		if (beatmap == NULL) {
			fprintf(stderr, "%zu: Could not parse osu beatmap\n",
				i);
			continue;
		}
		chart = db_chart(i);
		if (chart == NULL) {
			fprintf(stderr, "%zu: Could not parse chart\n", i);
			continue;
		}
		difficulty = chart_difficulty(chart);
		printf("%s - %s (%s) [%s]\n", beatmap->artist, beatmap->title,
		       beatmap->creator, beatmap->version);
		printf("* music: %s\n", beatmap->music);
		printf("* length: %zu notes\n", chart->length);
		printf("* difficulty: %.2f stars\n\n", difficulty);
	}
	db_close();
}
