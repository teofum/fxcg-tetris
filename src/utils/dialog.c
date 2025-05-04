#include "dialog.h"

#include <fxcg/keyboard.h>
#include <stdio.h>

uint32_t dialog(const char *title, const char **options, uint32_t n_opts,
                color_t frame_color, void (*draw_cb)(void *), void *cb_param) {
  disp_shade_rect(0, 0, LCD_WIDTH_PX, LCD_HEIGHT_PX, COLOR_WHITE);

  // Clear the GetKey buffer
  int key;
  while (GetKeyWait_OS(&key, &key, KEYWAIT_HALTOFF_TIMEROFF, 0, 0, NULL))
    ;
  key = 0;

  int selected_option = 0;
  while (key != KEY_CTRL_EXE) {
    // Handle keys
    switch (key) {
    case KEY_CTRL_UP:
      selected_option = selected_option == 0 ? n_opts - 1 : selected_option - 1;
      break;
    case KEY_CTRL_DOWN:
      selected_option = (selected_option + 1) % n_opts;
      break;
    case KEY_CTRL_F1:
      return 0;
    case KEY_CTRL_F2:
      return 1;
    case KEY_CTRL_F3:
      return 2;
    case KEY_CTRL_F4:
      return 3;
    case KEY_CTRL_F5:
      return 4;
    case KEY_CTRL_F6:
      return 5;
    }

    // Draw frame
    disp_frame(80, 40, LCD_WIDTH_PX - 80, LCD_HEIGHT_PX - 40, frame_color);
    PrintCXY(100, 36, title, 0x20, -1, 0, 0, 1, 0);

    int first_y = 158 - 10 * n_opts;
    int cur_x = 100, cur_y = first_y;
    char option_str[64]; // buffer

    // Draw options
    for (int i = 0; i < n_opts; i++) {
      sprintf(option_str, "  %s", options[i]);
      PrintMiniMini(&cur_x, &cur_y, option_str, 0x40, 0, 0);

      if (i < 6) {
        cur_x = 240;
        sprintf(option_str, "[F%d]", i + 1);
        PrintMiniMini(&cur_x, &cur_y, option_str, 0x40, 0, 0);
      }

      cur_x = 100;
      cur_y += 10;
    }

    // Draw current option indicator
    cur_y = first_y + selected_option * 10;
    PrintMiniMini(&cur_x, &cur_y, ">", 0x40, 0, 0);

    // Optionally, run the draw callback
    if (draw_cb != NULL)
      draw_cb(cb_param);

    GetKey(&key);
  }

  return selected_option;
}
