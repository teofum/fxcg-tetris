#ifndef __GFX_H__
#define __GFX_H__

#include <fxcg/display.h>

void disp_fill_rect(short x0, short y0, short x1, short y1, color_t color);

color_t color(unsigned char r, unsigned char g, unsigned char b);

#endif
