#include "osz.h"
#include "array.h"
#include "beatmap.h"
#include "db.h"
#include "log.h"
#include "osu.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zip.h>

metadata_t **
osz_import_path(const char *path)
{
	zip_t *z;
	metadata_t **metadatas;
	z = zip_open(path, 0, NULL);
	if (z == NULL) {
		ERRORF("%s: Failed to open zip file\n", path);
		return NULL;
	}
	metadatas = osz_import(z);
	if (metadatas == NULL) {
		ERRORF("%s: Failed to import osz file\n", path);
		zip_close(z);
		return NULL;
	}
	zip_close(z);
	return metadatas;
}

metadata_t **
osz_import(zip_t *z)
{
	zip_int64_t entries, i;
	ini_t *ini;
	metadata_t *metadata;
	metadata_t **metadatas = NULL;
	note_t *notes;
	int count = 0;
	struct zip_stat zs;
	char *c;
	array_init(metadatas);
	entries = zip_get_num_entries(z, 0);
	for (i = 0; i < entries; i++) {
		zip_stat_index(z, i, 0, &zs);
		c = strrchr(zs.name, '.');
		if (c == NULL) {
			continue;
		}
		if (strcmp(c, ".osu") != 0) {
			continue;
		}
		ini = osz_osu(z, zs.size, i);
		if (ini == NULL) {
			continue;
		}
		metadata = osu_metadata(ini);
		if (metadata == NULL) {
			ERRORF("%s: Failed to parse metadata\n", zs.name);
			ini_free(ini);
			continue;
		}
		notes = osu_notes(ini);
		if (notes == NULL) {
			ERRORF("%s: Failed to parse chart\n", zs.name);
			ini_free(ini);
			metadata_free(metadata);
			continue;
		}
		if (db_add(metadata, notes) != 0) {
			ERRORF("%s: Failed to add to database\n", zs.name);
			metadata_free(metadata);
			array_free(notes);
			continue;
		}
		array_append(metadatas, metadata);
		array_free(notes);
		ini_free(ini);
		count++;
	}
	return metadatas;
}

ini_t *
osz_osu(zip_t *z, size_t size, uint64_t i)
{
	zip_file_t *zf;
	char *buf;
	ini_t *ini;
	zf = zip_fopen_index(z, i, 0);
	if (zf == NULL) {
		ERRORF("Failed to open archived file: %s\n", strerror(errno));
		return NULL;
	}
	buf = malloc(size + 1);
	if (buf == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		zip_fclose(zf);
		return NULL;
	}
	zip_fread(zf, buf, size);
	buf[size] = '\0';
	zip_fclose(zf);
	ini = ini_init(buf);
	return ini;
}
