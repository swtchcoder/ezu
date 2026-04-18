#ifndef SHAPES_H
#define SHAPES_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <SDL3/SDL.h>

void
shapes_filled_circle(SDL_Renderer *r, int cx, int cy, int radius);
void
shapes_outlined_circle(SDL_Renderer *r, int cx, int cy, int radius);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* SHAPES_H */
