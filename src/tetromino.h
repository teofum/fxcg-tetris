#ifndef __TETROMINO_H__
#define __TETROMINO_H__

#include <fxcg/display.h>

#include "utils/gfx.h"
#include "utils/rng.h"
#include "utils/types.h"

typedef uint8_t tetromino_t[4];

#define TETROMINO_I {0xf0, 0x00, 0x10, 0x20}
#define TETROMINO_O {0x00, 0x01, 0x10, 0x11}
#define TETROMINO_T {0xf0, 0x00, 0x10, 0x01}
#define TETROMINO_J {0x0f, 0x00, 0x01, 0xf1}
#define TETROMINO_L {0x0f, 0x00, 0x01, 0x11}
#define TETROMINO_S {0x00, 0x10, 0x01, 0xf1}
#define TETROMINO_Z {0x00, 0xf0, 0x01, 0x11}

extern color_t colors[21];

typedef struct {
  int16_t x, y;
} point;

uint8_t enc_tetro_pos(int16_t x, int16_t y);

point dec_tetro_pos(uint8_t pos);

void rotate_tetro(tetromino_t tetro);

void next_tetro(tetromino_t current, tetromino_t next, uint32_t *n_current,
                uint32_t *n_next, pcg32_random_t *rng);

#endif
