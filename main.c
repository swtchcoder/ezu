#include "beatmap.h"
#include "chart.h"
#include "osu.h"
#include <stdio.h>

int
main(int argc, char *argv[])
{
	char *path = NULL;
	FILE *f;
	beatmap_t *beatmap;
	chart_t *chart;
	float difficulty;
	if (argc == 1) {
		fprintf(stderr, "You need to pass a file");
		return 1;
	}
	path = argv[1];
	f = fopen(path, "rb");
	if (f == NULL) {
		perror(path);
		return 1;
	}

	beatmap = osu_beatmap(f);
	if (beatmap == NULL) {
		fprintf(stderr, "%s: Could not parse osu beatmap", path);
		fclose(f);
		return 1;
	}
	chart = osu_chart(f);
	if (chart == NULL) {
		fprintf(stderr, "%s: Could not parse chart", path);
		fclose(f);
		return 1;
	}
	difficulty = chart_difficulty(chart);

	printf("%s - %s (%s) [%s]\n", beatmap->artist, beatmap->title,
	       beatmap->creator, beatmap->version);
	printf("* length: %llu notes\n", chart->length);
	printf("* difficulty: %.2f stars\n", difficulty);

	beatmap_free(beatmap);
	chart_free(chart);

	fclose(f);
	return 0;
}