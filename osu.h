#ifndef OSU_H
#define OSU_H

#include "chart.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "beatmap.h"
#include "chart.h"
#include <stdio.h>

beatmap_t *
osu_beatmap(FILE *file);
note_t *
osu_chart(FILE *file);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSU_H */
