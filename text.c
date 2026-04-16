#include "log.h"
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *
text_format(const char *fmt, ...)
{
	va_list ap;
	va_list ap_copy;
	char *buffer;
	int size;
	va_start(ap, fmt);
	va_copy(ap_copy, ap);
	size = vsnprintf(NULL, 0, fmt, ap);
	va_end(ap);
	if (size < 0)
		return NULL;
	buffer = malloc((size_t)size + 1);
	if (buffer == NULL) {
		ERRORF("Failed to allocate buffer: %s\n", strerror(errno));
		va_end(ap_copy);
		return NULL;
	}
	vsnprintf(buffer, (size_t)size + 1, fmt, ap_copy);
	va_end(ap_copy);
	return buffer;
}
