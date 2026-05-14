#ifndef DIRECTORY_H
#define DIRECTORY_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <sys/stat.h>

int
mkdirall(const char *path, __mode_t mode);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DIRECTORY_H */
