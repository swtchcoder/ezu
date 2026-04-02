#include "beatmap.h"
#include <stdlib.h>

void
beatmap_free(beatmap_t *beatmap)
{
	free(beatmap->music);
	free(beatmap->artist);
	free(beatmap->title);
	free(beatmap->creator);
	free(beatmap->version);
	beatmap->music = NULL;
	beatmap->artist = NULL;
	beatmap->title = NULL;
	beatmap->creator = NULL;
	beatmap->version = NULL;
}