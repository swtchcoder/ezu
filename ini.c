#include "ini.h"
#include "log.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INI_SEPARATOR ':'
#define INI_STRING_LENGTH 128

ini_t *
ini_init(char *origin)
{
	ini_t *ini;
	ini = malloc(sizeof(ini_t));
	if (ini == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		return NULL;
	}
	ini->origin = origin;
	ini->cursor = origin;
	ini->section = origin;
	return ini;
}

int
ini_section(ini_t *ini, const char *section)
{
	unsigned long long length;
	char *end;
	length = strlen(section);
	if (ini_section_ex(ini, section, length, NULL) == 0) {
		return 0;
	}
	end = ini->cursor;
	ini->cursor = ini->origin;
	if (ini_section_ex(ini, section, length, end) == 0) {
		return 0;
	}
	ERRORF("%s: Section not found\n", section);
	return 1;
}

int
ini_section_ex(ini_t *ini, const char *section, const size_t length, char *end)
{
	while (ini->cursor != end && *ini->cursor != '\0') {
		if (*ini->cursor != '[') {
			ini->cursor++;
			continue;
		}
		if (ini->cursor[length + 1] != ']') {
			ini->cursor++;
			continue;
		}
		ini->cursor++;
		if (strncmp(section, ini->cursor, length) != 0) {
			ini->cursor++;
			continue;
		}
		ini->cursor += length + 1;
		ini->section = ini->cursor;
		return 0;
	}
	return 1;
}

char *
ini_value(ini_t *ini, const char *key)
{
	size_t length;
	char *end;
	char *r;
	length = strlen(key);
	end = ini->cursor;
	r = ini_value_ex(ini, key, length, NULL);
	if (r == NULL) {
		ini->cursor = ini->section;
		r = ini_value_ex(ini, key, length, end);
		if (r == NULL) {
			ERRORF("%s: Key not found\n", key);
			return NULL;
		}
	}
	return r;
}

char *
ini_value_ex(ini_t *ini, const char *key, const size_t length, char *end)
{
	int ok = 0;
	long i, capacity;
	char *value, *tmp;
	i = 0;
	while (ini->cursor != end && *ini->cursor != '\0') {
		if (*ini->cursor != key[0]) {
			ini->cursor++;
			continue;
		}
		if (strncmp(key, ini->cursor, length) != 0) {
			ini->cursor++;
			continue;
		}
		ini->cursor++;
		while (*ini->cursor != INI_SEPARATOR) {
			ini->cursor++;
		}
		ok = 1;
		break;
	}
	if (ok == 0) {
		return NULL;
	}
	do {
		ini->cursor++;
	} while (*ini->cursor == ' ');
	capacity = INI_STRING_LENGTH;
	value = malloc(capacity);
	if (value == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		return NULL;
	}
	i = 0;
	while (1) {
		if (*ini->cursor == '\r' || *ini->cursor == '\n' ||
		    *ini->cursor == '\0') {
			value[i] = '\0';
			return value;
		}
		if (i + 1 >= capacity) {
			capacity *= 2;
			tmp = realloc(value, capacity);
			if (tmp == NULL) {
				ERRORF("Failed to reallocate buffer: %s\n",
				       strerror(errno));
				break;
			}
			value = tmp;
		}
		value[i++] = *ini->cursor;
		ini->cursor++;
	}
	free(value);
	return NULL;
}
