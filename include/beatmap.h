#ifndef BEATMAP_H
#define BEATMAP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	char *music;
	char *artist;
	char *title;
	char *creator;
	char *version;
} metadata_t;

typedef struct {
	int time;
	int lane;
	int hit;
} note_t;

typedef struct {
	metadata_t *metadata;
	note_t *notes;
} beatmap_t;

float
beatmap_difficulty(note_t *notes);

void
beatmap_free(metadata_t *metadata, note_t *notes);

void
metadata_free(metadata_t *metadata);

void
notes_free(note_t *notes);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BEATMAP_H */
