#include "game.h"
#include "mainmenu.h"

#include <gint/display.h>
#include <gint/keyboard.h>

#include "utils/font.h"

int main() {
  // Set default font
  dfont(&dosv_jpn12);

  game_t game;

  gint_call_t call_hscore_load = GINT_CALL(hscore_load, (void *)game.hscores);
  gint_world_switch(call_hscore_load);

  int game_running = 0;
  while (1) {
    game_running = mm_show(&game);

    if (game_running == 1)
      // Initialize game
      game_init(&game);

    while (game_running == 1) {
      // Update the keyboard state
      cleareventflips();
      clearevents();

      // Update the game state
      game_running = game_update(&game);

      // Clear VRAM
      dclear(C_WHITE);

      game_draw(&game);

      // Present VRAM to screen
      dupdate();
    }
  }

  return 0;
}