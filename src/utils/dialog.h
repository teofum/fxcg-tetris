#ifndef __DIALOG_H__
#define __DIALOG_H__

#include "gfx.h"

uint32_t dialog(
    const char *title,
    const char **options,
    uint32_t n_opts,
    int16_t margin_x,
    int16_t margin_y,
    color_t frame_color,
    void (*draw_cb)(void *),
    void *cb_param
);

#endif
