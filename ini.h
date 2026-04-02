#ifndef INI_H
#define INI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdio.h>

long
ini_section(FILE *file, const char *section);
long
ini_section_ex(FILE *file, char *buffer, const char *section,
	       const unsigned long long length, const long origin,
	       const long count);
char *
ini_value(FILE *file, const long section, const char *key);
char *
ini_value_ex(FILE *file, char *buffer, const char *key,
	     const unsigned long long length, const long count);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INI_H */