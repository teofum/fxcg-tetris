#include "dialog.h"

#include <gint/keyboard.h>
#include <stdio.h>
#include <string.h>

#include "font.h"

#define FRAME_WIDTH 8
#define TITLE_LEFT 8
#define TITLE_TOP 8
#define OPTION_LEFT 8
#define OPTION_BOTTOM 6
#define OPTION_HEIGHT 14
#define OPTION_INDENT 10
#define OPTION_HINT_RIGHT 8

#define INPUT_HEIGHT 20
#define INPUT_CHAR_WIDTH 7

#define to_lower(x) ((x) + 'a' - 'A')
#define min(x, y) ((x) <= (y) ? (x) : (y))
#define max(x, y) ((x) >= (y) ? (x) : (y))

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

static int get_key_alpha(uint key) {
  int c = -1;

  if (key >= 0x61 && key <= 0x66)
    c = key - 0x61 + 'A'; // A-F
  else if (key >= 0x51 && key <= 0x56)
    c = key - 0x51 + 'G'; // G-L
  else if (key >= 0x41 && key <= 0x43)
    c = key - 0x41 + 'M'; // M-O
  else if (key >= 0x31 && key <= 0x35)
    c = key - 0x31 + 'P'; // P-T
  else if (key >= 0x21 && key <= 0x25)
    c = key - 0x21 + 'U'; // U-Y
  else if (key == 0x11)
    c = 'Z'; // Z
  else if (key == 0x12)
    c = ' '; // Space

  return c;
}

void dialog_textinput(
    const char *title,
    const char *prompt,
    char *text,
    uint32_t max_length,
    int16_t margin_x,
    int16_t margin_y,
    color_t frame_color
) {
  disp_shade_rect(0, 0, DWIDTH, DHEIGHT, C_WHITE);

  text[max_length] = 0; // Text should be at least max_length + 1 chars
  int32_t cursor = min(strlen(text), max_length - 1);
  int uppercase = (cursor == 0);

  key_event_t event = {0};
  while (event.key != KEY_EXE) {
    int c = get_key_alpha(event.key);
    if (c == -1) {
      // Handle non-character keys
      switch (event.key) {
      case KEY_LEFT:
        cursor = max(0, cursor - 1);
        break;
      case KEY_RIGHT:
        cursor = min(max_length - 1, cursor + 1);
        break;
      case KEY_OPTN:
        uppercase = !uppercase;
        break;
      case KEY_DEL:
        text[cursor] = text[cursor + 1] == 0 ? 0 : ' ';
        cursor = max(0, cursor - 1);
        if (cursor == 0)
          uppercase = 1;
        break;
      case KEY_ACON:
        for (int i = 0; i < max_length; i++)
          text[i] = 0;
        cursor = 0;
        uppercase = 1;
        break;
      }
    } else {
      // Handle alpha keys
      if (!uppercase && !event.shift)
        c = to_lower(c);

      // Make sure the string is null-terminated
      if (cursor >= strlen(text)) {
        for (int i = 0; i < cursor; i++)
          if (text[i] == 0)
            text[i] = ' ';
        text[cursor + 1] = 0;
      }

      text[cursor] = c;
      cursor = min(max_length - 1, cursor + 1);

      if (cursor == 1)
        uppercase = 0;
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

    x0 += OPTION_LEFT;
    y1 -= OPTION_BOTTOM;

    // Draw prompt
    dtext(x0, y1 - 3 * OPTION_HEIGHT - INPUT_HEIGHT, C_BLACK, prompt);

    // Draw input box and status
    dtext(x0, y1 - 2 * OPTION_HEIGHT - INPUT_HEIGHT, C_BLACK, text);
    for (int i = 0; i < max_length; i++) {
      drect(
          x0 + i * INPUT_CHAR_WIDTH + 1, y1 - 2 * OPTION_HEIGHT - 5,
          x0 + (i + 1) * INPUT_CHAR_WIDTH - 2,
          y1 - 2 * OPTION_HEIGHT - (i == cursor ? 4 : 5),
          (i == cursor ? C_BLUE : C_BLACK)
      );
    }

    // Draw hints
    dprint(
        x0, y1 - 2 * OPTION_HEIGHT, C_BLACK, "[OPTN] %s",
        uppercase ? "Lowercase" : "Uppercase"
    );
    dtext(x0, y1 - OPTION_HEIGHT, C_BLACK, "[AC] Clear");
    dtext_opt(
        x1 - OPTION_HINT_RIGHT, y1 - OPTION_HEIGHT, C_BLACK, C_NONE,
        DTEXT_RIGHT, DTEXT_TOP, "[EXE] Done"
    );

    dupdate();
    event = getkey();
  }
}
