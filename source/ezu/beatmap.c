#include "array.h"
#include "ezu.h"
#include <stdlib.h>

float
ezu_beatmap_difficulty(ezu_note_t *notes)
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
ezu_beatmap_free(ezu_metadata_t *metadata, ezu_note_t *notes)
{
	ezu_metadata_free(metadata);
	ezu_notes_free(notes);
}

void
ezu_metadata_free(ezu_metadata_t *metadata)
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
ezu_notes_free(ezu_note_t *notes)
{
	array_free(notes);
	notes = NULL;
}
