#ifndef __GRID_H__
#define __GRID_H__

#include "utils/gfx.h"
#include "utils/types.h"

#include "tetromino.h"

#define GRID_HEIGHT 18
#define GRID_WIDTH 10
#define GRID_BLOCK_SIZE 10
#define GRID_START_X 142
#define GRID_START_Y 22

typedef struct {
  int16_t x0, y0, x1, y1;
} rect;

void draw_grid(uint32_t *grid);

void draw_tetro(int16_t x, int16_t y, tetromino_t tetro, uint32_t color);

int try_rotate(uint32_t *grid, tetromino_t tetro, point gridpos);

int colcheck_x(uint32_t *grid, tetromino_t tetro, point gridpos, int dx);

int colcheck_down(uint32_t *grid, tetromino_t tetro, point gridpos);

void tetro_to_grid(uint32_t *grid, tetromino_t tetro, uint32_t color,
                   point gridpos);

int check_lines(uint32_t *grid);

#endif
