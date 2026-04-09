#include <stdio.h>

#define _STR(x) #x
#define STR(x) _STR(x)

#define ERRORF(fmt, ...)                                                       \
	fprintf(stderr,                                                        \
		"\x1b[31m["__FILE__                                            \
		":" STR(__LINE__) "] " fmt "\x1b[0m",                          \
		__VA_ARGS__)

#define ERROR(fmt)                                                             \
	fprintf(stderr, "\x1b[31m["__FILE__                                    \
			":" STR(__LINE__) "] " fmt "\x1b[0m")
