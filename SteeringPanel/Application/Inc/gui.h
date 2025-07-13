#include <stdint.h>

#include "lvgl/lvgl.h"
#include "steering.h"

#ifndef STEERING_GUI_H
    #define STEERING_GUI_H

    #define GRID_COLUMNS 10
    #define GRID_ROWS 10

    #define GRID_WIDTH_PX 800
    #define GRID_HEIGHT_PX 400

    // See Brand Guidelines
    #define GR_COLOR_PINK 0xEF0DA1
    #define GR_COLOR_PURPLE 0x7920FF
    #define GR_COLOR_NAVY 0x195297
    #define GR_COLOR_GRAY 0x9AA3B0
    #define GR_COLOR_WHITE 0xFFFFFF

    // See Brand Guidelines
    #define CT_COLOR_BLACK 0x202020
    #define CT_COLOR_RED 0xd02020
    #define CT_COLOR_YELLOW 0xb0b020
    #define CT_COLOR_GREEN 0x20d020
    #define CT_COLOR_PURPLE GR_COLOR_PURPLE

    typedef struct {
        struct {
            lv_obj_t * panel;
            lv_obj_t * text;
        } label;

        struct {
            lv_obj_t * chart;
            lv_chart_series_t * ser;
            size_t len;
        } chart;

        lv_obj_t * canvas;
    } LvglObjects;

    void initLVGL(void);
    int refreshScreen(void);
    void updateCellVoltages();
    void initGrid2();

    void initDebugMsg();
#endif
