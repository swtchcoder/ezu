#ifndef DB_H
#define DB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "beatmap.h"
#include <stdint.h>
#include <stdio.h>

int
db_open(void);

int
db_add(metadata_t *metadata, note_t *notes);

uint64_t
db_entries(void);

metadata_t *
db_metadata(uint64_t i);

note_t *
db_notes(uint64_t i);

void
db_close(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DB_H */
