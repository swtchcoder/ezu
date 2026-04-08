#ifndef OSZ_H
#define OSZ_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <stdio.h>
#include <zip.h>

int
osz_import(zip_t *z);
FILE *
osz_osu(zip_t *z, uint64_t i);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSZ_H */
