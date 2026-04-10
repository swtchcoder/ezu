#ifndef DB_H
#define DB_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "beatmap.h"
#include "chart.h"
#include <stdint.h>
#include <stdio.h>

int
db_open(void);

int
db_add(beatmap_t *beatmap, note_t *chart);

uint64_t
db_entries(void);

beatmap_t *
db_beatmap(uint64_t i);

note_t *
db_chart(uint64_t i);

void
db_close(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DB_H */
