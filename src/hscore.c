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

int hscore_open() {
  // uint16_t fpath[sizeof(HSCORE_FILEPATH) * 2];
  // Bfile_StrToName_ncpy(fpath, HSCORE_FILEPATH, sizeof(HSCORE_FILEPATH));

  // int fhnd = Bfile_OpenFile_OS(fpath, READWRITE, 0);
  // if (fhnd < 0) {
  //   // File does not exist, create it
  //   uint32_t size = sizeof(highscore_t) * HSCORE_COUNT * 2;
  //   if (Bfile_CreateEntry_OS(fpath, CREATEMODE_FILE, &size) == 0) {
  //     fhnd = Bfile_OpenFile_OS(fpath, READWRITE, 0);
  //     if (fhnd < 0)
  //       return -1;

  //     // Zero out the file
  //     char buf[sizeof(highscore_t)] = {0};
  //     for (int i = 0; i < HSCORE_COUNT; i++) {
  //       Bfile_SeekFile_OS(fhnd, 8 * i);
  //       Bfile_WriteFile_OS(fhnd, buf, sizeof(highscore_t));
  //     }
  //   } else {
  //     return -1;
  //   }
  // }

  return 0;
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

  // int file = hscore_open();
  // if (file < 0)
  //   return;

  // Bfile_ReadFile_OS(file, hscores, sizeof(highscore_t) * HSCORE_COUNT, 0);
  // Bfile_CloseFile_OS(file);
}

void hscore_save(highscore_t *hscores) {
  FILE *file = fopen(HSCORE_FILEPATH, "w");

  fwrite(hscores, sizeof(highscore_t), HSCORE_COUNT, file);
  fclose(file);

  // int file = hscore_open();
  // if (file < 0)
  //   return;

  // Bfile_SeekFile_OS(file, 0);
  // Bfile_WriteFile_OS(file, hscores, sizeof(highscore_t) * HSCORE_COUNT);
  // Bfile_CloseFile_OS(file);
}
