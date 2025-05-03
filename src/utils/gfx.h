#ifndef __GFX_H__
#define __GFX_H__

#include <fxcg/display.h>

#include "types.h"

#define COLOR(r, g, b)                                                         \
  (((r >> 3) & 0x1f) << 11) + (((g >> 2) & 0x3f) << 5) + ((b >> 3) & 0x1f)

void disp_fill_rect(short x0, short y0, short x1, short y1, color_t color);

void disp_shade_rect(short x0, short y0, short x1, short y1, color_t color);

void disp_frame(int16_t x0, int16_t y0, int16_t x1, int16_t y1, color_t color);

void extract_color(color_t color, uint8_t *r, uint8_t *g, uint8_t *b);

#endif
