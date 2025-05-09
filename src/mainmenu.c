#include "mainmenu.h"
#include "grid.h"
#include "utils/dialog.h"
#include "utils/font.h"

#include <gint/keyboard.h>

#define LOGO_START_X ((DWIDTH >> 1) - 105)
#define LOGO_START_Y 40
#define BLOCK_X(x) (LOGO_START_X + x * GRID_BLOCK_SIZE)
#define BLOCK_Y(y) (LOGO_START_Y + y * GRID_BLOCK_SIZE)
#define BLOCK(x, y)                                                            \
  ((rect){BLOCK_X(x), BLOCK_Y(y), BLOCK_X(x) + GRID_BLOCK_SIZE - 1,            \
          BLOCK_Y(y) + GRID_BLOCK_SIZE - 1})

#define N_OPTIONS 2
#define OPTION_LEFT 100
#define OPTION_BOTTOM 36
#define OPTION_HEIGHT 20
#define OPTION_INDENT 12
#define OPTION_HINT_RIGHT 100

const char *options[2] = {"Play", "Highscores"};
const char *hscore_options[1] = {"Close"};

void draw_hscores(highscore_t *hscores) {
  font_t *prev_font;

  for (int i = 0; i < HSCORE_COUNT; i++) {
    dprint(96, 72 + i * OPTION_HEIGHT + 3, C_BLACK, "%s", hscores[i].name);
    prev_font = dfont(&dosv_ank16);
    dprint_opt(
        DWIDTH - 96, 72 + i * OPTION_HEIGHT, C_BLACK, C_NONE, DTEXT_RIGHT,
        DTEXT_TOP, "%u", hscores[i].score
    );
    dfont(prev_font);
  }
}

void draw_logo() {
  draw_block(BLOCK(0, 0), 0);
  draw_block(BLOCK(1, 0), 0);
  draw_block(BLOCK(2, 0), 0);
  draw_block(BLOCK(1, 1), 0);
  draw_block(BLOCK(1, 2), 0);
  draw_block(BLOCK(1, 3), 0);
  draw_block(BLOCK(1, 4), 0);

  draw_block(BLOCK(4, 0), 1);
  draw_block(BLOCK(5, 0), 1);
  draw_block(BLOCK(6, 0), 1);
  draw_block(BLOCK(4, 1), 1);
  draw_block(BLOCK(4, 2), 1);
  draw_block(BLOCK(5, 2), 1);
  draw_block(BLOCK(4, 3), 1);
  draw_block(BLOCK(4, 4), 1);
  draw_block(BLOCK(5, 4), 1);
  draw_block(BLOCK(6, 4), 1);

  draw_block(BLOCK(8, 0), 2);
  draw_block(BLOCK(9, 0), 2);
  draw_block(BLOCK(10, 0), 2);
  draw_block(BLOCK(9, 1), 2);
  draw_block(BLOCK(9, 2), 2);
  draw_block(BLOCK(9, 3), 2);
  draw_block(BLOCK(9, 4), 2);

  draw_block(BLOCK(12, 0), 3);
  draw_block(BLOCK(13, 0), 3);
  draw_block(BLOCK(12, 1), 3);
  draw_block(BLOCK(14, 1), 3);
  draw_block(BLOCK(12, 2), 3);
  draw_block(BLOCK(13, 2), 3);
  draw_block(BLOCK(12, 3), 3);
  draw_block(BLOCK(14, 3), 3);
  draw_block(BLOCK(12, 4), 3);
  draw_block(BLOCK(14, 4), 3);

  draw_block(BLOCK(16, 0), 4);
  draw_block(BLOCK(16, 1), 4);
  draw_block(BLOCK(16, 2), 4);
  draw_block(BLOCK(16, 3), 4);
  draw_block(BLOCK(16, 4), 4);

  draw_block(BLOCK(19, 0), 5);
  draw_block(BLOCK(20, 0), 5);
  draw_block(BLOCK(18, 1), 5);
  draw_block(BLOCK(19, 2), 5);
  draw_block(BLOCK(20, 3), 5);
  draw_block(BLOCK(18, 4), 5);
  draw_block(BLOCK(19, 4), 5);
}

int mm_show(game_t *game) {
  font_t *prev_font = dfont(&dosv_ank16);

  clearevents();
  key_event_t event = {0};
  int selected_option = 0;
  while (event.key != KEY_EXE) {
    switch (event.key) {
    case KEY_UP:
      selected_option =
          selected_option == 0 ? N_OPTIONS - 1 : selected_option - 1;
      break;
    case KEY_DOWN:
      selected_option = (selected_option + 1) % N_OPTIONS;
      break;
    }

    // Clear VRAM
    dclear(C_WHITE);

    // Draw logo
    draw_logo();

    // Draw options
    int first_y = DHEIGHT - OPTION_BOTTOM - OPTION_HEIGHT * N_OPTIONS;
    int option_x = OPTION_LEFT, option_y = first_y;

    for (int i = 0; i < N_OPTIONS; i++) {
      dprint(option_x + OPTION_INDENT, option_y, C_BLACK, "%s", options[i]);
      option_y += OPTION_HEIGHT;
    }

    // Draw current option indicator
    option_y = first_y + selected_option * OPTION_HEIGHT;
    dtext(option_x, option_y, C_BLACK, ">");

    dupdate();
    event = getkey();
  }

  dfont(prev_font);

  switch (selected_option) {
  case 0:
    return 1;
  case 1:
    dialog(
        "Highscores", hscore_options, 1, 80, 20, C_BLUE, (void *)draw_hscores,
        game->hscores
    );
    return 0;
  }

  return 1;
}
