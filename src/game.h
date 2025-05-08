#ifndef __GAME_H__
#define __GAME_H__

#include <stdio.h>

#include "utils/gfx.h"
#include "utils/rng.h"

#include "grid.h"
#include "hscore.h"
#include "tetromino.h"

#define TICKS_PER_DAY 128 * 60 * 60 * 24
#define INITIAL_SPEED 64

typedef struct {
  pcg32_random_t rng;

  uint32_t grid[GRID_HEIGHT];
  point next_pos;
  point t_gridpos;
  tetromino_t t_current;
  tetromino_t t_next;
  uint32_t n_current;
  uint32_t n_next;

  // Game tick counter
  uint32_t game_speed; // RTC ticks to game ticks, 128 ticks = 1 second
  int32_t last_ticks;
  uint32_t tick_counter;
  int holding_down;

  // Score counters
  uint32_t lines;
  uint32_t score;
  char lines_str[20];
  char score_str[20];
  uint32_t total_tetros;

  // Highscores
  highscore_t hscores[HSCORE_COUNT];
} game_t;

void game_init(game_t *game);

int game_update(game_t *game);

void game_draw(game_t *game);

#endif
