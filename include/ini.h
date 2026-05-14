#ifndef INI_H
#define INI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>

#define ini_free(ini) free(ini)

typedef struct {
	char *origin;
	char *cursor;
	char *section;
} ini_t;

ini_t *
ini_init(char *origin);
int
ini_section(ini_t *ini, const char *section);
int
ini_section_ex(ini_t *ini, const char *section, const size_t length, char *end);
char *
ini_value(ini_t *ini, const char *key);
char *
ini_value_ex(ini_t *ini, const char *key, const size_t length, char *end);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INI_H */
