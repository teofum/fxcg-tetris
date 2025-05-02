#include "gfx.h"

color_t *vram = (void *)0xac000000;

void disp_fill_rect(short x0, short y0, short x1, short y1, color_t color) {
  for (short y = y0; y <= y1; y++) {
    for (short x = x0; x <= x1; x++) {
      vram[y * LCD_WIDTH_PX + x] = color;
    }
  }
}

color_t color(unsigned char r, unsigned char g, unsigned char b) {
  return (((r >> 3) & 0x1f) << 11) + (((g >> 2) & 0x3f) << 5) +
         ((b >> 3) & 0x1f);
}
