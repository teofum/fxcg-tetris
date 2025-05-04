#include "game.h"

#include "utils/dialog.h"

#define NEXT_X 266
#define NEXT_Y (GRID_START_Y - 8)
#define NEXT_W 72
#define NEXT_H 72
#define SCORE_X 266
#define SCORE_Y (NEXT_Y + NEXT_H + 16)
#define SCORE_W NEXT_W
#define SCORE_H (NEXT_Y + 196 - SCORE_Y - 1)

const char *pause_opts[3] = {"Resume", "Restart", "Main Menu"};
const char *gameover_opts[2] = {"Play again", "Main Menu"};

static uint32_t get_score(uint32_t lines) { return (1 << (lines - 1)) * 10; }

static void game_next_tetro(game_t *game) {
  next_tetro(game->t_current, game->t_next, &game->n_current, &game->n_next,
             &game->rng);
}

void game_init(game_t *game) {
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

static void gameover_draw_score(void *score_str) {
  int cur_x = 100, cur_y = 106;
  PrintMiniMini(&cur_x, &cur_y, "Your Score:", 0x42, 0, 0);
  cur_x += 8;
  cur_y -= 34;
  PrintCXY(cur_x, cur_y, score_str, 0x20, -1, 0, 0, 1, 0);
}

static int showgameover(game_t *game) {
  sprintf(game->score_str, "%u", game->score);

  uint32_t option = dialog("Game Over", gameover_opts, 2, COLOR_RED,
                           gameover_draw_score, game->score_str);

  return option == 0;
}

static int game_tick(game_t *game) {
  int continue_game = 1;

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
      continue_game = showgameover(game);
      game_init(game);
    }
  } else {
    game->t_gridpos.y++;
  }

  return continue_game;
}

static int showpausemenu(game_t *game) {
  disp_shade_rect(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, COLOR_WHITE);

  uint32_t option = dialog("Paused", pause_opts, 3, COLOR_BLUE, NULL, NULL);
  switch (option) {
  case 1:
    game_init(game);
    return 1;
  case 2:
    return 0;
  case 0:
  default:
    game->last_ticks = RTC_GetTicks();
    game->tick_counter = 0;
    game->holding_down = 0;
    return 1;
  }
}

static int handle_input(game_t *game) {
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

  // Pause
  if (kb_keydown(KEY_PRGM_EXIT))
    return showpausemenu(game);

  return 1;
}

int game_update(game_t *game) {
  int continue_game = 1;

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
    continue_game &= game_tick(game);
  }

  continue_game &= handle_input(game);
  return continue_game;
}

void game_draw(game_t *game) {
  // Draw grid frame
  int16_t gx0 = GRID_START_X,
          gx1 = GRID_START_X + GRID_WIDTH * GRID_BLOCK_SIZE - 1,
          gy0 = GRID_START_Y,
          gy1 = GRID_START_Y + GRID_HEIGHT * GRID_BLOCK_SIZE - 1;
  disp_frame(gx0 - 8, gy0 - 8, gx1 + 8, gy1 + 8, COLOR_BLUE);

  // Draw the grid with all "settled" blocks
  draw_grid(game->grid);

  // Draw the current tetromino in play
  draw_tetro(GRID_START_X + game->t_gridpos.x * GRID_BLOCK_SIZE,
             GRID_START_Y + game->t_gridpos.y * GRID_BLOCK_SIZE,
             game->t_current, game->n_current);

  int cur_x = 0, cur_y = 0;

  // Measure string
  PrintMiniMini(&cur_x, &cur_y, "Next", 0x42, 0, 1);

  // Draw next tetromino preview
  disp_frame(NEXT_X, NEXT_Y, NEXT_X + NEXT_W, NEXT_Y + NEXT_H, COLOR_BLUE);
  draw_tetro(NEXT_X + 26, NEXT_Y + 26, game->t_next, game->n_next);

  cur_x = NEXT_X + (NEXT_W >> 1) - (cur_x >> 1);
  cur_y = NEXT_Y + NEXT_H - 20;
  PrintMiniMini(&cur_x, &cur_y, "Next", 0x42, 0, 0);

  // Draw lines and score
  sprintf(game->lines_str, "%u", game->lines);
  sprintf(game->score_str, "%u", game->score);

  disp_frame(SCORE_X, SCORE_Y, SCORE_X + SCORE_W, SCORE_Y + SCORE_H,
             COLOR_BLUE);

  cur_x = SCORE_X + 16;
  cur_y = SCORE_Y + 16;
  PrintMiniMini(&cur_x, &cur_y, "Score", 0x42, 0, 0);
  cur_x = SCORE_X + 16;
  cur_y += 16;
  PrintMini(&cur_x, &cur_y, game->score_str, 0x42, -1, 0, 0, 0, 0, 1, 0);
  cur_x = SCORE_X + 16;
  cur_y += 24;
  PrintMiniMini(&cur_x, &cur_y, "Lines", 0x42, 0, 0);
  cur_x = SCORE_X + 16;
  cur_y += 16;
  PrintMini(&cur_x, &cur_y, game->lines_str, 0x42, -1, 0, 0, 0, 0, 1, 0);
}
