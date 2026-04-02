#include "ini.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INI_SEPARATOR ':'
#define INI_CAPACITY 2

long
ini_section(FILE *file, const char *section)
{
	unsigned long long length;
	char *buffer;
	long origin;
	long r = 0;
	length = strlen(section);
	buffer = malloc(length + 2);
	if (buffer == NULL) {
		return -1;
	}
	origin = ftell(file);
	r = ini_section_ex(file, buffer, section, length, origin, -1);
	if (r < 0) {
		fseek(file, 0, SEEK_SET);
		r = ini_section_ex(file, buffer, section, length, 0, origin);
	}
	free(buffer);
	return r;
}

long
ini_section_ex(FILE *file, char *buffer, const char *section,
	       const unsigned long long length, const long origin,
	       const long count)
{
	int c;
	long readen = 0;
	do {
		if (readen == count) {
			break;
		}
		readen++;
		c = fgetc(file);
		if (c != '[') {
			continue;
		}
		fgets(buffer, length + 2, file);
		readen += length + 2;
		if (buffer[length] != ']') {
			continue;
		}
		if (strncmp(section, buffer, length) != 0) {
			continue;
		}
		return origin + readen;
	} while (c != EOF);
	return -1;
}

char *
ini_value(FILE *file, const long section, const char *key)
{
	unsigned long long length;
	char *buffer;
	long origin;
	char *r;
	length = strlen(key);
	buffer = malloc(length + 1);
	if (buffer == NULL) {
		return NULL;
	}
	origin = ftell(file);
	r = ini_value_ex(file, buffer, key, length, -1);
	if (r == NULL) {
		fseek(file, section, SEEK_SET);
		r = ini_value_ex(file, buffer, key, length, origin);
	}
	free(buffer);
	return r;
}

char *
ini_value_ex(FILE *file, char *buffer, const char *key,
	     const unsigned long long length, const long count)
{
	int c;
	int ok = 0;
	long i, capacity;
	char *value, *tmp;
	i = 0;
	do {
		if (i == count) {
			return NULL;
		}
		i++;
		c = fgetc(file);
		if (c == EOF) {
			return NULL;
		}
		if (c != key[0]) {
			continue;
		}
		fgets(buffer, length, file);
		if (strncmp(key + 1, buffer, length - 1) != 0) {
			continue;
		}
		ok = 1;
		do {
			c = fgetc(file);
			if ((c != ' ' && c != INI_SEPARATOR) || c == EOF) {
				ok = 0;
			}
		} while (c != INI_SEPARATOR);
		if (ok == 1) {
			break;
		}
	} while (1);
	do {
		c = fgetc(file);
		if (c == EOF) {
			return NULL;
		}
	} while (c == ' ');
	capacity = INI_CAPACITY;
	value = malloc(capacity);
	if (value == NULL) {
		return NULL;
	}
	i = 0;
	while (1) {
		if (c == '\r' || c == '\n' || c == EOF) {
			value[i] = '\0';
			return value;
		}
		if (i + 1 >= capacity) {
			capacity *= 2;
			tmp = realloc(value, capacity);
			if (tmp == NULL) {
				break;
			}
			value = tmp;
		}
		value[i++] = c;
		c = fgetc(file);
	}
	free(value);
	return NULL;
}
