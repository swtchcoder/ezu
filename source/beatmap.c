#include "beatmap.h"
#include "array.h"
#include <stdlib.h>

float
beatmap_difficulty(note_t *notes)
{
	size_t length, i;
	float difficulty, difference;
	length = array_length(notes);
	difficulty = 0.0f;
	for (i = 1; i < length; i++) {
		difference = notes[i].time - notes[i - 1].time;
		if (difference == 0.0f) {
			difference = 1.0f;
		}
		difficulty += 0.001f / difference;
	}
	return difficulty;
}

void
beatmap_free(metadata_t *metadata, note_t *notes)
{
	metadata_free(metadata);
	notes_free(notes);
}

void
metadata_free(metadata_t *metadata)
{
	free(metadata->music);
	free(metadata->artist);
	free(metadata->title);
	free(metadata->creator);
	free(metadata->version);
	metadata->music = NULL;
	metadata->artist = NULL;
	metadata->title = NULL;
	metadata->creator = NULL;
	metadata->version = NULL;
}

void
notes_free(note_t *notes)
{
	array_free(notes);
	notes = NULL;
}
