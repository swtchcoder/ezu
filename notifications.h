#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdint.h>

int
notifications_init(uint64_t capacity);
int
notifications_add(const char *text);
void
notifications_render(SDL_Renderer *renderer, TTF_Font *font, float x, float y,
		     SDL_Color color);
void
notifications_free(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* NOTIFICATIONS_H */
