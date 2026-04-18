#include "log.h"
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
text_format(const char *fmt, ...)
{
	va_list ap;
	va_list ap_copy;
	char *buffer;
	int size;
	va_start(ap, fmt);
	va_copy(ap_copy, ap);
	size = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);
	if (size < 0)
		return NULL;
	buffer = malloc((size_t)size + 1);
	if (buffer == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		va_end(ap_copy);
		return NULL;
	}
	vsnprintf(buffer, (size_t)size + 1, fmt, ap_copy);
	va_end(ap_copy);
	return buffer;
}

int
text_render(SDL_Renderer *r, const char *text, float x, float y, TTF_Font *f,
	    SDL_Color color)
{
	SDL_Surface *surface =
	    TTF_RenderText_Blended(f, text, strlen(text), color);
	if (surface == NULL) {
		ERRORF("Failed to render text: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Texture *texture = SDL_CreateTextureFromSurface(r, surface);
	if (texture == NULL) {
		ERRORF("Failed to create texture: %s\n", SDL_GetError());
		SDL_DestroySurface(surface);
		return 1;
	}
	SDL_DestroySurface(surface);
	SDL_FRect destination = {
	    .x = x, .y = y, .w = texture->w, .h = texture->h};
	SDL_RenderTexture(r, texture, NULL, &destination);
	SDL_DestroyTexture(texture);
	return 0;
}
