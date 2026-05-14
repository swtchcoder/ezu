#include "path.h"
#include "text.h"
#include <string.h>

#ifdef __WIN32
#define PATH_SEPARATOR '\\'
#define PATH_SEPARATOR_STR "\\"
#else
#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STR "/"
#endif /* __WIN32 */

char *
path_join(const char *first, const char *second)
{
	long first_length;
	if (first == NULL) {
		return text_copy(second);
	}
	if (second == NULL) {
		return text_copy(first);
	}
	if (second[0] == PATH_SEPARATOR) {
		return text_copy(second);
	}
	first_length = strlen(first);
	if (first[first_length - 1] == PATH_SEPARATOR) {
		first_length--;
	}
	if (first_length == 0) {
		return text_copy(second);
	}
	return text_format("%s" PATH_SEPARATOR_STR "%s", first, second);
}
