#include "array.h"
#include "beatmap.h"
#include "db.h"
#include "log.h"
#include "notifications.h"
#include "osz.h"
#include "shapes.h"
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
#define NOTE_FALL_TIME 500.0f
#define LANE_WIDTH 100.0f
#define MISS_ALPHA 255.0f * 0.2f

#define PERFECT_OFFSET 50
#define GREAT_OFFSET 100
#define GOOD_OFFSET 150

typedef enum { MISS = -15, GOOD = 50, GREAT = 100, PERFECT = 300 } hit_t;

static int
step(void);
static void
handle_drop_file(SDL_Event event);
static void
handle_key_down(SDL_Event event);
static void
hit(int lane);
static void
render(void);
static void
render_menu(void);
static void
render_game(void);

static SDL_Window *window;
static SDL_Renderer *r;
static TTF_Font *font;

static const SDL_Color background_color = {0, 0, 0, 255};
static const SDL_Color text_color = {200, 200, 200, 255};

static uint64_t tick;

static metadata_t **metadatas;

static size_t cursor = 0;
static double cursor_alpha = 0.0;

static int ingame = 0;

static note_t *notes;
static uint64_t start;
static size_t notes_cursor;

static hit_t hit_score;
static int score;

static float miss_alpha;

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
		handle_drop_file(event);
		handle_key_down(event);
	}
	return 1;
}

static void
handle_drop_file(SDL_Event event)
{
	if (event.type != SDL_EVENT_DROP_FILE) {
		return;
	}
	metadata_t **_metadatas;
	_metadatas = osz_import_path(event.drop.data);
	if (_metadatas == NULL) {
		return;
	}
	size_t length, i;
	length = array_length(_metadatas);
	notifications_add(event.drop.data, tick);
	for (i = 0; i < length; i++) {
		array_append(metadatas, _metadatas[i]);
	}
	array_free(_metadatas);
	_metadatas = NULL;
}

static void
handle_key_down(SDL_Event event)
{
	if (event.type != SDL_EVENT_KEY_DOWN) {
		return;
	}
	if (ingame == 0) {
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
		case SDLK_RETURN:
			notes = db_notes(cursor);
			if (notes == NULL) {
				ERROR("Failed to load notes.\n");
				return;
			}
			ingame = 1;
			start = tick;
			notes_cursor = 0;
			score = 0;
			miss_alpha = 0;
			break;
		}
		return;
	} else {
		switch (event.key.key) {
		case SDLK_D:
			hit(0);
			break;
		case SDLK_F:
			hit(1);
			break;
		case SDLK_J:
			hit(2);
			break;
		case SDLK_K:
			hit(3);
			break;
		case SDLK_ESCAPE:
			ingame = 0;
			notes_free(notes);
			break;
		}
	}
}

static void
hit(int lane)
{
	note_t *note = NULL;
	int dt;
	size_t length, i;
	length = array_length(notes);
	for (i = notes_cursor; i < length; i++) {
		note = &notes[i];
		dt = note->time - ((int64_t)tick - (int64_t)start);
		if (note->lane == lane && !note->hit && dt < 400) {
			break;
		}
	}
	if (!note) {
		return;
	}
	note->hit = true;
	if (dt < -GOOD_OFFSET || dt > GOOD_OFFSET) {
		hit_score = MISS;
		miss_alpha = MISS_ALPHA;
	} else if (dt < -GREAT_OFFSET || dt > GREAT_OFFSET) {
		hit_score = GOOD;
	} else if (dt < -PERFECT_OFFSET || dt > PERFECT_OFFSET) {
		hit_score = GREAT;
	} else {
		hit_score = PERFECT;
	}
	score += hit_score;
}

static void
render(void)
{
	if (ingame == 0) {
		render_menu();
	} else {
		render_game();
	}
	notifications_render(r, font, 0, 0, text_color, tick);
	SDL_SetRenderDrawColor(r, background_color.r, background_color.g,
			       background_color.b, background_color.a);
}

static void
render_menu(void)
{
	size_t i, length;
	char *text;
	length = array_length(metadatas);
	cursor_alpha = cursor_alpha + ((double)cursor - cursor_alpha) * 0.3;
	for (i = 0; i < length; i++) {
		double t = ((double)i - cursor_alpha) * 0.1;
		double c = cos(t);
		if (c < 0) {
			continue;
		}
		text = text_format("%s - %s [%s]", metadatas[i]->artist,
				   metadatas[i]->title, metadatas[i]->version);
		if (text == NULL) {
			continue;
		}
		SDL_Surface *surface = TTF_RenderText_Blended(
		    font, text, strlen(text),
		    i == cursor ? background_color : text_color);
		free(text);
		if (surface == NULL) {
			ERRORF("Failed to render text: %s\n", SDL_GetError());
			continue;
		}
		SDL_Texture *texture = SDL_CreateTextureFromSurface(r, surface);
		if (texture == NULL) {
			ERRORF("Failed to create texture: %s\n",
			       SDL_GetError());
			SDL_DestroySurface(surface);
			continue;
		}
		SDL_DestroySurface(surface);
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
	}
}

static void
render_game(void)
{
	size_t i;
	float x, y;
	note_t *note;
	int64_t dt;
	float hit_line_y = WINDOW_HEIGHT - LANE_WIDTH / 2;
	char *text = NULL;
	size_t length = array_length(notes);
	text = text_format("%d", score < 0 ? 0 : score);
	text_render(r, text, 0, 0, font, text_color);
	free(text);
	SDL_SetRenderDrawColor(r, text_color.r, text_color.g, text_color.b,
			       text_color.a);
	for (i = notes_cursor; i < length; i++) {
		note = &notes[i];
		dt = note->time - ((int64_t)tick - (int64_t)start);
		if (dt >= 0.0f && dt <= NOTE_FALL_TIME + 100.0f) {
			y = hit_line_y * (1.0f - (float)dt / NOTE_FALL_TIME);
			x = (float)note->lane * LANE_WIDTH + WINDOW_WIDTH / 2 -
			    3.0 * LANE_WIDTH / 2.0;
			shapes_filled_circle(r, x, y, LANE_WIDTH / 2.0);
		} else if (dt < -GREAT_OFFSET) {
			notes_cursor++;
			if (!note->hit) {

				note->hit = 1;
				hit_score = MISS;
				score += hit_score;
				miss_alpha = MISS_ALPHA;
			}
		}
	}
	for (i = 0; i < 4; i++) {
		shapes_outlined_circle(r,
				       i * LANE_WIDTH + WINDOW_WIDTH / 2 -
					   3.0 * LANE_WIDTH / 2.0,
				       hit_line_y, LANE_WIDTH / 2.0);
	}
	if (miss_alpha >= 1.0f) {
		SDL_SetRenderDrawBlendMode(r, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(r, 255, 0, 0, miss_alpha);
		SDL_FRect destination = {
		    .x = 0, .y = 0, .w = WINDOW_WIDTH, .h = WINDOW_HEIGHT};
		SDL_RenderFillRect(r, &destination);
		miss_alpha *= 0.9f;
	} else {
		miss_alpha = 0.0f;
	}
}
