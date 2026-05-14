#ifndef OSU_H
#define OSU_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "beatmap.h"
#include "ini.h"
#include <stdio.h>

metadata_t *
osu_metadata(ini_t *ini);
note_t *
osu_notes(ini_t *ini);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSU_H */
