#include "notifications.h"
#include "circular_array.h"
#include "error.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NOTIFICATION_DURATION 2000.0

typedef struct {
	char *text;
	uint64_t tick;
} notification_t;

static void
notification_free(notification_t *notification);

static notification_t *notifications = NULL;

int
notifications_init(uint64_t capacity)
{
	if (notifications != NULL) {
		ERROR("Notifications are already initialized");
		return 1;
	}
	circular_array_init(notifications, capacity);
	return 0;
}

int
notifications_add(const char *text, uint64_t tick)
{
	char *buffer;
	size_t length;
	if (notifications == NULL) {
		ERROR("Notifications are not initialized");
		return 1;
	}
	length = strlen(text);
	buffer = malloc((length + 1) * sizeof(char));
	if (buffer == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		return 1;
	}
	memcpy(buffer, text, length + 1);
	circular_array_push(notifications,
			    ((notification_t){.text = buffer, .tick = tick}),
			    notification_free);
	return 0;
}

void
notifications_render(SDL_Renderer *renderer, TTF_Font *font, float x, float y,
		     SDL_Color color, uint64_t tick)
{
	size_t i, j;
	char *text;
	int offset_y = 0;
	notification_t n;
	uint64_t elapsed;
	SDL_Color _color = color;
	circular_array_header_t *header = circular_array_header(notifications);
	for (i = 0; i < header->count; i++) {
		n = circular_array_get(notifications, i);
		text = n.text;
		if (text == NULL) {
			continue;
		}
		elapsed = tick - n.tick;
		if (elapsed >= NOTIFICATION_DURATION) {
			header->head = (header->head + 1) % header->capacity;
			continue;
		} else {
			_color.a =
			    (uint8_t)(color.a *
				      (1.0f -
				       (float)elapsed / NOTIFICATION_DURATION));
		}
		SDL_Surface *surface =
		    TTF_RenderText_Blended(font, text, strlen(text), _color);
		if (surface == NULL) {
			ERRORF("Failed to render text: %s\n", SDL_GetError());
			continue;
		}
		SDL_Texture *texture =
		    SDL_CreateTextureFromSurface(renderer, surface);
		if (texture == NULL) {
			ERRORF("Failed to create texture: %s\n",
			       SDL_GetError());
			SDL_DestroySurface(surface);
			continue;
		}
		SDL_DestroySurface(surface);
		SDL_FRect destination = {.x = x,
					 .y = y + offset_y,
					 .w = texture->w,
					 .h = texture->h};
		SDL_RenderTexture(renderer, texture, NULL, &destination);
		offset_y += texture->h + 5;
		SDL_DestroyTexture(texture);
	}
}

static void
notification_free(notification_t *notification)
{
	free(notification->text);
	notification->text = NULL;
}

void
notifications_free(void)
{
	uint64_t i;
	for (i = 0; i < circular_array_header(notifications)->capacity; i++) {
		free(notifications[i].text);
		notifications[i].text = NULL;
	}
	circular_array_free(notifications);
	notifications = NULL;
}
