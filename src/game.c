#include "game.h"

#include "utils/dialog.h"

#include <gint/keyboard.h>
#include <gint/rtc.h>
#include <string.h>

#include "utils/font.h"

#define NEXT_X 266
#define NEXT_Y (GRID_START_Y - 8)
#define NEXT_W 72
#define NEXT_H 72
#define SCORE_X 266
#define SCORE_Y (NEXT_Y + NEXT_H + 16)
#define SCORE_W NEXT_W
#define SCORE_H (NEXT_Y + 196 - SCORE_Y - 1)
#define LEVEL_X 142 - 24 - 72
#define LEVEL_Y NEXT_Y
#define LEVEL_W 72
#define LEVEL_H 110

#define NUM_HEADING 18
#define NUM_SPACING 24

const char *pause_opts[3] = {"Resume", "Restart", "Main Menu"};
const char *gameover_opts[2] = {"Play again", "Main Menu"};
const char *hscore_opts[1] = {"OK"};

static uint32_t get_score(uint32_t lines) { return (1 << (lines - 1)) * 10; }

static void game_next_tetro(game_t *game) {
  // Advance game speed after 50 tetrominoes
  game->total_tetros++;
  if (game->total_tetros % 50 == 0 && game->game_speed > 16)
    game->game_speed -= 8;

  next_tetro(
      game->t_current, game->t_next, &game->n_current, &game->n_next, &game->rng
  );
}

void game_init(game_t *game) {
  // Seed the RNG with current time
  rtc_time_t time;
  rtc_get_time(&time);
  pcg32_srand(
      &game->rng, (uint64_t)time.seconds << 32 | time.ticks,
      (uint64_t)time.hours << 32 | time.minutes
  );

  // Initialize game state
  for (int i = 0; i < GRID_HEIGHT; i++)
    game->grid[i] = 0;
  game->t_gridpos = (point){4, 1};
  game->n_next = pcg32_rand(&game->rng) % 7;
  game_next_tetro(game);

  // Game tick counter
  game->game_speed = INITIAL_SPEED;
  game->last_ticks = rtc_ticks();
  game->tick_counter = 0;
  game->total_tetros = 0;
  game->holding_down = 0;

  // Score counters
  game->lines = 0;
  game->score = 0;
}

static void gameover_draw_score(void *score_str) {
  int cur_x = 96, cur_y = 112;
  dtext_opt(
      cur_x, cur_y, C_BLACK, C_NONE, DTEXT_LEFT, DTEXT_BOTTOM, "Your Score:"
  );

  cur_x += 90;
  const font_t *prev_font = dfont(&dosv_ank24);
  dtext_opt(cur_x, cur_y, C_BLACK, C_NONE, DTEXT_LEFT, DTEXT_BOTTOM, score_str);
  dfont(prev_font);
}

static int showgameover(game_t *game) {
  sprintf(game->score_str, "%u", game->score);

  if (hscore_check(game->hscores, game->score)) {
    char player_name[8] = {0};
    dialog_textinput(
        "New highscore!", "Enter your name:", player_name, 7, 80, 40, C_BLUE
    );

    highscore_t newscore = {
        .name = "",
        .score = game->score,
    };
    strcpy(newscore.name, player_name);
    hscore_insert(game->hscores, newscore);

    gint_call_t call_hscore_save =
        GINT_CALL(hscore_save, (void *)game->hscores);
    gint_world_switch(call_hscore_save);
  }

  uint32_t option = dialog(
      "Game Over", gameover_opts, 2, 80, 40, C_RED, gameover_draw_score,
      game->score_str
  );

  return option == 0;
}

static int game_tick(game_t *game) {
  int continue_game = 1;

  if (colcheck_down(game->grid, game->t_current, game->t_gridpos)) {
    tetro_to_grid(
        game->grid, game->t_current, game->n_current, game->t_gridpos
    );
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
  uint32_t option = dialog("Paused", pause_opts, 3, 80, 40, C_BLUE, NULL, NULL);
  switch (option) {
  case 1:
    game_init(game);
    return 1;
  case 2:
    return 0;
  case 0:
  default:
    game->last_ticks = rtc_ticks();
    game->tick_counter = 0;
    game->holding_down = 0;
    return 1;
  }
}

static int handle_input(game_t *game) {
  // Rotate tetromino, unless it's a square block
  if ((keypressed(KEY_UP) || keypressed(KEY_F1)) && game->n_current != 1)
    try_rotate(game->grid, game->t_current, game->t_gridpos);

  // Move tetromino
  if (keypressed(KEY_LEFT) &&
      !colcheck_x(game->grid, game->t_current, game->t_gridpos, -1))
    game->t_gridpos.x--;
  if (keypressed(KEY_RIGHT) &&
      !colcheck_x(game->grid, game->t_current, game->t_gridpos, 1))
    game->t_gridpos.x++;

  // Pause
  if (keypressed(KEY_EXIT))
    return showpausemenu(game);

  return 1;
}

int game_update(game_t *game) {
  int continue_game = 1;

  // Update game timer
  int32_t current_ticks = rtc_ticks();
  if (current_ticks < game->last_ticks) // Handle RTC tick reset at midnight
    game->last_ticks -= TICKS_PER_DAY;

  game->tick_counter += current_ticks - game->last_ticks;
  game->last_ticks = current_ticks;

  int tick = game->tick_counter >= game->game_speed;
  if (keypressed(KEY_DOWN) || (game->holding_down && keydown(KEY_DOWN))) {
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
  disp_frame(gx0 - 8, gy0 - 8, gx1 + 8, gy1 + 8, C_BLUE);

  // Draw the grid with all "settled" blocks
  draw_grid(game->grid);

  // Draw the current tetromino in play
  draw_tetro(
      GRID_START_X + game->t_gridpos.x * GRID_BLOCK_SIZE,
      GRID_START_Y + game->t_gridpos.y * GRID_BLOCK_SIZE, game->t_current,
      game->n_current
  );

  int cur_x = 0, cur_y = 0;

  // Draw next tetromino preview
  disp_frame(NEXT_X, NEXT_Y, NEXT_X + NEXT_W, NEXT_Y + NEXT_H, C_BLUE);
  draw_tetro(NEXT_X + 26, NEXT_Y + 26, game->t_next, game->n_next);

  cur_x = NEXT_X + (NEXT_W >> 1) - (cur_x >> 1);
  cur_y = NEXT_Y + NEXT_H - 20;
  dtext_opt(cur_x, cur_y, C_BLACK, C_NONE, DTEXT_CENTER, DTEXT_TOP, "Next");

  // Draw lines and score
  sprintf(game->lines_str, "%u", game->lines);
  sprintf(game->score_str, "%u", game->score);

  disp_frame(SCORE_X, SCORE_Y, SCORE_X + SCORE_W, SCORE_Y + SCORE_H, C_BLUE);

  cur_x = SCORE_X + 16;
  cur_y = SCORE_Y + 16;
  dtext(cur_x, cur_y, C_BLACK, "Score");
  cur_y += NUM_HEADING;
  font_t *prev_font = dfont(&nix_m16);
  dprint(cur_x, cur_y, C_BLACK, "%u", game->score);
  dfont(prev_font);
  cur_y += NUM_SPACING;
  dtext(cur_x, cur_y, C_BLACK, "High");
  cur_y += NUM_HEADING;
  prev_font = dfont(&nix_m16);
  dprint(cur_x, cur_y, C_BLACK, "%u", game->hscores[0].score);
  dfont(prev_font);

  // Draw level
  disp_frame(LEVEL_X, LEVEL_Y, LEVEL_X + LEVEL_W, LEVEL_Y + LEVEL_H, C_BLUE);

  cur_x = LEVEL_X + 16;
  cur_y = LEVEL_Y + 16;
  dtext(cur_x, cur_y, C_BLACK, "Level");
  cur_y += NUM_HEADING;
  prev_font = dfont(&nix_m16);
  dprint(
      cur_x, cur_y, C_BLACK, "%u", (INITIAL_SPEED + 8 - game->game_speed) >> 3
  );
  dfont(prev_font);
  cur_y += NUM_SPACING;
  dtext(cur_x, cur_y, C_BLACK, "Lines");
  cur_y += NUM_HEADING;
  prev_font = dfont(&nix_m16);
  dprint(cur_x, cur_y, C_BLACK, "%u", game->lines);
  dfont(prev_font);

  short end_x = LEVEL_X + 8 + ((LEVEL_W - 16) * (game->total_tetros % 50)) / 50;
  drect(
      LEVEL_X + 8, LEVEL_Y + LEVEL_H - 9, end_x, LEVEL_Y + LEVEL_H - 8, C_RED
  );
}
