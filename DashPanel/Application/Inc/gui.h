// gui.h

#ifndef GUI_H
#define GUI_H

#include "lvgl/lvgl.h"
#include "dash.h"

// --- WIDGET STYLES ---
#define GR_NAVY 0x195297
#define GR_PINK 0xEF0DA1
#define GR_PRPL 0x7920FF
#define GR_GRAY 0x9AA3B0
#define TOP_HEIGHT 200
#define GRID_ROWS 2
#define GRID_COLUMNS 4
#define CELL_WIDTH_PX 20
#define CELL_HEIGHT_PX 20
#define GRID_WIDTH_PX 180
#define GRID_HEIGHT_PX 180

// --- Global or static variables needed in the timer callback ---
extern lv_obj_t * speed;
extern lv_obj_t * state; 
extern lv_obj_t * voltage;
extern lv_obj_t * SoC;
extern lv_obj_t * power;   

extern lv_obj_t * cell;
extern lv_obj_t * motor;
extern lv_obj_t * inverter;
extern lv_obj_t * brake;

extern lv_obj_t * current;
extern lv_obj_t * torqueMapping;
extern lv_obj_t * regen;

extern lv_obj_t * wheelDispCanvas;


typedef struct {
    lv_obj_t * panel;
    lv_obj_t * text;
} DebugMsg;

extern DebugMsg debugMsg;

extern lv_obj_t * gridCells[GRID_ROWS * GRID_COLUMNS];

void styleSetup(void);
void displaySetup(void);
void topSetup(lv_obj_t * parent_obj);
void bottomSetup(lv_obj_t * parent_obj);
void initDebugMsg(lv_obj_t * parent_obj);
// static void ecu_update_timer_cb(lv_timer_t * timer);
void createGrid(lv_obj_t * parent_obj);

#endif

