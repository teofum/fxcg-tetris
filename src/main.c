#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/rtc.h>

#include "utils/gfx.h"
#include "utils/input.h"

#include "game.h"

#define TICKS_PER_DAY 128 * 60 * 60 * 24

uint32_t get_score(uint32_t lines) { return (1 << (lines - 1)) * 10; }

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

  // Enable full-color mode
  Bdisp_EnableColor(1);

  // Initialize game
  game_t game;
  game_init(&game, colors);

  while (1) {
    // Update the keyboard state
    kb_update();
    kb_handle_special_keys();

    // Update the game state
    game_update(&game);

    // Clear VRAM
    Bdisp_AllClr_VRAM();

    game_draw(&game);

    // Present VRAM to screen
    Bdisp_PutDisp_DD();
  }

  int key;
  while (1) {
    GetKey(&key);
  }

  return 0;
}
