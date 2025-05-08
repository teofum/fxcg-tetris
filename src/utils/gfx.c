#include "gfx.h"

void disp_shade_rect(short x0, short y0, short x1, short y1, color_t color) {
  for (short y = y0; y <= y1; y++) {
    for (short x = x0 + ((y - y0) & 1); x <= x1; x += 2) {
      dpixel(x, y, color);
    }
  }
}

void disp_frame(int16_t x0, int16_t y0, int16_t x1, int16_t y1, color_t color) {
  // Outer black frame
  dline(x0, y0, x0, y1, C_BLACK);
  dline(x1, y0, x1, y1, C_BLACK);
  dline(x0 + 1, y0, x1 - 1, y0, C_BLACK);
  dline(x0 + 1, y1, x1 - 1, y1, C_BLACK);

  // Outer shaded frame
  dline(x0 + 1, y0 + 1, x0 + 1, y1 - 1, C_WHITE);
  dline(x0 + 2, y0 + 1, x1 - 1, y0 + 1, C_WHITE);
  dline(x1 - 1, y0 + 2, x1 - 1, y1 - 1, C_BLACK);
  dline(x0 + 2, y1 - 1, x1 - 2, y1 - 1, C_BLACK);

  // Color frame
  drect(x0 + 2, y0 + 2, x0 + 5, y1 - 2, color);
  drect(x1 - 5, y0 + 2, x1 - 2, y1 - 2, color);
  drect(x0 + 6, y0 + 2, x1 - 6, y0 + 5, color);
  drect(x0 + 6, y1 - 5, x1 - 6, y1 - 2, color);

  // Inner shaded frame
  dline(x0 + 7, y0 + 6, x1 - 6, y0 + 6, C_BLACK);
  dline(x1 - 6, y0 + 7, x1 - 6, y1 - 6, C_WHITE);
  dline(x0 + 6, y0 + 6, x0 + 6, y1 - 6, C_BLACK);
  dline(x0 + 7, y1 - 6, x1 - 7, y1 - 6, C_WHITE);

  // Inner black frame
  dline(x0 + 7, y0 + 7, x0 + 7, y1 - 7, C_BLACK);
  dline(x1 - 7, y0 + 7, x1 - 7, y1 - 7, C_BLACK);
  dline(x0 + 8, y0 + 7, x1 - 8, y0 + 7, C_BLACK);
  dline(x0 + 8, y1 - 7, x1 - 8, y1 - 7, C_BLACK);

  // Fill
  drect(x0 + 8, y0 + 8, x1 - 8, y1 - 8, C_WHITE);
}

void extract_color(color_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
  *r = (color >> 11) << 3;
  *g = ((color >> 5) & 0x3f) << 2;
  *b = (color & 0x1f) << 3;
}
