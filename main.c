#include "beatmap.h"
#include "chart.h"
#include "db.h"
#include "osz.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_init.h>
#include <stdint.h>
#include <stdio.h>
#include <zip.h>

static int
step(void);
static void
draw(void);

static SDL_Window *w;
static SDL_Renderer *r;

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
	if (SDL_Init(SDL_INIT_VIDEO) == 0) {
		fprintf(stderr, "Unable to initialize SDL3: %s\n",
			SDL_GetError());
		return 1;
	}
	w = SDL_CreateWindow("ezu", 800, 600, SDL_WINDOW_OPENGL);
	if (w == NULL) {
		fprintf(stderr, "Failed to create window: %s\n",
			SDL_GetError());
		SDL_Quit();
		return 1;
	}
	r = SDL_CreateRenderer(w, NULL);
	if (r == NULL) {
		fprintf(stderr, "Failed to create renderer: %s\n",
			SDL_GetError());
		SDL_DestroyWindow(w);
		SDL_Quit();
		return 1;
	}
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	SDL_RenderClear(r);
	SDL_RenderPresent(r);
	while (step()) {
		draw();
	}
	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(w);
	SDL_Quit();
	db_close();
	return 0;
}

static int
step(void)
{
	SDL_Event event;
	SDL_Delay(1000.0 / 60.0);
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT) {
			return 0;
		}
	}
	return 1;
}

static void
draw(void)
{
}
