#ifndef OSZ_H
#define OSZ_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "ini.h"
#include <stdint.h>
#include <stdio.h>
#include <zip.h>

int
osz_import_path(const char *path);
int
osz_import(zip_t *z);
ini_t *
osz_osu(zip_t *z, size_t size, uint64_t i);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OSZ_H */
