#include <SDL3/SDL.h>
#include <math.h>

void
shapes_filled_circle(SDL_Renderer *r, int cx, int cy, int radius)
{
	for (int dy = -radius; dy <= radius; dy++) {
		int dx = (int)sqrtf((float)(radius * radius - dy * dy));
		SDL_RenderLine(r, cx - dx, cy + dy, cx + dx, cy + dy);
	}
}

void
shapes_outlined_circle(SDL_Renderer *r, int cx, int cy, int radius)
{
	const int diameter = (radius * 2);
	int x = (radius - 1);
	int y = 0;
	int tx = 1;
	int ty = 1;
	int error = (tx - diameter);
	while (x >= y) {
		SDL_RenderPoint(r, cx + x, cy - y);
		SDL_RenderPoint(r, cx + x, cy + y);
		SDL_RenderPoint(r, cx - x, cy - y);
		SDL_RenderPoint(r, cx - x, cy + y);
		SDL_RenderPoint(r, cx + y, cy - x);
		SDL_RenderPoint(r, cx + y, cy + x);
		SDL_RenderPoint(r, cx - y, cy - x);
		SDL_RenderPoint(r, cx - y, cy + x);
		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}
		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}
