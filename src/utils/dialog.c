#include "dialog.h"

#include <gint/keyboard.h>
#include <stdio.h>

#include "font.h"

#define FRAME_WIDTH 8
#define TITLE_LEFT 8
#define TITLE_TOP 8
#define OPTION_LEFT 8
#define OPTION_BOTTOM 6
#define OPTION_HEIGHT 14
#define OPTION_INDENT 10
#define OPTION_HINT_RIGHT 8

uint32_t dialog(
    const char *title,
    const char **options,
    uint32_t n_opts,
    int16_t margin_x,
    int16_t margin_y,
    color_t frame_color,
    void (*draw_cb)(void *),
    void *cb_param
) {
  disp_shade_rect(0, 0, DWIDTH, DHEIGHT, C_WHITE);

  // Clear the GetKey buffer
  key_event_t event;
  int selected_option = 0;
  while (event.key != KEY_EXE) {
    // Handle keys
    switch (event.key) {
    case KEY_UP:
      selected_option = selected_option == 0 ? n_opts - 1 : selected_option - 1;
      break;
    case KEY_DOWN:
      selected_option = (selected_option + 1) % n_opts;
      break;
    case KEY_F1:
      return 0;
    case KEY_F2:
      return 1;
    case KEY_F3:
      return 2;
    case KEY_F4:
      return 3;
    case KEY_F5:
      return 4;
    case KEY_F6:
      return 5;
    }

    // Draw frame
    int16_t x0 = margin_x, y0 = margin_y;
    int16_t x1 = DWIDTH - margin_x, y1 = DHEIGHT - margin_y;
    disp_frame(x0, y0, x1, y1, frame_color);

    x0 += FRAME_WIDTH;
    y0 += FRAME_WIDTH;
    x1 -= FRAME_WIDTH;
    y1 -= FRAME_WIDTH;

    // Draw title
    const font_t *prev_font = dfont(&dosv_ank24);
    dtext(x0 + TITLE_LEFT, y0 + TITLE_TOP, C_BLACK, title);
    dfont(prev_font);

    int first_y = y1 - OPTION_BOTTOM - OPTION_HEIGHT * n_opts;
    int option_x = x0 + OPTION_LEFT, option_y = first_y;

    // Draw options
    for (int i = 0; i < n_opts; i++) {
      dprint(option_x + OPTION_INDENT, option_y, C_BLACK, "%s", options[i]);

      if (i < 6) {
        dprint_opt(
            x1 - OPTION_HINT_RIGHT, option_y, C_BLACK, C_NONE, DTEXT_RIGHT,
            DTEXT_TOP, "[F%d]", i + 1
        );
      }

      option_y += OPTION_HEIGHT;
    }

    // Draw current option indicator
    option_y = first_y + selected_option * OPTION_HEIGHT;
    dtext(option_x, option_y, C_BLACK, ">");

    // Optionally, run the draw callback
    if (draw_cb != NULL)
      draw_cb(cb_param);

    dupdate();
    event = getkey();
  }

  return selected_option;
}
