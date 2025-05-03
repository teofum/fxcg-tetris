#include "game.h"

#define NEXT_X 258 + 18
#define NEXT_Y 18 + 18
#define SCORE_X 258

static uint32_t get_score(uint32_t lines) { return (1 << (lines - 1)) * 10; }

static void game_next_tetro(game_t *game) {
  next_tetro(game->t_current, game->t_next, &game->n_current, &game->n_next,
             &game->rng);
}

void game_init(game_t *game, color_t *colors) {
  game->colors = colors;

  // Seed the RNG with current time
  uint32_t hour, minute, second, milli;
  RTC_GetTime(&hour, &minute, &second, &milli);
  pcg32_srand(&game->rng, (uint64_t)second << 32 | milli,
              (uint64_t)hour << 32 | minute);

  // Initialize game state
  for (int i = 0; i < GRID_HEIGHT; i++)
    game->grid[i] = 0;
  game->t_gridpos = (point){4, 1};
  game->n_next = pcg32_rand(&game->rng) % 7;
  game_next_tetro(game);

  // Game tick counter
  game->game_speed = 64; // RTC ticks to game ticks, 128 ticks = 1 second
  game->last_ticks = RTC_GetTicks();
  game->tick_counter = 0;
  game->holding_down = 0;

  // Score counters
  game->lines = 0;
  game->score = 0;
}

static void game_tick(game_t *game) {
  if (colcheck_down(game->grid, game->t_current, game->t_gridpos)) {
    tetro_to_grid(game->grid, game->t_current, game->n_current,
                  game->t_gridpos);
    uint32_t cleared_lines = check_lines(game->grid);
    if (cleared_lines) {
      game->lines += cleared_lines;
      game->score += get_score(cleared_lines);
    }

    game->t_gridpos = (point){4, 1};
    game_next_tetro(game);

    game->holding_down = 0;

    if (colcheck_x(game->grid, game->t_current, game->t_gridpos, 0)) {
      // Game over
      disp_fill_rect(80, 40, LCD_WIDTH_PX - 80, LCD_HEIGHT_PX - 40, 0xffff);
      int cur_x = 100, cur_y = 60;
      PrintMiniMini(&cur_x, &cur_y, "Game Over", 0x40, 0, 0);
      cur_x = 100;
      cur_y += 16;
      PrintMiniMini(&cur_x, &cur_y, "Press [EXE] to restart", 0x40, 0, 0);

      int key = 0;
      while (key != KEY_CTRL_EXE) {
        GetKey(&key);
      }

      game_init(game, game->colors);
    }
  } else {
    game->t_gridpos.y++;
  }
}

static void handle_input(game_t *game) {
  // Rotate tetromino, unless it's a square block
  if ((kb_keydown(KEY_PRGM_UP) || kb_keydown(KEY_PRGM_F1)) &&
      game->n_current != 1)
    try_rotate(game->grid, game->t_current, game->t_gridpos);

  // Move tetromino
  if (kb_keydown(KEY_PRGM_LEFT) &&
      !colcheck_x(game->grid, game->t_current, game->t_gridpos, -1))
    game->t_gridpos.x--;
  if (kb_keydown(KEY_PRGM_RIGHT) &&
      !colcheck_x(game->grid, game->t_current, game->t_gridpos, 1))
    game->t_gridpos.x++;
}

void game_update(game_t *game) {
  // Update game timer
  int32_t current_ticks = RTC_GetTicks();
  if (current_ticks < game->last_ticks) // Handle RTC tick reset at midnight
    game->last_ticks -= TICKS_PER_DAY;

  game->tick_counter += current_ticks - game->last_ticks;
  game->last_ticks = current_ticks;

  int tick = game->tick_counter >= game->game_speed;
  if (kb_keydown(KEY_PRGM_DOWN) ||
      (game->holding_down && kb_ispressed(KEY_PRGM_DOWN))) {
    game->holding_down = 1;
    tick = 1;
  }

  if (tick) {
    game->tick_counter = game->tick_counter > game->game_speed
                             ? game->tick_counter - game->game_speed
                             : 0;
    game_tick(game);
  }

  handle_input(game);
}

void game_draw(game_t *game) {
  // Draw grid borders
  int16_t gx0 = GRID_START_X - 1,
          gx1 = GRID_START_X + GRID_WIDTH * GRID_BLOCK_SIZE,
          gy0 = GRID_START_Y - 1,
          gy1 = GRID_START_Y + GRID_HEIGHT * GRID_BLOCK_SIZE;
  disp_fill_rect(gx0, gy0, gx1, gy0, 0);
  disp_fill_rect(gx0, gy1, gx1, gy1, 0);
  disp_fill_rect(gx0, gy0 + 1, gx0, gy1 - 1, 0);
  disp_fill_rect(gx1, gy0 + 1, gx1, gy1 - 1, 0);

  // Draw the grid with all "settled" blocks
  draw_grid(game->grid, game->colors);

  // Draw the current tetromino in play
  draw_tetro(GRID_START_X + game->t_gridpos.x * GRID_BLOCK_SIZE,
             GRID_START_Y + game->t_gridpos.y * GRID_BLOCK_SIZE,
             game->t_current, game->n_current, game->colors);

  // Draw next tetromino preview
  draw_tetro(NEXT_X, NEXT_Y, game->t_next, game->n_next, game->colors);

  // Draw lines and score
  sprintf(game->lines_str, "%u", game->lines);
  sprintf(game->score_str, "%u", game->score);

  int cur_x = SCORE_X;
  int cur_y = 100;
  PrintMiniMini(&cur_x, &cur_y, "Score", 0x40, 0, 0);
  cur_x = SCORE_X;
  cur_y += 24;
  PrintMini(&cur_x, &cur_y, game->score_str, 0x42, -1, 0, 0, 0, 0, 1, 0);
  cur_x = SCORE_X;
  cur_y += 32;
  PrintMiniMini(&cur_x, &cur_y, "Lines", 0x40, 0, 0);
  cur_x = SCORE_X;
  cur_y += 24;
  PrintMini(&cur_x, &cur_y, game->lines_str, 0x42, -1, 0, 0, 0, 0, 1, 0);
}
