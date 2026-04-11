#ifndef OSU_H
#define OSU_H

#include "chart.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "beatmap.h"
#include "chart.h"
#include "ini.h"
#include <stdio.h>

beatmap_t *
osu_beatmap(ini_t *ini);
note_t *
osu_chart(ini_t *ini);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSU_H */
