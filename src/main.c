#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/rtc.h>

#include "utils/gfx.h"
#include "utils/input.h"
#include "utils/rng.h"

#include "grid.h"
#include "tetromino.h"

#define TICKS_PER_DAY 128 * 60 * 60 * 24

int main() {
  color_t colors[21] = {
      color(160, 20, 40),  color(200, 80, 120), color(80, 10, 20),

      color(160, 120, 20), color(200, 160, 40), color(80, 60, 10),

      color(160, 160, 40), color(200, 200, 60), color(80, 80, 20),

      color(40, 160, 20),  color(60, 200, 40),  color(20, 80, 10),

      color(20, 160, 160), color(40, 200, 200), color(10, 80, 80),

      color(40, 20, 180),  color(60, 40, 240),  color(20, 10, 100),

      color(160, 20, 160), color(200, 40, 200), color(80, 10, 80),
  };

  pcg32_random_t rng;

  // Seed the RNG with current time
  uint32_t hour, minute, second, milli;
  RTC_GetTime(&hour, &minute, &second, &milli);
  pcg32_srand(&rng, (uint64_t)second << 32 | milli,
              (uint64_t)hour << 32 | minute);

  // Initialize game state
  uint32_t grid[GRID_HEIGHT] = {0};
  point next_pos = {GRID_START_X + GRID_BLOCK_SIZE * 14, GRID_START_Y};
  point t_gridpos = {4, 1};
  tetromino_t t_current;
  tetromino_t t_next;
  uint32_t n_current;
  uint32_t n_next = pcg32_rand(&rng) % 7;
  next_tetro(t_current, t_next, &n_current, &n_next, &rng);

  // Game tick counter
  uint32_t game_speed = 64; // RTC ticks to game ticks, 128 ticks = 1 second
  int32_t last_ticks = RTC_GetTicks();
  uint32_t tick_counter = 0;

  // Enable full-color mode
  Bdisp_EnableColor(1);

  while (1) {
    // Update the keyboard state
    kb_update();
    kb_handle_special_keys();

    // Update game timer
    int32_t current_ticks = RTC_GetTicks();
    // Handle RTC tick reset at midnight
    if (current_ticks < last_ticks)
      last_ticks -= TICKS_PER_DAY;

    tick_counter += current_ticks - last_ticks;
    last_ticks = current_ticks;

    int tick = tick_counter >= game_speed;
    if (kb_keydown(KEY_PRGM_DOWN))
      tick = 1;

    // Move tetromino down, on collision get next
    if (tick) {
      tick_counter = tick_counter > game_speed ? tick_counter - game_speed : 0;

      if (colcheck_down(grid, t_current, t_gridpos)) {
        tetro_to_grid(grid, t_current, n_current, t_gridpos);
        check_lines(grid);

        t_gridpos = (point){4, 1};
        next_tetro(t_current, t_next, &n_current, &n_next, &rng);
      } else {
        t_gridpos.y++;
      }
    }

    // Rotate tetromino, unless it's a square block
    if ((kb_keydown(KEY_PRGM_UP) || kb_keydown(KEY_PRGM_F1)) && n_current != 1)
      try_rotate(grid, t_current, t_gridpos);

    // Move tetromino
    if (kb_keydown(KEY_PRGM_LEFT) &&
        !colcheck_x(grid, t_current, t_gridpos, -1))
      t_gridpos.x--;
    if (kb_keydown(KEY_PRGM_RIGHT) &&
        !colcheck_x(grid, t_current, t_gridpos, 1))
      t_gridpos.x++;

    // Clear VRAM
    Bdisp_AllClr_VRAM();

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
    draw_grid(grid, colors);

    // Draw the current tetromino in play
    draw_tetro(GRID_START_X + t_gridpos.x * GRID_BLOCK_SIZE,
               GRID_START_Y + t_gridpos.y * GRID_BLOCK_SIZE, t_current,
               n_current, colors);

    // Draw next tetromino preview
    draw_tetro(next_pos.x, next_pos.y, t_next, n_next, colors);

    Bdisp_PutDisp_DD();
  }

  int key;
  while (1) {
    GetKey(&key);
  }

  return 0;
}
