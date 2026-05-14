/**
 * The ezu rhythm game core functions
 */
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

/**
 * Opens the database from a directory
 */
int
ezu_db_open(const char *directory);
/**
 * Adds a beatmap to the database
 */
int
ezu_db_add(ezu_metadata_t *metadata, ezu_note_t *notes);
/**
 * Returns the number of beatmaps in the database
 */
uint64_t
ezu_db_entries(void);
/**
 * Returns the metadata from a beatmap in database
 */
ezu_metadata_t *
ezu_db_metadata(uint64_t i);
/**
 * Returns the notes from a beatmap in database
 */
ezu_note_t *
ezu_db_notes(uint64_t i);
/**
 * Closes the database
 */
void
ezu_db_close(void);

/**
 * Returns the calculated difficulty of a beatmap
 */
float
ezu_beatmap_difficulty(ezu_note_t *notes);
/**
 * Frees a beatmap
 */
void
ezu_beatmap_free(ezu_metadata_t *metadata, ezu_note_t *notes);
/**
 * Frees a beatmap metadata
 */
void
ezu_metadata_free(ezu_metadata_t *metadata);
/**
 * Frees beatmap notes
 */
void
ezu_notes_free(ezu_note_t *notes);

/**
 * Imports an osu beatmapset archive from a path
 */
ezu_metadata_t **
ezu_osz_import_path(const char *path);
/**
 * Imports an osu beatmapset archive from a zip archive pointer
 */
ezu_metadata_t **
ezu_osz_import(zip_t *z);
/**
 * Imports an osu beatmap from a zip archive pointer
 */
ini_t *
ezu_osz_osu(zip_t *z, size_t size, uint64_t i);

/**
 *  Parses the metadata of an osu beatmap from an ini pointer
 */
ezu_metadata_t *
ezu_osu_metadata(ini_t *ini);
/**
 * Parses the notes of an osu beatmap from an ini pointer
 */
ezu_note_t *
ezu_osu_notes(ini_t *ini);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* EZU_H */
