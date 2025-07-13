/*********************
 *      INCLUDES
 *********************/

 #include "lvgl_port_display.h"
 #include "main.h"
 #include "ltdc.h"
 #include "dma2d.h"
 #include "stm32u5xx_hal_dma2d.h"
 #include "cmsis_os2.h"
 
 /**********************
  *  STATIC PROTOTYPES
  **********************/
 
 static void disp_flush(lv_display_t *, const lv_area_t *, uint8_t *color_p);
 
 /**********************
  *  STATIC VARIABLES
  **********************/
 
 lv_display_t *display;
 
 /*Add commentMore actions
 ltdc_buf:
  - this is a 2nd buffer that gets data DMA'ed to it from lvgl_buf
  - LTDC is the STM32 peripheral responsible for continuously sending pixel data to the physical display panel
  - LTDC stands for LCD-TFT display controller

lvgl_buf:
  - this buffer is drawn to by lvgl
 */
 volatile uint16_t ltdc_buf[MY_DISP_HOR_RES * MY_DISP_VER_RES];
 volatile uint16_t lvgl_buf[MY_DISP_HOR_RES * MY_DISP_VER_RES];
 
 /**********************
  *   GLOBAL FUNCTIONS
  **********************/
 
 void lvgl_display_init(void)
 {
   /* display initialization */
   ; /* display is already initialized by cubemx-generated code */
 
   /* display buffer initialization */
   // lv_disp_draw_buf_init (&disp_buf,
   //                        (void*) buf_1,
   //                        NULL,
   //                        MY_DISP_HOR_RES * MY_DISP_VER_RES);
 
   /* register the display in LVGL */
   // lv_disp_drv_init(&disp_drv);
   display = lv_display_create(MY_DISP_HOR_RES, MY_DISP_VER_RES);
   lv_display_set_flush_cb(display, disp_flush); // disp_flush is a callback function that lvgl invokes when finished with drawing to lvgl_buf
   lv_display_set_buffers(display, (void*) &lvgl_buf, NULL, sizeof(lvgl_buf), LV_DISPLAY_RENDER_MODE_DIRECT); // registers lvgl_buf as the DRAWING buffer
 
   HAL_LTDC_SetAddress(&hltdc, (uint32_t) &ltdc_buf, LTDC_LAYER_1);
 
   /* set the resolution of the display */
   // disp_drv.hor_res = MY_DISP_HOR_RES;
   // disp_drv.ver_res = MY_DISP_VER_RES;
 
   /* set callback for display driver */
   // disp_drv.flush_cb = disp_flush;
   // disp_drv.full_refresh = 0;
   // disp_drv.direct_mode = 1;
 
   /* set a display buffer */
   // disp_drv.draw_buf = &disp_buf;
 
   /* finally register the driver */
   // lv_disp_drv_register(&disp_drv);
 }
 
 /**********************
  *   STATIC FUNCTIONS
  **********************/
 
/*
Overview of double buffer data flow to describe how drawings are updated on screen:
  1. LVGL draws into lvgl_buf
  2. LVGL calls disp_flush once LVGL is done with drawing 
  3. disp_flush uses DMA to copy data from lvgl_buf into ltdc_buf
  4. ltdc_buf is read from continuously from the hardware to refresh the screen
*/
 static void disp_flush(lv_display_t *drv, const lv_area_t *area, uint8_t *color_p)
 {
   UNUSED(drv);
 
   lv_coord_t width = lv_area_get_width(area);
   lv_coord_t height = lv_area_get_height(area);
 
   DMA2D->CR = 0x0U << DMA2D_CR_MODE_Pos;
   DMA2D->FGPFCCR = DMA2D_INPUT_RGB565;
   DMA2D->FGMAR = (uint32_t)color_p;
   DMA2D->FGOR = 0;
   DMA2D->OPFCCR = DMA2D_OUTPUT_RGB565;
   DMA2D->OMAR = ((uint32_t) &ltdc_buf) + 2 * (area->y1 * MY_DISP_HOR_RES + area->x1);
   DMA2D->OOR = MY_DISP_HOR_RES - width;
   DMA2D->NLR = (width << DMA2D_NLR_PL_Pos) | (height << DMA2D_NLR_NL_Pos);
   DMA2D->IFCR = 0x3FU;
   DMA2D->CR |= DMA2D_CR_TCIE;
   DMA2D->CR |= DMA2D_CR_START;
 
   lv_display_flush_ready(display);
 }
 