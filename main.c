#include "array.h"
#include "beatmap.h"
#include "db.h"
#include "log.h"
#include "notifications.h"
#include "osz.h"
#include "text.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <zip.h>

#define WINDOW_WIDTH 800.0
#define WINDOW_HEIGHT 600.0
#define NOTIFICATIONS_CAPACITY 10

static int
step(void);
static void
render(void);
static void
render_menu(void);

static SDL_Window *window;
static SDL_Renderer *r;
static TTF_Font *font;
static const SDL_Color background_color = {0, 0, 0, 255};
static const SDL_Color text_color = {200, 200, 200, 255};
static uint64_t tick;
metadata_t **metadatas;
size_t cursor = 0;
double cursor_alpha = 0.0;

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
	window = SDL_CreateWindow("ezu", WINDOW_WIDTH, WINDOW_HEIGHT,
				  SDL_WINDOW_OPENGL);
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
	SDL_SetRenderDrawColor(r, background_color.r, background_color.g,
			       background_color.b, background_color.a);
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
		if (event.type == SDL_EVENT_KEY_DOWN) {
			switch (event.key.key) {
			case SDLK_UP:
				if (cursor > 0) {
					cursor--;
				} else {
					cursor = array_length(metadatas) - 1;
				}
				break;
			case SDLK_DOWN:
				if (cursor + 1 < array_length(metadatas)) {
					cursor++;
				} else {
					cursor = 0;
				}
				break;
			}
		}
	}
	return 1;
}

static void
render(void)
{
	render_menu();
	SDL_SetRenderDrawColor(r, background_color.r, background_color.g,
			       background_color.b, background_color.a);
}

static void
render_menu(void)
{
	size_t i, length;
	char *title;
	length = array_length(metadatas);
	cursor_alpha = cursor_alpha + ((double)cursor - cursor_alpha) * 0.3;
	for (i = 0; i < length; i++) {
		title = text_format("%s - %s [%s]", metadatas[i]->artist,
				    metadatas[i]->title, metadatas[i]->version);
		if (title == NULL) {
			continue;
		}
		SDL_Surface *surface = TTF_RenderText_Blended(
		    font, title, strlen(title),
		    i == cursor ? background_color : text_color);
		if (surface == NULL) {
			ERRORF("Failed to render text: %s\n", SDL_GetError());
			free(title);
			continue;
		}
		SDL_Texture *texture = SDL_CreateTextureFromSurface(r, surface);
		if (texture == NULL) {
			ERRORF("Failed to create texture: %s\n",
			       SDL_GetError());
			SDL_DestroySurface(surface);
			free(title);
			continue;
		}
		SDL_DestroySurface(surface);
		double t = ((double)i - cursor_alpha) * 0.1;
		double c = cos(t);
		if (c < 0) {
			SDL_DestroyTexture(texture);
			free(title);
			continue;
		}
		SDL_FRect destination = {.x = c * 150.0 - 100.0,
					 .y =
					     WINDOW_HEIGHT / 2 + sin(t) * 400.0,
					 .w = texture->w,
					 .h = texture->h};
		if (i == cursor) {
			SDL_SetRenderDrawColor(r, text_color.r, text_color.g,
					       text_color.b, text_color.a);
			SDL_RenderFillRect(r, &destination);
		}
		SDL_RenderTexture(r, texture, NULL, &destination);
		SDL_DestroyTexture(texture);
		free(title);
	}
	notifications_render(r, font, 0, 0, text_color, tick);
}
