#include "grid.h"
#include "tetromino.h"
#include <string.h>

static void draw_block(rect r, uint32_t color, color_t *colors) {
  color_t primary = colors[color * 3];
  color_t highlight = colors[color * 3 + 1];
  color_t shadow = colors[color * 3 + 2];

  // Draw a nice block
  disp_fill_rect(r.x0, r.y0, r.x1, r.y1, primary);
  disp_fill_rect(r.x0, r.y0, r.x0, r.y1, highlight);
  disp_fill_rect(r.x0 + 1, r.y0, r.x1, r.y0, highlight);
  disp_fill_rect(r.x1, r.y0, r.x1, r.y1, shadow);
  disp_fill_rect(r.x0, r.y1, r.x1 - 1, r.y1, shadow);
}

static inline uint32_t grid_value(uint32_t *grid, uint32_t x, uint32_t y) {
  return (grid[y] >> (3 * (GRID_WIDTH - 1 - x))) & 7;
}

void draw_grid(uint32_t *grid, color_t *colors) {
  for (uint32_t y = 0; y < GRID_HEIGHT; y++) {
    for (uint32_t x = 0; x < GRID_WIDTH; x++) {
      // 3 bits per block
      uint32_t block_color = grid_value(grid, x, y);

      if (block_color > 0) {
        rect r = {
            .x0 = GRID_START_X + x * GRID_BLOCK_SIZE,
            .y0 = GRID_START_Y + y * GRID_BLOCK_SIZE,
            .x1 = r.x0 + GRID_BLOCK_SIZE - 1,
            .y1 = r.y0 + GRID_BLOCK_SIZE - 1,
        };

        draw_block(r, block_color - 1, colors);
      }
    }
  }
}

void draw_tetro(int16_t x, int16_t y, tetromino_t tetro, uint32_t color,
                color_t *colors) {
  for (int32_t i = 0; i < 4; i++) {
    point block = dec_tetro_pos(tetro[i]);
    rect r = {
        .x0 = x + block.x * GRID_BLOCK_SIZE,
        .y0 = y + block.y * GRID_BLOCK_SIZE,
        .x1 = r.x0 + GRID_BLOCK_SIZE - 1,
        .y1 = r.y0 + GRID_BLOCK_SIZE - 1,
    };

    draw_block(r, color, colors);
  }
}

int try_rotate(uint32_t *grid, tetromino_t tetro, point gridpos) {
  tetromino_t rotated;
  memcpy(rotated, tetro, sizeof(tetromino_t));

  rotate_tetro(rotated);
  int can_rotate = !colcheck_x(grid, rotated, gridpos, 0);

  if (can_rotate)
    rotate_tetro(tetro);

  return can_rotate;
}

int colcheck_x(uint32_t *grid, tetromino_t tetro, point gridpos, int dx) {
  for (int i = 0; i < 4; i++) {
    point local_gridpos = dec_tetro_pos(tetro[i]);
    local_gridpos.x += gridpos.x + dx;
    local_gridpos.y += gridpos.y;

    if (local_gridpos.x < 0 || local_gridpos.x >= GRID_WIDTH ||
        grid_value(grid, local_gridpos.x, local_gridpos.y))
      return 1;
  }
  return 0;
}

int colcheck_down(uint32_t *grid, tetromino_t tetro, point gridpos) {
  for (int i = 0; i < 4; i++) {
    point local_gridpos = dec_tetro_pos(tetro[i]);
    local_gridpos.x += gridpos.x;
    local_gridpos.y += gridpos.y + 1;

    if (local_gridpos.y < 0 || local_gridpos.y >= GRID_HEIGHT ||
        grid_value(grid, local_gridpos.x, local_gridpos.y))
      return 1;
  }
  return 0;
}

void tetro_to_grid(uint32_t *grid, tetromino_t tetro, uint32_t color,
                   point gridpos) {
  for (int i = 0; i < 4; i++) {
    point local_gridpos = dec_tetro_pos(tetro[i]);
    local_gridpos.x += gridpos.x;
    local_gridpos.y += gridpos.y;

    grid[local_gridpos.y] |= ((color + 1) & 7)
                             << ((GRID_WIDTH - 1 - local_gridpos.x) * 3);
  }
}

static inline int check_line(uint32_t line) {
  for (int i = 0; i < GRID_WIDTH; i++) {
    if (!(line & (0x7 << (i * 3))))
      return 0;
  }

  return 1;
}

int check_lines(uint32_t *grid) {
  int j = GRID_HEIGHT - 1;
  for (int i = j; i >= 0; i--) {
    if (!check_line(grid[i])) {
      grid[j--] = grid[i];
    }
  }

  for (int i = j; i >= 0; i--)
    grid[i] = 0;

  return j > 0 ? j : 0;
}
