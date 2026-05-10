#include "directory.h"
#include "error.h"
#include "text.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int
mkdirall(const char *path, __mode_t mode)
{
	char *copy = NULL;
	char *ptr = NULL;
	size_t length;
	copy = text_copy(path);
	if (copy == NULL) {
		ERRORF("%s: Failed to create directory\n", path);
		return 1;
	}
	length = strlen(copy);
	if (copy[length - 1] == '/') {
		copy[length - 1] = '\0';
	}
	for (ptr = copy + 1; *ptr; ptr++) {
		if (*ptr == '/') {
			*ptr = '\0';
			if (mkdir(copy, mode) != 0 && errno != EEXIST) {
				PERROR(copy);
				return 1;
			}
			*ptr = '/';
		}
	}
	if (mkdir(copy, mode) != 0 && errno != EEXIST) {
		PERROR(copy);
		return 1;
	}
	free(copy);
	return 0;
}
