#include <string.h>

#include "tetromino.h"
#include "utils/rng.h"

tetromino_t tetrominoes[7] = {TETROMINO_I, TETROMINO_O, TETROMINO_T,
                              TETROMINO_J, TETROMINO_L, TETROMINO_S,
                              TETROMINO_Z};

uint8_t enc_tetro_pos(int16_t x, int16_t y) {
  return ((x & 0xf) << 4) | (y & 0xf);
}

point dec_tetro_pos(uint8_t pos) {
  point p = {
      .x = ((pos & 0x70) >> 4) - ((pos & 0x80) >> 4),
      .y = (pos & 0x07) - (pos & 0x08),
  };

  return p;
}

void rotate_tetro(tetromino_t tetro) {
  for (int32_t i = 0; i < 4; i++) {
    point block = dec_tetro_pos(tetro[i]);
    tetro[i] = enc_tetro_pos(block.y, -block.x);
  }
}

void next_tetro(tetromino_t current, tetromino_t next, uint32_t *n_current,
                uint32_t *n_next, pcg32_random_t *rng) {
  *n_current = *n_next;
  *n_next = pcg32_rand(rng) % 7;

  memcpy(current, tetrominoes[*n_current], sizeof(tetromino_t));
  memcpy(next, tetrominoes[*n_next], sizeof(tetromino_t));
}
