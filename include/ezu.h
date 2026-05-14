#ifndef EZU_H
#define EZU_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ini.h"
#include <stdint.h>
#include <stdio.h>
#include <zip.h>

typedef struct {
	char *music;
	char *artist;
	char *title;
	char *creator;
	char *version;
} ezu_metadata_t;

typedef struct {
	int time;
	int lane;
	int hit;
} ezu_note_t;

typedef struct {
	ezu_metadata_t *metadata;
	ezu_note_t *notes;
} ezu_beatmap_t;

int
ezu_db_open(const char *directory);
int
ezu_db_add(ezu_metadata_t *metadata, ezu_note_t *notes);
uint64_t
ezu_db_entries(void);
ezu_metadata_t *
ezu_db_metadata(uint64_t i);
ezu_note_t *
ezu_db_notes(uint64_t i);
void
ezu_db_close(void);

float
ezu_beatmap_difficulty(ezu_note_t *notes);
void
ezu_beatmap_free(ezu_metadata_t *metadata, ezu_note_t *notes);
void
ezu_metadata_free(ezu_metadata_t *metadata);
void
ezu_notes_free(ezu_note_t *notes);

ezu_metadata_t **
ezu_osz_import_path(const char *path);
ezu_metadata_t **
ezu_osz_import(zip_t *z);
ini_t *
ezu_osz_osu(zip_t *z, size_t size, uint64_t i);

ezu_metadata_t *
ezu_osu_metadata(ini_t *ini);
ezu_note_t *
ezu_osu_notes(ini_t *ini);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* EZU_H */
