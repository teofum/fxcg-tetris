#include "mainmenu.h"

#include <fxcg/display.h>
#include <fxcg/keyboard.h>

#include "grid.h"

#define LOGO_START_X ((LCD_WIDTH_PX >> 1) - 105)
#define LOGO_START_Y 40
#define BLOCK_X(x) (LOGO_START_X + x * GRID_BLOCK_SIZE)
#define BLOCK_Y(y) (LOGO_START_Y + y * GRID_BLOCK_SIZE)
#define BLOCK(x, y)                                                            \
  ((rect){BLOCK_X(x), BLOCK_Y(y), BLOCK_X(x) + GRID_BLOCK_SIZE - 1,            \
          BLOCK_Y(y) + GRID_BLOCK_SIZE - 1})

int mm_show() {
  Bdisp_AllClr_VRAM();

  // Show logo
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
