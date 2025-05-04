#include <fxcg/display.h>
#include <fxcg/keyboard.h>
#include <fxcg/rtc.h>

#include "utils/input.h"

#include "game.h"
#include "mainmenu.h"

#define TICKS_PER_DAY 128 * 60 * 60 * 24

uint32_t get_score(uint32_t lines) { return (1 << (lines - 1)) * 10; }

int main() {
  // Enable full-color mode
  Bdisp_EnableColor(1);

  // Disable status bar when GetKey is called
  EnableDisplayHeader(0, 0);

  game_t game;
  int game_running = 0;

  while (1) {
    game_running = mm_show();

    if (game_running)
      // Initialize game
      game_init(&game);

    while (game_running) {
      // Update the keyboard state
      kb_update();
      kb_handle_special_keys();

      // Update the game state
      game_running = game_update(&game);

      // Clear VRAM
      Bdisp_AllClr_VRAM();

      game_draw(&game);

      // Present VRAM to screen
      Bdisp_PutDisp_DD();
    }
  }

  return 0;
}
