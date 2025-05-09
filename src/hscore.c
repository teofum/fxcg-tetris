#include "hscore.h"

#include <stdio.h>
#include <string.h>

#define HSCORE_FILEPATH "thsc.txt"

int hscore_check(highscore_t *hscores, uint32_t score) {
  return score > hscores[HSCORE_COUNT - 1].score;
}

void hscore_insert(highscore_t *hscores, highscore_t newscore) {
  for (int i = 0; i < HSCORE_COUNT; i++) {
    if (newscore.score > hscores[i].score) {
      for (int j = HSCORE_COUNT - 1; j > i; j--)
        hscores[j] = hscores[j - 1];

      hscores[i] = newscore;
      break;
    }
  }
}

void hscore_load(highscore_t *hscores) {
  FILE *file = fopen(HSCORE_FILEPATH, "r");
  if (file == NULL) {
    // If the file doesn't exist, create it and fill with zero
    file = fopen(HSCORE_FILEPATH, "w");

    highscore_t buf[HSCORE_COUNT] = {0};
    fwrite(buf, sizeof(highscore_t), HSCORE_COUNT, file);
    fclose(file);

    file = fopen(HSCORE_FILEPATH, "r");
  }

  fread(hscores, sizeof(highscore_t), HSCORE_COUNT, file);
  fclose(file);
}

void hscore_save(highscore_t *hscores) {
  FILE *file = fopen(HSCORE_FILEPATH, "w");

  fwrite(hscores, sizeof(highscore_t), HSCORE_COUNT, file);
  fclose(file);
}
