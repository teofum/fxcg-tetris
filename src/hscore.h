#ifndef __HSCORE_H__
#define __HSCORE_H__

#include <gint/gint.h>

#define HSCORE_COUNT 5

typedef struct {
  char name[4];
  uint32_t score;
} highscore_t;

int hscore_check(highscore_t *hscores, uint32_t score);

void hscore_insert(highscore_t *hscores, highscore_t newscore);

void hscore_load(highscore_t *hscores);

void hscore_save(highscore_t *hscores);

#endif
