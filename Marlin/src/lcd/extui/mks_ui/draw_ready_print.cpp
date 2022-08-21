/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "../../../inc/MarlinConfigPre.h"

#if HAS_TFT_LVGL_UI

#include "draw_ready_print.h"
#include "draw_tool.h"
#include <lv_conf.h>
#include "tft_lvgl_configuration.h"
#include "draw_ui.h"
#include "../../../../src/gcode/gcode.h"
#include "../../../../src/module/stepper.h"
#include "../../../../src/module/endstops.h"
#include "../../../../src/module/motion.h"
#include "../../../../src/sd/cardreader.h"
#include <lvgl.h>

#include "../../../module/temperature.h"
#include "../../../inc/MarlinConfig.h"

#if ENABLED(TOUCH_SCREEN_CALIBRATION)
  #include "../../tft_io/touch_calibration.h"
  #include "draw_touch_calibration.h"
#endif

#include "mks_hardware.h"
#include <stdio.h>

#define ICON_POS_Y          260
#define TARGET_LABEL_MOD_Y -36
#define LABEL_MOD_Y         30


typedef enum{
  READY_IMGBTN_W = 150,
  READY_IMGBTN_H = 152,
  READY_IMGBTN_FIRST_X = 7,
  READY_IMGBTN_X = 6,
  READY_IMGBTN_FIRST_Y = 7,
  READY_IMGBTN_Y = 5,
  READY_IMG_X = 10,
  READY_IMG_Y1 = 11,
  READY_IMG_Y2 = 31
}READY_UI_LAYOUT;

typedef struct{
  lv_obj_t *ex1_temp_label;
  lv_obj_t *ex2_temp_label;
  lv_obj_t *bed_temp_label;

  lv_obj_t *move_imgbrn;
  lv_obj_t *preheat_imgbrn;
  lv_obj_t *print_imgbrn;
  lv_obj_t *tool_imgbrn;
  lv_obj_t *filament_imgbrn;
  lv_obj_t *emergency_imgbrn;

  lv_obj_t *btn_preheat;

  lv_style_t style_btn;
}READY_UI_PAGE;

READY_UI_PAGE ready_print_page;
extern lv_group_t*  g;
extern bool g_emergency_endstop_flag;
bool g_emergency_endstop = false;
bool g_emergency_home = true;
// bool gb_emergency_stop = false;

#ifndef USE_NEW_LVGL_CONF
  static lv_obj_t *scr;
#endif
static lv_obj_t *buttonExt1, *labelExt1, *buttonFanstate, *labelFan;
// extern User_ui_scr mks_printer;
#if HAS_MULTI_HOTEND
  static lv_obj_t *labelExt2;
  static lv_obj_t *buttonExt2;
#endif

#if HAS_HEATED_BED
  static lv_obj_t* labelBed;
  static lv_obj_t* buttonBedstate;
#endif
extern bool g_emergency_stop_flag;
// bool g_move_flag = false;
#if ENABLED(MKS_TEST)
  uint8_t current_disp_ui = 0;
#endif

enum { ID_TOOL = 1, ID_SET, ID_PRINT,ID_FILAMENT, ID_PREHEAT, ID_INFO_BED, ID_INFO_FAN,ID_MOVE,ID_EMER };

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
      button_sound_end();
      lv_clear_ready_print();
      switch (obj->mks_obj_id) {
        case ID_TOOL:   lv_draw_tool(); break;
        case ID_SET:    lv_draw_set(); break;
        case ID_FILAMENT: 
        uiCfg.hotendTargetTempBak = thermalManager.degTargetHotend(active_extruder);
        lv_draw_filament_change();
        break;
        case ID_PREHEAT:  uiCfg.curTempType = 0; lv_draw_preHeat(); break;
        case ID_MOVE:     lv_draw_move_motor();break;  //        g_move_flag = true;
        case ID_INFO_BED:  uiCfg.curTempType = 1; lv_draw_preHeat(); break;
        case ID_INFO_FAN:  lv_draw_fan(); break;
        // case ID_PRINT: TERN(MULTI_VOLUME, lv_draw_media_select(), lv_draw_print_file()); break;
        case ID_PRINT:  lv_draw_print_file(); break;
        case ID_EMER:   emergency(); break;
      }
  }
}

lv_obj_t *limit_info, *det_info;
lv_style_t limit_style, det_style;


void emergency()
{
  g_emergency_endstop = true;
  queue.clear();
  memset(planner.block_buffer, 0, sizeof(planner.block_buffer));
  g_emergency_stop_flag = true;
  // queue.inject("G92 X0 Y0 Z0");
  while(planner.has_blocks_queued())
  {
    stepper.discard_current_block();
  }
  planner.quick_stop();
  // Reiterate heaters off
  thermalManager.disable_all_heaters();

  // TERN_(HAS_CUTTER, cutter.kill());  // Reiterate cutter shutdown

  // Power off all steppers (for M112) or just the E steppers
  stepper.disable_all_steppers();



  // sei();
  TERN(HAS_SUICIDE, suicide(), gcode.process_subcommands_now(PSTR("M410")));
  gcode.process_subcommands_now(PSTR("M84"));
  lv_refr_now(lv_refr_get_disp_refreshing());
  lv_draw_dialog(DIALOG_EMERGENCY_TIPS);
}
void disp_Limit_ok() {
  limit_style.text.color.full = 0xFFFF;
  lv_obj_set_style(limit_info, &limit_style);
  lv_label_set_text(limit_info, "Limit:ok");
}

void disp_Limit_error() {
  limit_style.text.color.full = 0xF800;
  lv_obj_set_style(limit_info, &limit_style);
  lv_label_set_text(limit_info, "Limit:error");
}

void disp_det_ok() {
  det_style.text.color.full = 0xFFFF;
  lv_obj_set_style(det_info, &det_style);
  lv_label_set_text(det_info, "det:ok");
}

void disp_det_error() {
  det_style.text.color.full = 0xF800;
  lv_obj_set_style(det_info, &det_style);
  lv_label_set_text(det_info, "det:error");
}

lv_obj_t *e1, *e2, *e3, *bed;

void mks_disp_test() {
  char buf[30] = {0};
  #if HAS_HOTEND
    sprintf_P(buf, PSTR("e1:%d"), thermalManager.wholeDegHotend(0));
    lv_label_set_text(e1, buf);
  #endif
  #if HAS_MULTI_HOTEND
    sprintf_P(buf, PSTR("e2:%d"), thermalManager.wholeDegHotend(1));
    lv_label_set_text(e2, buf);
  #endif
  #if HAS_HEATED_BED
    sprintf_P(buf, PSTR("bed:%d"), thermalManager.wholeDegBed());
    lv_label_set_text(bed, buf);
  #endif
}

void set_main_screen(void) {
#ifdef USE_NEW_LVGL_CONF
  mks_ui.src_main = lv_obj_create(nullptr, nullptr);
  lv_obj_set_style(mks_ui.src_main, &tft_style_scr);
  lv_scr_load(mks_ui.src_main);
#endif
}
  // lv_obj_t *btn,*label;
// static void event_handler_temp(lv_obj_t *obj, lv_event_t event) {
//   if (event != LV_EVENT_RELEASED) return;
//     lv_label_set_text(label,"hi");
  
// }
// void lv_draw_ready_print() {

//   mks_printer.user_src = lv_screen_user_create(mks_printer.user_src,PRINT_READY_UI, "");
//   // lv_obj_set_style(mks_printer.user_src, &User_style.show_bg_style);

//   btn = lv_btn_create_back(mks_printer.user_src, 10, 10, 100, 50, event_handler_temp, 1);
//   lv_btn_set_style(btn,LV_BTN_STYLE_REL,&User_style.btn2_bg_style);
//   lv_btn_set_style(btn,LV_BTN_STYLE_PR,&User_style.btn2_bg_style);
//   label = lv_label_create(btn,"hello");
//   lv_label_set_style(label,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);
// }
// mks_printer.user_src
// void lv_draw_ready_print() {
//   lv_screen_distribution(PRINT_READY_UI);
//   mks_printer.user_src1 = lv_title_create(mks_printer.user_src, main_menu.tool);

// }
void lv_draw_ready_print() {
  
  char buf[30] = {0};
  lv_obj_t *buttonTool;

  static lv_style_t ready_style;

  disp_state_stack._disp_index = 0;
  ZERO(disp_state_stack._disp_state);
  
#ifdef USE_NEW_LVGL_CONF
  mks_ui.src_main = lv_set_scr_id_title(mks_ui.src_main, PRINT_READY_UI, "");
#else 
  lv_screen_distribution(PRINT_READY_UI);
#endif

  if (mks_test_flag == 0x1E) {
    scr = mks_printer.user_src;
    // Create image buttons
#ifdef USE_NEW_LVGL_CONF
    buttonTool = lv_imgbtn_create(mks_ui.src_main, "F:/bmp_tool.bin", event_handler, ID_TOOL);
#else
    buttonTool = lv_imgbtn_create(scr, "F:/bmp_tool.bin", event_handler, ID_TOOL);
#endif

    lv_obj_set_pos(buttonTool, 360, 180);

    lv_obj_t *label_tool = lv_label_create_empty(buttonTool);
    if (gCfgItems.multiple_language) {
      lv_label_set_text(label_tool, main_menu.tool);
      lv_obj_align(label_tool, buttonTool, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }

    #if HAS_HOTEND
#ifdef USE_NEW_LVGL_CONF
      e1 = lv_label_create_empty(mks_ui.src_main);
#else
      e1 = lv_label_create_empty(scr);
#endif
      lv_obj_set_pos(e1, 20, 20);
      sprintf_P(buf, PSTR("e1:  %d"), thermalManager.wholeDegHotend(0));
      lv_label_set_text(e1, buf);
    #endif
    #if HAS_MULTI_HOTEND
      e2 = lv_label_create_empty(scr);
      lv_obj_set_pos(e2, 20, 45);
      sprintf_P(buf, PSTR("e2:  %d"), thermalManager.wholeDegHotend(1));
      lv_label_set_text(e2, buf);
    #endif
    #if HAS_HEATED_BED
#ifdef USE_NEW_LVGL_CONF
      bed = lv_label_create_empty(mks_ui.src_main);
#else
      bed = lv_label_create_empty(scr);
#endif
      lv_obj_set_pos(bed, 20, 95);
      sprintf_P(buf, PSTR("bed:  %d"), thermalManager.wholeDegBed());
      lv_label_set_text(bed, buf);
    #endif
#ifdef USE_NEW_LVGL_CONF
    limit_info = lv_label_create_empty(mks_ui.src_main);
#else
    limit_info = lv_label_create_empty(scr);
#endif
    lv_style_copy(&limit_style, &lv_style_scr);
    limit_style.body.main_color.full = 0x0000;
    limit_style.body.grad_color.full = 0x0000;
    limit_style.text.color.full      = 0xFFFF;
    lv_obj_set_style(limit_info, &limit_style);

    lv_obj_set_pos(limit_info, 20, 120);
    lv_label_set_text(limit_info, " ");

#ifdef USE_NEW_LVGL_CONF
    det_info = lv_label_create_empty(mks_ui.src_main);
#else 
    det_info = lv_label_create_empty(scr);
#endif
    lv_style_copy(&det_style, &lv_style_scr);
    det_style.body.main_color.full = 0x0000;
    det_style.body.grad_color.full = 0x0000;
    det_style.text.color.full      = 0xFFFF;
    lv_obj_set_style(det_info, &det_style);

    lv_obj_set_pos(det_info, 20, 145);
    lv_label_set_text(det_info, " ");
  }
  else {
    card.abortFilePrintNow();
  // mks_printer.user_src = lv_screen_user_create(mks_printer.user_src,PRINT_READY_UI, "");
  // lv_obj_set_style(mks_printer.user_src, &User_style.show_bg_style);
    lv_style_copy(&ready_print_page.style_btn, &lv_style_scr);
    ready_print_page.style_btn.body.main_color = LV_COLOR_MAKE(0x1b,0xa2,0x64);
    ready_print_page.style_btn.body.grad_color = LV_COLOR_MAKE(0x1b,0xa2,0x64);

  // mks_printer.user_src1 = lv_title_create(mks_printer.user_src);
  // mks_printer.user_src2 = lv_scr_create(mks_printer.user_src,0,TITLE_H,TITLE_W,TFT_HEIGHT-TITLE_H);
  // lv_refr_now(lv_refr_get_disp_refreshing());png_cor_filament
  lv_refr_now(lv_refr_get_disp_refreshing());
  ready_print_page.btn_preheat = lv_btn_create(mks_printer.user_src,NULL);
  lv_obj_set_pos(ready_print_page.btn_preheat,READY_IMGBTN_FIRST_X,READY_IMGBTN_FIRST_Y);
  lv_obj_set_size(ready_print_page.btn_preheat,150,46);
  lv_btn_set_style_both(ready_print_page.btn_preheat,&ready_print_page.style_btn);
  lv_obj_set_event_cb_mks(ready_print_page.btn_preheat, event_handler, ID_PREHEAT, "", 0);
  
  ready_print_page.preheat_imgbrn = lv_imgbtn_label_align_create(mks_printer.user_src, "F:/png_cur_for_preheat.bin", main_menu.preheat,LV_ALIGN_IN_BOTTOM_LEFT, READY_IMGBTN_FIRST_X, READY_IMGBTN_FIRST_Y+46,10, -5, event_handler, ID_PREHEAT);
  ready_print_page.move_imgbrn = lv_imgbtn_label_align_create(mks_printer.user_src, "F:/png_move.bin", main_menu.move,LV_ALIGN_IN_BOTTOM_LEFT, READY_IMGBTN_FIRST_X + READY_IMGBTN_X + READY_IMGBTN_W, READY_IMGBTN_FIRST_Y,10, -5, event_handler, ID_MOVE);
  ready_print_page.print_imgbrn = lv_imgbtn_label_align_create(mks_printer.user_src, "F:/png_print.bin", main_menu.print,LV_ALIGN_IN_BOTTOM_LEFT, READY_IMGBTN_FIRST_X, READY_IMGBTN_FIRST_Y + READY_IMGBTN_H + READY_IMGBTN_Y,10, -5, event_handler, ID_PRINT);
  ready_print_page.filament_imgbrn = lv_imgbtn_label_align_create(mks_printer.user_src, "F:/png_cor_filament.bin",main_menu.filament,LV_ALIGN_IN_BOTTOM_LEFT, READY_IMGBTN_FIRST_X + READY_IMGBTN_X + READY_IMGBTN_W, READY_IMGBTN_FIRST_Y + READY_IMGBTN_H + READY_IMGBTN_Y,10, -5, event_handler, ID_FILAMENT);
  ready_print_page.tool_imgbrn = lv_imgbtn_label_align_create(mks_printer.user_src, "F:/png_tool.bin", main_menu.tool,LV_ALIGN_IN_BOTTOM_LEFT, READY_IMGBTN_FIRST_X, READY_IMGBTN_FIRST_Y + READY_IMGBTN_H*2 + READY_IMGBTN_Y*2,10, -5, event_handler, ID_TOOL);
  ready_print_page.emergency_imgbrn = lv_imgbtn_label_align_create(mks_printer.user_src, "F:/png_emergency.bin",main_menu.emergency ,LV_ALIGN_IN_BOTTOM_LEFT, READY_IMGBTN_FIRST_X + READY_IMGBTN_X + READY_IMGBTN_W, READY_IMGBTN_FIRST_Y + READY_IMGBTN_H*2 + READY_IMGBTN_Y*2,10, -5, event_handler, ID_EMER);
  
  // ready_print_page.print_imgbrn = lv_imgbtn_label_align_create(mks_printer.user_src, "F:/ready_png_printing.bin", main_menu.print,LV_ALIGN_IN_BOTTOM_LEFT, 2, 142+3+2+30,10, 0, event_handler, ID_PRINT);
  // ready_print_page.tool_imgbrn = lv_imgbtn_label_align_create(mks_printer.user_src, "F:/ready_png_tool.bin", main_menu.tool,LV_ALIGN_IN_BOTTOM_LEFT, 2+117+2, 142+3+2+30,10, 0, event_handler, ID_TOOL);
  lv_refr_now(lv_refr_get_disp_refreshing());


  // static lv_style_t img_style;
  // lv_style_copy(&img_style, &lv_style_scr);
  // img_style.image.color = LV_COLOR_BLACK;
  // img_style.image.intense = 255;
  // lv_img_set_style(ready_print_page.move_imgbrn, LV_IMG_STYLE_MAIN, &img_style);
  
  lv_imgbtn_set_src_both(ready_print_page.print_imgbrn,  "F:/png_print.bin");
  lv_imgbtn_set_src_both(ready_print_page.tool_imgbrn,  "F:/png_tool.bin");
  lv_imgbtn_set_src_both(ready_print_page.move_imgbrn,  "F:/png_move.bin");
  lv_imgbtn_set_src_both(ready_print_page.filament_imgbrn,  "F:/png_cor_filament.bin");
  lv_imgbtn_set_src_both(ready_print_page.emergency_imgbrn,  "F:/png_emergency.bin");

  lv_refr_now(lv_refr_get_disp_refreshing());

  ready_print_page.ex1_temp_label =  lv_label_align_img(mks_printer.user_src, "F:/png_ready_ex.bin", "0°C",LV_ALIGN_OUT_RIGHT_MID,READY_IMG_X, READY_IMG_Y1,5, 0,&User_style.my_text_white_style);
  ready_print_page.bed_temp_label =  lv_label_align_img(mks_printer.user_src, "F:/png_ready_bed.bin", "0°C",LV_ALIGN_OUT_RIGHT_MID,READY_IMG_X, READY_IMG_Y2,5, 2,&User_style.my_text_white_style);

  // lv_imgbtn_label_align_create(lv_obj_t *par, const char *img, const char *text,lv_align_t align, lv_coord_t x, lv_coord_t y,lv_coord_t label_x, lv_coord_t label_y, lv_event_cb_t cb, const int id);
  // lv_imgbtn_set_src_both(lv_obj_t *imgbtn, const void *src);
  // btn = lv_btn_create_back(scr, 10, 10, 100, 50, event_handler_temp, 1);
  // lv_btn_set_style(btn,LV_BTN_STYLE_REL,&User_style.btn2_bg_style);
  // lv_btn_set_style(btn,LV_BTN_STYLE_PR,&User_style.btn2_bg_style);
  // label = lv_label_create(btn,"hello");
  // lv_label_set_style(label,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);

  // btn = lv_big_button_create(scr, "F:/ready_png_move.bin", main_menu.tool, 10, 10, event_handler, ID_TOOL);
  // lv_imgbtn_align_create(scr, "F:/ready_png_move.bin", LV_ALIGN_IN_TOP_LEFT, 10, 40, event_handler, ID_TOOL);
  // // lv_imgbtn_label_align_create(scr, "F:/ready_png_move.bin", "hello",LV_ALIGN_IN_BOTTOM_LEFT, 5, 40,10, 0, event_handler, ID_TOOL);
  // lv_title_create(scr,"tool",240, 30);
  // lv_img_align_label(scr, "F:/preheat_bed.bin", "hai",LV_ALIGN_IN_RIGHT_MID, 10, 250,50,0,&User_style.text_black_style);
  // lv_btn_align_create(scr,0,30,LV_ALIGN_CENTER,event_handler,ID_TOOL,&User_style.btn1_bg_style);
  // lv_my_title_create(scr,"hello",0,0);
  
  
    // lv_big_button_create(scr, "F:/ready_png_preheat.bin", main_menu.tool, 10, 130, event_handler, ID_TOOL);

  
    // lv_btn_create_back(scr, 10, 10, 20, 20, event_handler, 1);
// #ifdef USE_NEW_LVGL_CONF
//     lv_big_button_create(mks_ui.src_main, "F:/bmp_tool.bin", main_menu.tool, 20, 90, event_handler, ID_TOOL);
//     lv_big_button_create(mks_ui.src_main, "F:/bmp_set.bin", main_menu.set, 180, 90, event_handler, ID_SET);
//     lv_big_button_create(mks_ui.src_main, "F:/bmp_printing.bin", main_menu.print, 340, 90, event_handler, ID_PRINT);
// #else
 
//     lv_big_button_create(scr, "F:/bmp_tool.bin", main_menu.tool, 20, 90, event_handler, ID_TOOL);
//     lv_big_button_create(scr, "F:/bmp_set.bin", main_menu.set, 180, 90, event_handler, ID_SET);
//     lv_big_button_create(scr, "F:/bmp_printing.bin", main_menu.print, 340, 90, event_handler, ID_PRINT);
// #endif
//     // Monitoring
//     #if HAS_HOTEND
// #ifdef USE_NEW_LVGL_CONF
//       buttonExt1 = lv_big_button_create(mks_ui.src_main, "F:/bmp_ext1_state.bin", " ", 20, ICON_POS_Y, event_handler, ID_INFO_EXT);
// #else
//       buttonExt1 = lv_big_button_create(scr, "F:/bmp_ext1_state.bin", " ", 20, ICON_POS_Y, event_handler, ID_INFO_EXT);
// #endif
//     #endif
//     #if HAS_MULTI_HOTEND
//       buttonExt2 = lv_big_button_create(scr, "F:/bmp_ext2_state.bin", " ", 180, ICON_POS_Y, event_handler, ID_INFO_EXT);
//     #endif
//     #if HAS_HEATED_BED
// #ifdef USE_NEW_LVGL_CONF
//       buttonBedstate = lv_big_button_create(mks_ui.src_main, "F:/bmp_bed_state.bin", " ", TERN(HAS_MULTI_HOTEND, 271, 210), ICON_POS_Y, event_handler, ID_INFO_BED);
// #else
//       buttonBedstate = lv_big_button_create(scr, "F:/bmp_bed_state.bin", " ", TERN(HAS_MULTI_HOTEND, 340, 210), ICON_POS_Y, event_handler, ID_INFO_BED);
// #endif
//     #endif

// #ifdef USE_NEW_LVGL_CONF
//     TERN_(HAS_HOTEND, labelExt1 = lv_label_create_empty(mks_ui.src_main));
//     TERN_(HAS_MULTI_HOTEND, labelExt2 = lv_label_create_empty(mks_ui.src_main));
//     TERN_(HAS_HEATED_BED, labelBed = lv_label_create_empty(mks_ui.src_main));
//     TERN_(HAS_FAN, labelFan = lv_label_create_empty(mks_ui.src_main));
// #else
//     TERN_(HAS_HOTEND, labelExt1 = lv_label_create_empty(scr));
//     TERN_(HAS_MULTI_HOTEND, labelExt2 = lv_label_create_empty(scr));
//     TERN_(HAS_HEATED_BED, labelBed = lv_label_create_empty(scr));
//     TERN_(HAS_FAN, labelFan = lv_label_create_empty(scr));
// #endif
    lv_temp_refr();
  lv_imgbtn_set_src_both(ready_print_page.preheat_imgbrn,  "F:/png_cur_for_preheat.bin");
  lv_imgbtn_set_src_both(ready_print_page.print_imgbrn,  "F:/png_print.bin");
  }

  #if ENABLED(TOUCH_SCREEN_CALIBRATION)
    // If calibration is required, let's trigger it now, handles the case when there is default value in configuration files
    if (!touch_calibration.calibration_loaded()) {
      lv_clear_ready_print();
      lv_draw_touch_calibration_screen();
    }
  #endif
}

void lv_temp_refr() {
  #if HAS_HOTEND
    // sprintf(public_buf_l, printing_menu.temp1, thermalManager.wholeDegHotend(0), thermalManager.degTargetHotend(0));
    sprintf(public_buf_l, "%d°C", thermalManager.wholeDegHotend(0));
    lv_label_set_text(ready_print_page.ex1_temp_label, public_buf_l);
    // lv_obj_align(ready_print_page.ex1_temp_label, buttonExt1, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
  #endif
  #if HAS_MULTI_HOTEND
    sprintf(public_buf_l, printing_menu.temp1, thermalManager.wholeDegHotend(1), thermalManager.degTargetHotend(1));
    lv_label_set_text(labelExt2, public_buf_l);
    lv_obj_align(labelExt2, buttonExt2, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
  #endif
  #if HAS_HEATED_BED
    // sprintf(public_buf_l, printing_menu.bed_temp, thermalManager.wholeDegBed(), thermalManager.degTargetBed());
    sprintf(public_buf_l, "%d°C", thermalManager.wholeDegBed());
    lv_label_set_text(ready_print_page.bed_temp_label, public_buf_l);
    // lv_obj_align(labelBed, buttonBedstate, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
  #endif
  // #if HAS_FAN
  //   sprintf_P(public_buf_l, PSTR("%d%%"), (int)thermalManager.fanSpeedPercent(0));
  //   lv_label_set_text(labelFan, public_buf_l);
  //   lv_obj_align(labelFan, buttonFanstate, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
  // #endif
}

void lv_clear_ready_print() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
#ifdef USE_NEW_LVGL_CONF
  lv_obj_clean(mks_ui.src_main);
#else
  lv_obj_clean(mks_printer.user_src);
#endif
}

#endif // HAS_TFT_LVGL_UI
