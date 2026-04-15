#include "array.h"
#include "beatmap.h"
#include "db.h"
#include "log.h"
#include "notifications.h"
#include "osz.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdint.h>
#include <stdio.h>
#include <zip.h>

#define NOTIFICATIONS_CAPACITY 10

static int
step(void);
static void
render(void);

static SDL_Window *window;
static SDL_Renderer *r;
static TTF_Font *font;
static const SDL_Color notifications_color = {200, 200, 200, 255};
static uint64_t tick;
metadata_t **metadatas;

int
main(int argc, char *argv[])
{
	uint64_t entries, i;
	int j;
	metadata_t *metadata;
	if (db_open() != 0) {
		ERROR("Failed to open database\n");
		return 1;
	}
	if (notifications_init(NOTIFICATIONS_CAPACITY) != 0) {
		ERROR("Unable to initialize notifications\n");
	}
	entries = db_entries();
	char buffer[256];
	snprintf(buffer, 256, "Found %zu entries", entries);
	notifications_add(buffer, 0);
	for (j = 1; j < argc; j++) {
		if (osz_import_path(argv[j]) == 0) {
			notifications_add(argv[j], 0);
		}
	}
	array_init_capacity(metadatas, entries);
	for (i = 0; i < entries; i++) {
		metadata = db_metadata(i);
		if (metadata == NULL) {
			ERRORF("%zu: Could not parse osu beatmap\n", i);
			continue;
		}
		array_append(metadatas, metadata);
	}
	if (TTF_Init() == 0) {
		ERRORF("Unable to initialize SDL3_ttf: %s\n", SDL_GetError());
		return 1;
	}
	font = TTF_OpenFont("font.ttf", 18);
	if (font == NULL) {
		ERRORF("Failed to open font: %s\n", SDL_GetError());
		return 1;
	}
	if (SDL_Init(SDL_INIT_VIDEO) == 0) {
		ERRORF("Unable to initialize SDL3: %s\n", SDL_GetError());
		return 1;
	}
	window = SDL_CreateWindow("ezu", 800, 600, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		ERRORF("Failed to create window: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	r = SDL_CreateRenderer(window, NULL);
	if (r == NULL) {
		ERRORF("Failed to create renderer: %s\n", SDL_GetError());
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}
	SDL_SetRenderDrawColor(r, 0, 0, 0, 255);
	while (step()) {
		SDL_RenderClear(r);
		render();
		SDL_RenderPresent(r);
	}
	SDL_DestroyRenderer(r);
	SDL_DestroyWindow(window);
	SDL_Quit();
	notifications_free();
	db_close();
	for (i = 0; i < array_length(metadatas); i++) {
		metadata_free(metadatas[i]);
	}
	array_free(metadatas);
	return 0;
}

static int
step(void)
{
	SDL_Event event;
	SDL_Delay(1000.0 / 60.0);
	tick = SDL_GetTicks();
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_EVENT_QUIT) {
			return 0;
		}
		if (event.type == SDL_EVENT_DROP_FILE) {
			metadata_t **_metadatas;
			_metadatas = osz_import_path(event.drop.data);
			if (_metadatas != NULL) {
				size_t length, i;
				length = array_length(_metadatas);
				notifications_add(event.drop.data, tick);
				for (i = 0; i < length; i++) {
					array_append(metadatas, _metadatas[i]);
				}
				array_free(_metadatas);
				_metadatas = NULL;
			}
		}
	}
	return 1;
}

static void
render(void)
{
	notifications_render(r, font, 0, 0, notifications_color, tick);
}
