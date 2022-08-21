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

#include "draw_ui.h"
#include <lv_conf.h>

#include "../../../gcode/queue.h"
#include "../../../gcode/gcode.h"
#include "../../../inc/MarlinConfig.h"

#if ENABLED(EEPROM_SETTINGS)
  #include "../../../module/settings.h"
#endif

#if HAS_BED_PROBE
  #include "../../../module/probe.h"
#endif
extern feedRate_t feedrate_mm_s;
extern lv_group_t *g;
extern float z_offset_add;
float z_offset_step = 0.01;
static lv_obj_t *scr;
bool baby_step_set_data = false;
static lv_obj_t *labelV, *buttonV, *zOffsetText;
static lv_obj_t *test_speed;

enum {
  ID_BABY_STEP_X_P = 1,
  ID_BABY_STEP_X_N,
  ID_BABY_STEP_Y_P,
  ID_BABY_STEP_Y_N,
  ID_BABY_STEP_Z_P,
  ID_BABY_STEP_Z_N,
  ID_BABY_STEP_DIST,
  ID_BABY_STEP_001,
  ID_BABY_STEP_01,
  ID_BABY_STEP_1,
  ID_BABY_STEP_RETURN
};
typedef struct{
  lv_obj_t *z_add;
  lv_obj_t *z_dec;
  lv_obj_t *z_offset_label;
  lv_obj_t *z_offset_scr;
  lv_obj_t *z_step001;
  lv_obj_t *z_step01;
  lv_obj_t *z_step1;
  lv_obj_t *back_imgbtn;
  lv_style_t z_label_bg_style;
  lv_style_t z_label_style;
  lv_style_t z_step_style;
  lv_style_t z_step_re_style;
}BABY_STEP_PAGE;

BABY_STEP_PAGE baby_step_page;

static float babystep_dist  = 0.01;
static uint8_t has_adjust_z = 0;

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  // if (event != LV_EVENT_RELEASED) return;
  char baby_buf[30] = { 0 };
  char str_1[16];
  char buf[20];
    if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
        button_sound_end();
    switch (obj->mks_obj_id) {
      case ID_BABY_STEP_X_P:
        sprintf_P(baby_buf, PSTR("M290 X%s"), dtostrf(babystep_dist, 1, 3, str_1));
        gcode.process_subcommands_now_P(PSTR(baby_buf));
        has_adjust_z = 1;
        break;
      case ID_BABY_STEP_X_N:
        sprintf_P(baby_buf, PSTR("M290 X%s"), dtostrf(-babystep_dist, 1, 3, str_1));
        gcode.process_subcommands_now_P(PSTR(baby_buf));
        has_adjust_z = 1;
        break;
      case ID_BABY_STEP_Y_P:
        sprintf_P(baby_buf, PSTR("M290 Y%s"), dtostrf(babystep_dist, 1, 3, str_1));
        gcode.process_subcommands_now_P(PSTR(baby_buf));
        has_adjust_z = 1;
        break;
      case ID_BABY_STEP_Y_N:
        sprintf_P(baby_buf, PSTR("M290 Y%s"), dtostrf(-babystep_dist, 1, 3, str_1));
        gcode.process_subcommands_now_P(PSTR(baby_buf));
        has_adjust_z = 1;
        break;
      case ID_BABY_STEP_Z_P:
      if(gCfgItems.add_full) break;
        sprintf_P(baby_buf, PSTR("M290 Z%s"), dtostrf(babystep_dist, 1, 3, str_1));
        gcode.process_subcommands_now_P(PSTR(baby_buf));
        has_adjust_z = 1;
        if(z_offset_step < 0)
        z_offset_step = z_offset_step*-1;
        
        z_offset_add = z_offset_add + z_offset_step;
        if(z_offset_add > 10)
        {
          gCfgItems.add_full = true;
        }
        else
        {
          gCfgItems.add_full = false;
        }
        gCfgItems.del_full = false;
        sprintf_P(buf, PSTR("%s"), dtostrf(z_offset_add, 1, 2, str_1));
        lv_label_set_text(baby_step_page.z_offset_label, buf);
        gCfgItems.babystep_data = z_offset_add;
        // ZERO(buf);
        // ZERO(str_1);
        // sprintf_P(buf,PSTR("M851 Z%s"),dtostrf(z_offset_add, 1, 3, str_1));
        // gcode.process_subcommands_now_P(PSTR(buf));
        update_spi_flash();
        break;
        case ID_BABY_STEP_Z_N:
        if(gCfgItems.del_full) break;
        sprintf_P(baby_buf, PSTR("M290 Z%s"), dtostrf(-babystep_dist, 1, 3, str_1));
        gcode.process_subcommands_now_P(PSTR(baby_buf));
        has_adjust_z = 1;
        if(z_offset_step > 0)
        z_offset_step = z_offset_step*-1;
        z_offset_add = z_offset_add + z_offset_step;
        
        if(z_offset_add < -10)
        {
          gCfgItems.del_full = true;
        }
        else
        {
          gCfgItems.del_full = false;
        }
        gCfgItems.add_full = false;
        sprintf_P(buf, PSTR("%s"), dtostrf(z_offset_add, 1, 2, str_1));
        lv_label_set_text(baby_step_page.z_offset_label, buf);
        gCfgItems.babystep_data = z_offset_add;
        // ZERO(buf);
        // ZERO(str_1);
        // sprintf_P(buf,PSTR("M851 Z%s"),dtostrf(z_offset_add, 1, 3, str_1));
        // gcode.process_subcommands_now_P(PSTR(buf));
        update_spi_flash();
        break;
      case ID_BABY_STEP_001:
          babystep_dist = 0.01;
          z_offset_step = 0.01;
          lv_btn_set_style_both(baby_step_page.z_step001,&baby_step_page.z_step_re_style);
          lv_btn_set_style_both(baby_step_page.z_step01,&baby_step_page.z_step_style);
          //lv_btn_set_style_both(baby_step_page.z_step1,&baby_step_page.z_step_style);
      break;
      case ID_BABY_STEP_01:
          babystep_dist = 0.1;
          z_offset_step = 0.1;
            lv_btn_set_style_both(baby_step_page.z_step01,&baby_step_page.z_step_re_style);
            //lv_btn_set_style_both(baby_step_page.z_step1,&baby_step_page.z_step_style);
            lv_btn_set_style_both(baby_step_page.z_step001,&baby_step_page.z_step_style);
      break;
      case ID_BABY_STEP_1:
          babystep_dist = 1;
          z_offset_step = 1;
            //lv_btn_set_style_both(baby_step_page.z_step1,&baby_step_page.z_step_re_style);
            lv_btn_set_style_both(baby_step_page.z_step001,&baby_step_page.z_step_style);
            lv_btn_set_style_both(baby_step_page.z_step01,&baby_step_page.z_step_style);
      break;
      // case ID_BABY_STEP_DIST:
      //   if (ABS((int)(100 * babystep_dist)) == 1)
      //     babystep_dist = 0.05;
      //   else if (ABS((int)(100 * babystep_dist)) == 5)
      //     babystep_dist = 0.1;
      //   else
      //     babystep_dist = 0.01;
      //   disp_baby_step_dist();
      //   break;
      case ID_BABY_STEP_RETURN:
        if (has_adjust_z == 1) {
          TERN_(EEPROM_SETTINGS, (void)settings.save());
          has_adjust_z = 0;
        }
        clear_cur_ui();
        draw_return_ui();
        break;
    }
  }
}
void lv_draw_baby_stepping() {
  char buf[20];
  char str_1[16];

  // lv_screen_distribution(BABY_STEP_UI);
  lv_screen_distribution(BABY_STEP_UI); 
  
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(BABY_STEP_UI),TITLE_LABEL_X);
  // mks_printer.user_src1 = lv_title_create(mks_printer.user_src);
  // lv_label_align_create(mks_printer.user_src1,printing_menu.z_offset,50, 0,LV_ALIGN_IN_LEFT_MID,  &User_style.text_white_style);

  lv_refr_now(lv_refr_get_disp_refreshing());
  lv_style_copy(&baby_step_page.z_step_style, &lv_style_scr);
  baby_step_page.z_step_style.body.main_color = lv_color_make(0xb5,0xb5,0xb5);
  baby_step_page.z_step_style.body.grad_color = lv_color_make(0xb5,0xb5,0xb5);
  baby_step_page.z_step_style.body.radius = 5;

  lv_style_copy(&baby_step_page.z_step_re_style, &lv_style_scr);
  baby_step_page.z_step_re_style.body.main_color = LV_COLOR_MAKE(0x41,0x85,0xf7);//LV_COLOR_MAKE(0x41,0x85,0xf7);
  baby_step_page.z_step_re_style.body.grad_color = LV_COLOR_MAKE(0x41,0x85,0xf7);//LV_COLOR_MAKE(0x41,0x85,0xf7);
  baby_step_page.z_step_re_style.body.radius = 5;

  // baby_step_page.back_imgbtn = lv_imgbtn_align_create(mks_printer.user_src1, "F:/title_return.bin", LV_ALIGN_IN_LEFT_MID, 0, 0, event_handler, ID_BABY_STEP_RETURN);
  baby_step_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_back.bin", TITLE_BACK_X, TITLE_BACK_Y, event_handler, ID_BABY_STEP_RETURN);

  baby_step_page.z_add = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_baby_z_add.bin", LV_ALIGN_IN_TOP_LEFT, 35, 345, event_handler, ID_BABY_STEP_Z_P);
  baby_step_page.z_dec = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_baby_z_dec.bin", LV_ALIGN_IN_TOP_LEFT, 210, 345, event_handler, ID_BABY_STEP_Z_N);

  
  #if 0
  baby_step_page.z_step1 = lv_button_btn_create(mks_printer.user_src, 34, 119, 82, 60, event_handler, ID_BABY_STEP_1);
  baby_step_page.z_step01 = lv_button_btn_create(mks_printer.user_src, 118, 119, 82, 60, event_handler, ID_BABY_STEP_01);
  #else
  baby_step_page.z_step01 = lv_button_btn_create(mks_printer.user_src, 34, 119, 82, 60, event_handler, ID_BABY_STEP_01);
  #endif
  baby_step_page.z_step001 = lv_button_btn_create(mks_printer.user_src, 204, 119, 82, 60, event_handler, ID_BABY_STEP_001);
  
  #if 0
  if(babystep_dist*100 == 100)
    lv_btn_set_style_both(baby_step_page.z_step1,&baby_step_page.z_step_re_style);
  else
    lv_btn_set_style_both(baby_step_page.z_step1,&baby_step_page.z_step_style);
  #endif

  if(babystep_dist*100 == 10)
    lv_btn_set_style_both(baby_step_page.z_step01,&baby_step_page.z_step_re_style);
  else
    lv_btn_set_style_both(baby_step_page.z_step01,&baby_step_page.z_step_style);

  if(babystep_dist*100 == 1)
    lv_btn_set_style_both(baby_step_page.z_step001,&baby_step_page.z_step_re_style);
  else
    lv_btn_set_style_both(baby_step_page.z_step001,&baby_step_page.z_step_style);


  //lv_label_align_create(baby_step_page.z_step1,"1",0, 0,LV_ALIGN_CENTER,  &User_style.text_white_style);
  lv_label_align_create(baby_step_page.z_step01,"0.1",0, 0,LV_ALIGN_CENTER,  &User_style.text_white_style);
  lv_label_align_create(baby_step_page.z_step001,"0.01",0, 0,LV_ALIGN_CENTER,  &User_style.text_white_style);

  lv_style_copy(&baby_step_page.z_label_bg_style, &lv_style_scr);
  baby_step_page.z_label_bg_style.body.main_color = LV_COLOR_WHITE;
  baby_step_page.z_label_bg_style.body.grad_color = LV_COLOR_WHITE;
  baby_step_page.z_label_bg_style.body.border.color = lv_color_make(0X62,0X62,0X62);
  baby_step_page.z_label_bg_style.body.border.width = 3;

  baby_step_page.z_label_bg_style.body.radius = 10;
  // baby_step_page.z_label_bg_style.body.border.width = 3;
  // baby_step_page.z_label_bg_style.body.border.color = LV_COLOR_BLACK;
  // baby_step_page.z_label_bg_style.text.font = &lv_font_roboto_28;
  // baby_step_page.z_label_bg_style.text.color = LV_COLOR_BLACK;

  lv_style_copy(&baby_step_page.z_label_style, &lv_style_scr);
  baby_step_page.z_label_style.body.main_color = LV_COLOR_WHITE;
  baby_step_page.z_label_style.body.grad_color = LV_COLOR_WHITE;
  baby_step_page.z_label_style.text.font = &lv_font_roboto_28;
  baby_step_page.z_label_style.text.color = LV_COLOR_BLACK;

  baby_step_page.z_offset_scr = lv_obj_create(mks_printer.user_src,NULL);
  lv_obj_set_pos(baby_step_page.z_offset_scr, 30, 207);
  lv_obj_set_size(baby_step_page.z_offset_scr,261,115);
  lv_obj_set_style(baby_step_page.z_offset_scr,&baby_step_page.z_label_bg_style);

  // sprintf_P(buf, PSTR("%s"), dtostrf(probe.offset.z, 1, 3, str_1));
  sprintf_P(buf, PSTR("%s"), dtostrf(z_offset_add, 1, 2, str_1));
  baby_step_page.z_offset_label = lv_label_align_create(baby_step_page.z_offset_scr,buf,0, 0,LV_ALIGN_CENTER,  &User_style.text28_black_style);

  
  // sprintf_P(buf, PSTR("%s"), dtostrf(feedrate_mm_s, 1, 2, str_1));
  // test_speed = lv_label_create_empty(mks_printer.user_src);
  // lv_label_set_text(test_speed, buf);
  // lv_label_set_style(test_speed, LV_LABEL_STYLE_MAIN, &User_style.text28_black_style);
  // lv_obj_set_pos(test_speed, 2, 57);

  // lv_obj_align( baby_step_page.z_offset_label,baby_step_page.z_offset_scr ,LV_ALIGN_CENTER,0,0);
  // lv_obj_set_style(baby_step_page.z_offset_label,&baby_step_page.z_label_style);

  // // scr = lv_screen_create(BABY_STEP_UI);
  // lv_big_button_create(scr, "F:/bmp_xAdd.bin", move_menu.x_add, INTERVAL_V, titleHeight, event_handler, ID_BABY_STEP_X_P);
  // lv_big_button_create(scr, "F:/bmp_xDec.bin", move_menu.x_dec, INTERVAL_V, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_BABY_STEP_X_N);
  // lv_big_button_create(scr, "F:/bmp_yAdd.bin", move_menu.y_add, BTN_X_PIXEL + INTERVAL_V * 2, titleHeight, event_handler, ID_BABY_STEP_Y_P);
  // lv_big_button_create(scr, "F:/bmp_yDec.bin", move_menu.y_dec, BTN_X_PIXEL + INTERVAL_V * 2, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_BABY_STEP_Y_N);
  // lv_big_button_create(scr, "F:/bmp_zAdd.bin", move_menu.z_add, BTN_X_PIXEL * 2 + INTERVAL_V * 3, titleHeight, event_handler, ID_BABY_STEP_Z_P);
  // lv_big_button_create(scr, "F:/bmp_zDec.bin", move_menu.z_dec, BTN_X_PIXEL * 2 + INTERVAL_V * 3, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_BABY_STEP_Z_N);
  // buttonV = lv_imgbtn_create(scr, nullptr, BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_BABY_STEP_DIST);
  // labelV = lv_label_create_empty(buttonV);
  // #if HAS_ROTARY_ENCODER
  //   if (gCfgItems.encoder_enable)
  //     lv_group_add_obj(g, buttonV);
  // #endif

  // lv_big_button_create(scr, "F:/bmp_return.bin", common_menu.text_back, BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_BABY_STEP_RETURN);

  // disp_baby_step_dist();

  // zOffsetText = lv_label_create(scr, 290, TITLE_YPOS, nullptr);
  // disp_z_offset_value();
  // send_m290();
}

// void disp_baby_step_dist() {
//   if ((int)(100 * babystep_dist) == 1)
//     lv_imgbtn_set_src_both(buttonV, "F:/bmp_baby_move0_01.bin");
//   else if ((int)(100 * babystep_dist) == 5)
//     lv_imgbtn_set_src_both(buttonV, "F:/bmp_baby_move0_05.bin");
//   else if ((int)(100 * babystep_dist) == 10)
//     lv_imgbtn_set_src_both(buttonV, "F:/bmp_baby_move0_1.bin");

//   if (gCfgItems.multiple_language) {
//     if ((int)(100 * babystep_dist) == 1) {
//       lv_label_set_text(labelV, move_menu.step_001mm);
//       lv_obj_align(labelV, buttonV, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
//     }
//     else if ((int)(100 * babystep_dist) == 5) {
//       lv_label_set_text(labelV, move_menu.step_005mm);
//       lv_obj_align(labelV, buttonV, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
//     }
//     else if ((int)(100 * babystep_dist) == 10) {
//       lv_label_set_text(labelV, move_menu.step_01mm);
//       lv_obj_align(labelV, buttonV, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
//     }
//   }
// }
void send_m290()
{
    char buf[20];
    char str_1[16];
    ZERO(buf);
    ZERO(str_1);
    if(baby_step_set_data)
    {
      sprintf_P(buf,PSTR("M290 Z%s"),dtostrf(z_offset_add, 1, 3, str_1));
      gcode.process_subcommands_now_P(PSTR(buf));
      baby_step_set_data = false;
    }
}
void disp_z_offset_value() {
  char buf[20];
  char str_1[16];

  #if HAS_BED_PROBE
    // sprintf_P(buf, PSTR("Offset Z: %s mm"), dtostrf(probe.offset.z, 1, 3, str_1));
    sprintf_P(buf, PSTR("%s"), dtostrf(z_offset_add, 1, 2, str_1));
  #else
    sprintf_P(buf, PSTR("%s"), dtostrf(z_offset_add, 1, 2, str_1));
  #endif
  lv_label_set_text(baby_step_page.z_offset_label, buf);
}

void lv_clear_baby_stepping() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_clean(mks_printer.user_src);
}

void disp_test_speed()
{
  char buf[20];
  char str_1[20];
  
  sprintf_P(buf, PSTR("%s"), dtostrf(feedrate_mm_s, 1, 2, str_1));
  lv_label_set_text(test_speed, buf);
}

#endif // HAS_TFT_LVGL_UI
