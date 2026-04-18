#ifndef TEXT_H
#define TEXT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

char *
text_format(const char *fmt, ...);
int
text_render(SDL_Renderer *r, const char *text, float x, float y, TTF_Font *f,
	    SDL_Color color);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* TEXT_H */
