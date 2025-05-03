#include "gfx.h"

color_t *vram = (void *)0xac000000;

void disp_fill_rect(short x0, short y0, short x1, short y1, color_t color) {
  for (short y = y0; y <= y1; y++) {
    for (short x = x0; x <= x1; x++) {
      vram[y * LCD_WIDTH_PX + x] = color;
    }
  }
}

void disp_shade_rect(short x0, short y0, short x1, short y1, color_t color) {
  for (short y = y0; y <= y1; y++) {
    for (short x = x0 + ((y - y0) & 1); x <= x1; x += 2) {
      vram[y * LCD_WIDTH_PX + x] = color;
    }
  }
}

void disp_frame(int16_t x0, int16_t y0, int16_t x1, int16_t y1, color_t color) {
  // Outer black frame
  disp_fill_rect(x0, y0, x0, y1, COLOR_BLACK);
  disp_fill_rect(x1, y0, x1, y1, COLOR_BLACK);
  disp_fill_rect(x0 + 1, y0, x1 - 1, y0, COLOR_BLACK);
  disp_fill_rect(x0 + 1, y1, x1 - 1, y1, COLOR_BLACK);

  // Outer shaded frame
  disp_fill_rect(x0 + 1, y0 + 1, x0 + 1, y1 - 1, COLOR_WHITE);
  disp_fill_rect(x0 + 2, y0 + 1, x1 - 1, y0 + 1, COLOR_WHITE);
  disp_fill_rect(x1 - 1, y0 + 2, x1 - 1, y1 - 1, COLOR_BLACK);
  disp_fill_rect(x0 + 2, y1 - 1, x1 - 2, y1 - 1, COLOR_BLACK);

  // Color frame
  disp_fill_rect(x0 + 2, y0 + 2, x0 + 5, y1 - 2, color);
  disp_fill_rect(x1 - 5, y0 + 2, x1 - 2, y1 - 2, color);
  disp_fill_rect(x0 + 6, y0 + 2, x1 - 6, y0 + 5, color);
  disp_fill_rect(x0 + 6, y1 - 5, x1 - 6, y1 - 2, color);

  // Inner shaded frame
  disp_fill_rect(x0 + 6, y0 + 6, x0 + 6, y1 - 6, COLOR_BLACK);
  disp_fill_rect(x0 + 7, y0 + 6, x1 - 6, y0 + 6, COLOR_BLACK);
  disp_fill_rect(x1 - 6, y0 + 7, x1 - 6, y1 - 6, COLOR_WHITE);
  disp_fill_rect(x0 + 7, y1 - 6, x1 - 7, y1 - 6, COLOR_WHITE);

  // Inner black frame
  disp_fill_rect(x0 + 7, y0 + 7, x0 + 7, y1 - 7, COLOR_BLACK);
  disp_fill_rect(x1 - 7, y0 + 7, x1 - 7, y1 - 7, COLOR_BLACK);
  disp_fill_rect(x0 + 8, y0 + 7, x1 - 8, y0 + 7, COLOR_BLACK);
  disp_fill_rect(x0 + 8, y1 - 7, x1 - 8, y1 - 7, COLOR_BLACK);

  // Fill
  disp_fill_rect(x0 + 8, y0 + 8, x1 - 8, y1 - 8, COLOR_WHITE);
}

void extract_color(color_t color, uint8_t *r, uint8_t *g, uint8_t *b) {
  *r = (color >> 11) << 3;
  *g = ((color >> 5) & 0x3f) << 2;
  *b = (color & 0x1f) << 3;
}
