#include "mainmenu.h"

#include <fxcg/display.h>
#include <fxcg/keyboard.h>

int mm_show() {
  Bdisp_AllClr_VRAM();

  PrintCXY(100, 36, "TETRIS", 0x20, -1, 0, 0, 1, 0);

  int x = 0, y = 0;
  PrintMini(&x, &y, "Press [EXE] to start", 0x42, -1, 0, 0, 0, 0, 0, 0);
  x = (LCD_WIDTH_PX >> 1) - (x >> 1);
  y = 136;
  PrintMini(&x, &y, "Press [EXE] to start", 0x42, -1, 0, 0, 0, 0, 1, 0);

  int key = 0;
  while (key != KEY_CTRL_EXE) {
    GetKey(&key);
  }

  return 1;
}
