#ifndef BEATMAP_H
#define BEATMAP_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	char *music;
	char *artist;
	char *title;
	char *creator;
	char *version;
} beatmap_t;

void
beatmap_free(beatmap_t *beatmap);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* BEATMAP_H */