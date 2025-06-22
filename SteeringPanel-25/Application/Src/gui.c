#include "gui.h"
#include "lvgl/lvgl.h"
#include "utils.h"
#include "steering.h"
#include <math.h>

volatile LvglObjects lvglObjects = {0};

const size_t SCREEN_WIDTH_PX = 800;
const size_t SCREEN_HEIGHT_PX = 480;

LV_DRAW_BUF_DEFINE_STATIC(drawBuffer, GRID_WIDTH_PX, GRID_HEIGHT_PX, LV_COLOR_FORMAT_RGB565);

void initLVGL(void)
{
    /* Change Active Screen's background color */
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x00ff00), LV_PART_MAIN);
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);

    initGrid2(lv_screen_active());
    initDebugMsg();
}

int refreshScreen(void)
{
    updateCellVoltages();

    if (incomingData.debugMessage[0] != '\0') {
        lv_label_set_text_static(lvglObjects.label.text, (const char*)incomingData.debugMessage);
        lv_obj_clear_flag(lvglObjects.label.panel, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(lvglObjects.label.panel, LV_OBJ_FLAG_HIDDEN);
    }

    return 1;
}

char cellTempBuf[CELL_COUNT*3];

void initGrid2()
{
    LV_DRAW_BUF_INIT_STATIC(drawBuffer);

    lvglObjects.canvas = lv_canvas_create(lv_screen_active());
    lv_obj_set_size(lvglObjects.canvas, GRID_WIDTH_PX, GRID_HEIGHT_PX);
    lv_canvas_set_draw_buf(lvglObjects.canvas, &drawBuffer);

    lv_canvas_fill_bg(lvglObjects.canvas, lv_color_hex(0xff00ff), LV_OPA_COVER);
    lv_obj_center(lvglObjects.canvas);

    for (size_t i = 0; i < CELL_COUNT; i++) {
        cellTempBuf[i*3+2] = '\0';
    }
}

lv_color_t tempMap(uint8_t temp) {
    if (temp < 50) return lv_color_hex(CT_COLOR_BLACK);
    if (temp < 135) return lv_color_mix(lv_color_hex(CT_COLOR_RED), lv_color_hex(CT_COLOR_YELLOW), (temp-50)<<1);
    if (temp < 220) return lv_color_mix(lv_color_hex(CT_COLOR_YELLOW), lv_color_hex(CT_COLOR_GREEN), (temp-135)<<1);
    return lv_color_hex(CT_COLOR_PURPLE);
}

void updateCellVoltages()
{
    lv_layer_t layer;
    lv_canvas_init_layer(lvglObjects.canvas, &layer);

    lv_draw_rect_dsc_t rectDesc;
    lv_draw_rect_dsc_init(&rectDesc);
    rectDesc.border_color = lv_color_hex(GR_COLOR_PURPLE);
    rectDesc.border_width = 0;
    rectDesc.radius = 0;

    lv_draw_label_dsc_t labelDesc;
    lv_draw_label_dsc_init(&labelDesc);
    labelDesc.color = lv_color_hex(GR_COLOR_WHITE);
    
    int i = 0;
    int caseNum = 0;
    lv_area_t coords;
    while (i < CELL_COUNT) {
        coords.x1 = 10 + caseNum*50;
        coords.x2 = 30 + caseNum*50;

        for (int j = 0; j < 15; j++) {
            rectDesc.bg_color = tempMap(incomingData.cellData[i].cellTemperature);
            coords.y1 = 10 + j*25;
            coords.y2 = 30 + j*25;
            cellTempBuf[i*3] = ((incomingData.cellData[i].cellTemperature>>2)/10) + '0';
            cellTempBuf[(i*3)+1] = ((incomingData.cellData[i].cellTemperature>>2)%10) + '0';
            labelDesc.text = (cellTempBuf+(i*3));
            lv_draw_rect(&layer, &rectDesc, &coords);
            lv_draw_label(&layer, &labelDesc, &coords);
            i++;
        }

        coords.x1 = 35 + caseNum*50;
        coords.x2 = 55 + caseNum*50;
        i += 12;
        for (int j = 0; j < 13; j++) {
            rectDesc.bg_color = tempMap(incomingData.cellData[i].cellTemperature);
            coords.y1 = 20 + j*25;
            coords.y2 = 40 + j*25;
            cellTempBuf[i*3] = ((incomingData.cellData[i].cellTemperature>>2)/10) + '0';
            cellTempBuf[(i*3)+1] = ((incomingData.cellData[i].cellTemperature>>2)%10) + '0';
            labelDesc.text = (cellTempBuf+(i*3));
            lv_draw_rect(&layer, &rectDesc, &coords);
            lv_draw_label(&layer, &labelDesc, &coords);
            i--;
        }
        i += 14;
        caseNum++;
    }

    lv_canvas_finish_layer(lvglObjects.canvas, &layer);
}

void initDebugMsg()
{
    lvglObjects.label.panel = lv_obj_create(lv_screen_active());
    lv_obj_set_size(lvglObjects.label.panel, 800, 70);
    lv_obj_center(lvglObjects.label.panel);
    lv_obj_set_style_bg_color(lvglObjects.label.panel, lv_color_hex(0x7920FF), LV_PART_MAIN);
    lv_obj_add_flag(lvglObjects.label.panel, LV_OBJ_FLAG_HIDDEN);

    lvglObjects.label.text = lv_label_create(lvglObjects.label.panel);
    lv_label_set_text(lvglObjects.label.text, "");
    lv_obj_center(lvglObjects.label.text);
}