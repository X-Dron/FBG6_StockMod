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

#include "../../../MarlinCore.h" // for marlin_state
#include "../../../module/temperature.h"
#include "../../../module/motion.h"
#include "../../../sd/cardreader.h"
#include "../../../gcode/queue.h"
#include "../../../gcode/gcode.h"
#include "../../../inc/MarlinConfig.h"
#include "../../../module/planner.h"        // for synchronize

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../../../feature/powerloss.h"
#endif

#if BOTH(LCD_SET_PROGRESS_MANUALLY, USE_M73_REMAINING_TIME)
  #include "../../marlinui.h"
#endif

extern lv_group_t *g;
extern uint8_t sel_id;

typedef struct{
  lv_obj_t *bar;
  lv_obj_t *bar_value_label;
  lv_obj_t *name_label;
  lv_obj_t *ex1_label;
  lv_obj_t *ex2_label;
  lv_obj_t *bed_label;
  lv_obj_t *fan_label;
  lv_obj_t *speed_label;
  lv_obj_t *filament_label;
  lv_obj_t *z_label;
  lv_obj_t *layer_label;
  lv_obj_t *z_offset_label;

  lv_obj_t *title_time_label;
  lv_obj_t *title_speed_label;
  lv_obj_t *title_label;

  lv_obj_t *title_time_img;
  lv_obj_t *title_speed_img;

  lv_obj_t *state_imgbtn;
  lv_obj_t *option_imgbtn;
  lv_obj_t *stop_imgbtn;
  // lv_obj_t *back_imgbtn;

  lv_obj_t *ex1_imgbtn;
  lv_obj_t *ex2_imgbtn;
  lv_obj_t *bed_imgbtn;
  lv_obj_t *fan_imgbtn;
  lv_obj_t *speed_imgbtn;
  lv_obj_t *filament_imgbtn;
  lv_obj_t *z_offset_imgbtn;
  lv_obj_t *layer_imgbtn;

  lv_style_t print_bar_bg_style;
  lv_style_t print_bar_indic_style;
  lv_style_t gcode_pre_block_style;

}PRINTING_UI_PAGE;

PRINTING_UI_PAGE printing_page;

#if HAS_MULTI_EXTRUDER
  static lv_obj_t *labelExt2;
  static lv_obj_t *buttonExt2;
#endif

#if HAS_HEATED_BED
  // static lv_obj_t* printing_page.bed_label;
  // static lv_obj_t* buttonBedstate;
#endif

enum {
  ID_PAUSE = 1,
  ID_STOP,
  ID_OPTION,
  ID_EX_TEMP,
  ID_BED_TEMP,
  ID_BABYSTEP,
  ID_SPEED_TEMP,
  ID_EX_SPEED_TEMP,
  ID_FAN_TEMP,
  ID_LAYER_STOP,
  ID_MORE,
  ID_FAN
};

bool once_flag; // = false
extern bool flash_preview_begin, default_preview_flg, gcode_preview_over;
extern uint32_t To_pre_view;
uint8_t layer_stop_flag = LAYER_STOP_NO_TRIGGERED;

void paused_print()
{
   #if ENABLED(SDSUPPORT)
        card.pauseSDPrint();
        stop_print_time();
        uiCfg.print_state = PAUSING;
   #endif
}
static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
    button_sound_end();

  if (gcode_preview_over) return;
  switch (obj->mks_obj_id) {
    case ID_PAUSE:
      if (uiCfg.print_state == WORKING) {
        #if ENABLED(SDSUPPORT)
          card.pauseSDPrint();
          stop_print_time();
          uiCfg.print_state = PAUSING;
        #endif
        lv_imgbtn_set_src_both(printing_page.state_imgbtn, "F:/png_continue.bin");
      }
      else if (uiCfg.print_state == PAUSED) {
        if(layer_stop_flag == LAYER_STOP_TRIGGERED)
        {
          layer_stop_flag = LAYER_STOP_CANNOT_TRIGGER;
        }
        uiCfg.print_state = RESUMING;
        lv_imgbtn_set_src_both(printing_page.state_imgbtn, "F:/png_suspend.bin");
      }
      break;
    case ID_MORE:
      disp_pre_cover_up();
      lv_clear_printing();
      lv_draw_more();
    break;
    case ID_STOP:
      disp_pre_cover_up();
      lv_clear_printing();
      lv_draw_dialog(DIALOG_TYPE_STOP);
      break;
    case ID_EX_TEMP:
        disp_pre_cover_up();
        lv_clear_printing();
        mks_draw_simple_num_kb(0,EX1_TEMP);
      break;
      case ID_BED_TEMP:
        lv_clear_printing();
        disp_pre_cover_up();
        mks_draw_simple_num_kb(0,BED_TEMP);
      break;
    case ID_FAN_TEMP:
        disp_pre_cover_up();
        lv_clear_printing();
        mks_draw_simple_num_kb(0,FAN_SPEED);
      break;
    case ID_SPEED_TEMP:
        disp_pre_cover_up();
        disp_pre_cover_up();
        lv_clear_printing();
        mks_draw_simple_num_kb(0,PRINT_SPEED);
      break;
    case ID_EX_SPEED_TEMP:
        disp_pre_cover_up();
        lv_clear_printing();
        mks_draw_simple_num_kb(0,EX_SPEED);
      break;
    case ID_BABYSTEP:
      disp_pre_cover_up();
      lv_clear_printing();
      lv_draw_baby_stepping();
      break;
    case ID_LAYER_STOP:
      disp_pre_cover_up();
      lv_clear_printing();
      lv_draw_layer_stop();
      break;
  }
  }
}

void lv_draw_printing() {
  static lv_style_t printing_small_style, printing_text_style;
  lv_style_copy(&printing_small_style, &User_style.small_text_black_style);
  printing_small_style.text.font         = &DengXian_12;
  lv_style_copy(&printing_text_style, &User_style.my_text_black_style);
  printing_text_style.text.font         = &DengXian_16;

  disp_state_stack._disp_index = 0;
  ZERO(disp_state_stack._disp_state);
  lv_screen_distribution(PRINTING_UI);
  lv_refr_now(lv_refr_get_disp_refreshing());

  if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  lv_refr_now(lv_refr_get_disp_refreshing());

  // mks_printer.user_src = lv_title_offset_n_create(mks_printer.user_src);

  printing_page.title_time_img = lv_img_create(mks_printer.user_src,NULL);
  lv_img_set_src(printing_page.title_time_img,"F:/png_time.bin");
  lv_obj_set_pos(printing_page.title_time_img, 141, 226);

  // lv_style_copy(&printing_page.gcode_pre_block_style, &lv_style_plain_color);
  // printing_page.gcode_pre_block_style.body.main_color = LV_COLOR_WHITE;
  // printing_page.gcode_pre_block_style.body.grad_color = LV_COLOR_WHITE;
  // printing_page.gcode_pre_block_style.body.border.width = 2;
  // printing_page.gcode_pre_block_style.body.border.color = LV_COLOR_BLACK;

  // mks_printer.user_src2 = lv_scr_create(mks_printer.user_src, 13,90,205,210);
  // lv_obj_set_style(mks_printer.user_src2,&printing_page.gcode_pre_block_style);

  printing_page.title_time_label = lv_label_align(mks_printer.user_src,printing_page.title_time_label,printing_page.title_time_img , "00:00:00",LV_ALIGN_OUT_RIGHT_MID,5, 8, &printing_small_style);

  printing_page.z_label = lv_label_align_create(mks_printer.user_src,"Z 0.00", 245, 234,LV_ALIGN_IN_TOP_LEFT, &printing_small_style);
  
  // printing_page.title_label = lv_label_align_create(mks_printer.user_src,"printing", 10, -2,LV_ALIGN_IN_LEFT_MID, &User_style.small_text_black_style);

  // Create image buttons
  printing_page.ex1_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_e_temp.bin",230, 313,  event_handler, ID_EX_TEMP);

  #if HAS_MULTI_EXTRUDER
    buttonExt2 = lv_imgbtn_create(scr, "F:/bmp_ext2_state.bin", 350, 136, event_handler, ID_TEMP_EXT);
  #endif

  #if HAS_HEATED_BED
      printing_page.bed_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_bed_temp.bin",30, 313, event_handler, ID_BED_TEMP);
  #endif


  // printing_page.fan_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_fan_temp.bin",253, 244, event_handler, ID_FAN_TEMP);

  // printing_page.fan_btn = lv_imgbtn_create(mks_printer.user_src, "F:/print_wind.bin",180, 90, event_handler, ID_FAN_TEMP);

  // lv_obj_t *buttonTime = lv_img_create(scr, nullptr);
  // lv_img_set_src(buttonTime, "F:/bmp_time_state.bin");
  // lv_obj_set_pos(buttonTime, 206, 86);

  // buttonZpos = lv_imgbtn_create(scr, "F:/bmp_zpos_state.bin", 350, 86, event_handler, ID_BABYSTEP);
  // printing_page.z_offset_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_z_offset.bin",247, 324, event_handler, ID_BABYSTEP);
  // printing_page.filament_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_ex_factor.bin",257, 163, event_handler, ID_EX_SPEED_TEMP);
  printing_page.speed_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_speed_factor.bin",132, 313, event_handler, ID_SPEED_TEMP);
  // printing_page.layer_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_layer.bin",172, 315, event_handler, ID_LAYER_STOP);


  printing_page.ex1_label = lv_label_create_empty(mks_printer.user_src);
  lv_label_set_text(printing_page.ex1_label, "0°C/0°C");
  lv_obj_align(printing_page.ex1_label, printing_page.ex1_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  lv_obj_set_style(printing_page.ex1_label,&printing_text_style);

  // printing_page.filament_label = lv_label_create_empty(mks_printer.user_src);
  // lv_label_set_text(printing_page.filament_label, "100%");
  // lv_obj_align(printing_page.filament_label, printing_page.filament_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  // lv_obj_set_style(printing_page.filament_label,&User_style.small_text_black_style);

  // printing_page.fan_label = lv_label_create_empty(mks_printer.user_src);
  // lv_label_set_text(printing_page.fan_label, "100%");
  // lv_obj_align(printing_page.fan_label, printing_page.fan_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  // lv_obj_set_style(printing_page.fan_label,&User_style.small_text_black_style);

  // printing_page.z_offset_label = lv_label_create_empty(mks_printer.user_src);
  // lv_label_set_text(printing_page.z_offset_label, move_menu.zoffset);
  // if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  // lv_obj_align(printing_page.z_offset_label, printing_page.z_offset_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, -15, 3);
  // else
  // lv_obj_align(printing_page.z_offset_label, printing_page.z_offset_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 5, 3);
  // lv_obj_set_style(printing_page.z_offset_label,&User_style.small_text_black_style);

  // printing_page.layer_label = lv_label_create_empty(mks_printer.user_src);
  // lv_label_set_text(printing_page.layer_label, move_menu.layout_stop);
  //   if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  // lv_obj_align(printing_page.layer_label, printing_page.layer_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, -15, 3);
  // else
  // lv_obj_align(printing_page.layer_label, printing_page.layer_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 12, 3);
  // lv_obj_set_style(printing_page.layer_label,&User_style.small_text_black_style);

  printing_page.speed_label = lv_label_create_empty(mks_printer.user_src);
  lv_label_set_text(printing_page.speed_label, "150%");
  lv_obj_align(printing_page.speed_label, printing_page.speed_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  lv_obj_set_style(printing_page.speed_label,&printing_text_style);

  printing_page.bed_label = lv_label_create_empty(mks_printer.user_src);
  lv_label_set_text(printing_page.bed_label, "0°C/0°C");
  lv_obj_align(printing_page.bed_label, printing_page.bed_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  lv_obj_set_style(printing_page.bed_label,&printing_text_style);
  // printing_page.ex1_label = lv_label_align(mks_printer.user_src,printing_page.ex1_label,printing_page.ex1_imgbtn , "0/0",LV_ALIGN_OUT_BOTTOM_MID,0, 3, &User_style.small_text_black_style);
  // printing_page.filament_label = lv_label_align(mks_printer.user_src,printing_page.filament_label,printing_page.filament_imgbtn , "100%",LV_ALIGN_OUT_BOTTOM_MID,0, 3, &User_style.small_text_black_style);
  // printing_page.fan_label = lv_label_align(mks_printer.user_src,printing_page.fan_label,printing_page.fan_imgbtn , "100%",LV_ALIGN_OUT_BOTTOM_MID,0, 3, &User_style.small_text_black_style);
  // printing_page.z_offset_label = lv_label_align(mks_printer.user_src,printing_page.z_offset_label,printing_page.z_offset_imgbtn , "Z offset",LV_ALIGN_OUT_BOTTOM_MID,0, 3, &User_style.small_text_black_style);
  // printing_page.layer_label = lv_label_align(mks_printer.user_src,printing_page.layer_label,printing_page.layer_imgbtn , "Layer stop",LV_ALIGN_OUT_BOTTOM_MID,0, 3, &User_style.small_text_black_style);
  // printing_page.speed_label = lv_label_align(mks_printer.user_src,printing_page.speed_label,printing_page.speed_imgbtn , "150%",LV_ALIGN_OUT_BOTTOM_MID,0, 3, &User_style.small_text_black_style);
  // printing_page.bed_label = lv_label_align(mks_printer.user_src,printing_page.bed_label,printing_page.bed_imgbtn , "0/0",LV_ALIGN_OUT_BOTTOM_MID,0, 3, &User_style.small_text_black_style);


  if (uiCfg.print_state == WORKING || uiCfg.print_state == RESUMING) 
    printing_page.state_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_suspend.bin", LV_ALIGN_IN_TOP_LEFT, 128, 399, event_handler, ID_PAUSE); //LV_ALIGN_IN_BOTTOM_RIGHT -58 -11
  else
    printing_page.state_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_continue.bin", LV_ALIGN_IN_TOP_LEFT, 128, 399, event_handler, ID_PAUSE);

  printing_page.stop_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_stop.bin", LV_ALIGN_IN_TOP_LEFT, 21, 399, event_handler, ID_STOP);
  
  printing_page.stop_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_more.bin", LV_ALIGN_IN_TOP_LEFT, 229, 395, event_handler, ID_MORE);

    if(gCfgItems.language == LANG_SIMPLE_CHINESE)
    lv_refr_now(lv_refr_get_disp_refreshing());
  // buttonPause  = lv_imgbtn_create(scr, uiCfg.print_state == WORKING ? "F:/bmp_pause.bin" : "F:/bmp_resume.bin", 5, 240, event_handler, ID_PAUSE);
  // buttonStop   = lv_imgbtn_create(scr, "F:/bmp_stop.bin", 165, 240, event_handler, ID_STOP);
  // buttonOperat = lv_imgbtn_create(scr, "F:/bmp_operate.bin", 325, 240, event_handler, ID_OPTION);

  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) {
      lv_group_add_obj(g, buttonPause);
      lv_group_add_obj(g, buttonStop);
      lv_group_add_obj(g, buttonOperat);
      lv_group_add_obj(g, buttonPause);
      lv_group_add_obj(g, buttonPause);
      lv_group_add_obj(g, buttonPause);
    }
  #endif

  // printing_page.ex1_label = lv_label_create(scr, 250, 146, nullptr);

  #if HAS_MULTI_EXTRUDER
    labelExt2 = lv_label_create(scr, 395, 146, nullptr);
  #endif

  #if HAS_HEATED_BED
    // printing_page.bed_label = lv_label_create(scr, 250, 196, nullptr);
  #endif

  // printing_page.fan_label  = lv_label_create(scr, 395, 196, nullptr);
  // labelTime = lv_label_create(scr, 250, 96, nullptr);
  // labelZpos = lv_label_create(scr, 395, 96, nullptr);

  // labelPause  = lv_label_create_empty(buttonPause);
  // labelStop   = lv_label_create_empty(buttonStop);
  // labelOperat = lv_label_create_empty(buttonOperat);

  // if (gCfgItems.multiple_language) {
  //   lv_label_set_text(labelPause, uiCfg.print_state == WORKING ? printing_menu.pause : printing_menu.resume);
  //   lv_obj_align(labelPause, buttonPause, LV_ALIGN_CENTER, 20, 0);

  //   lv_label_set_text(labelStop, printing_menu.stop);
  //   lv_obj_align(labelStop, buttonStop, LV_ALIGN_CENTER, 20, 0);

  //   lv_label_set_text(labelOperat, printing_menu.option);
  //   lv_obj_align(labelOperat, buttonOperat, LV_ALIGN_CENTER, 20, 0);
  // }
  printing_page.title_speed_img = lv_img_create(mks_printer.user_src,NULL);
  lv_img_set_src(printing_page.title_speed_img,"F:/png_speed_mm.bin");
  lv_obj_set_pos(printing_page.title_speed_img, 14, 227);
  lv_refr_now(lv_refr_get_disp_refreshing());

  printing_page.title_speed_label = lv_label_align(mks_printer.user_src,printing_page.title_speed_label,printing_page.title_speed_img , "0mm/s",LV_ALIGN_OUT_RIGHT_MID,5, 8, &printing_small_style);
  
  lv_style_copy(&printing_page.print_bar_bg_style, &lv_style_plain_color);
  printing_page.print_bar_bg_style.body.main_color = LV_COLOR_MAKE(238,238,238);
  printing_page.print_bar_bg_style.body.grad_color = LV_COLOR_MAKE(238,238,238);

  lv_style_copy(&printing_page.print_bar_indic_style,&lv_style_plain_color);
  printing_page.print_bar_indic_style.body.main_color = LV_COLOR_MAKE(0x42,0x85,0xF8);
  printing_page.print_bar_indic_style.body.grad_color = LV_COLOR_MAKE(0x42,0x85,0xF8);//#11DF81  //#4285F8
    // print_src.print_bar_indic_style.body.radius = 5;
  printing_page.print_bar_indic_style.body.padding.left = 0;
  printing_page.print_bar_indic_style.body.padding.top = 0;
  printing_page.print_bar_indic_style.body.padding.right = 0;
  printing_page.print_bar_indic_style.body.padding.bottom = 0;
  
  printing_page.bar = lv_bar_create(mks_printer.user_src, nullptr);
  lv_obj_set_pos(printing_page.bar, 0, 262);
  lv_obj_set_size(printing_page.bar, 320, 36);
  lv_bar_set_style(printing_page.bar, LV_BAR_STYLE_INDIC, &printing_page.print_bar_indic_style);
  lv_bar_set_style(printing_page.bar, LV_BAR_STYLE_BG, &printing_page.print_bar_bg_style);
  lv_bar_set_anim_time(printing_page.bar, 1000);
  lv_bar_set_value(printing_page.bar, 0, LV_ANIM_ON);
  printing_page.bar_value_label = lv_label_create_empty(printing_page.bar);
  lv_label_set_text(printing_page.bar_value_label, "0%");
  lv_obj_align(printing_page.bar_value_label, printing_page.bar, LV_ALIGN_IN_RIGHT_MID, -20, 0);

  // char buf[30];
  // ZERO(buf);
  // strcpy(buf,)
  // sprintf(buf,"%s",list_file.long_name[sel_id]);
  printing_page.name_label = lv_label_create_empty(printing_page.bar);
  char *cur_name = NULL;
  char buf[30];
  cur_name = strrchr(list_file.long_name[sel_id], '/');
  if(cur_name)
    sprintf(buf,"%s",cur_name);
  else
    sprintf(buf,"%s",list_file.long_name[sel_id]);

  lv_label_set_text(printing_page.name_label, buf);
  lv_obj_align(printing_page.name_label, printing_page.bar, LV_ALIGN_IN_LEFT_MID, 10, 3);
  if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  lv_obj_set_style(printing_page.name_label,&printing_text_style);
  else
  lv_obj_set_style(printing_page.name_label,&printing_text_style);


  lv_obj_set_style(printing_page.bar_value_label,&printing_text_style);

#if ENABLED(POWER_LOSS_RECOVERY)
 if(recoveryed_flag)
 {
   if (uiCfg.print_state == REPRINTING) {
      uiCfg.print_state = REPRINTED;
      lv_imgbtn_set_src_both(printing_page.state_imgbtn, "F:/png_suspend.bin");
      print_time.minutes = recovery.info.print_job_elapsed / 60;
      print_time.seconds = recovery.info.print_job_elapsed % 60;
      print_time.hours   = print_time.minutes / 60;
      }
 }
 #endif
  // bar1 = lv_bar_create(scr, nullptr);
  // lv_obj_set_pos(bar1, 205, 36);
  // lv_obj_set_size(bar1, 270, 40);
  // lv_bar_set_style(bar1, LV_BAR_STYLE_INDIC, &lv_bar_style_indic);
  // lv_bar_set_anim_time(bar1, 1000);
  // lv_bar_set_value(bar1, 0, LV_ANIM_ON);
  // bar1ValueText = lv_label_create_empty(bar1);
  // lv_label_set_text(bar1ValueText, "0%");
  // lv_obj_align(bar1ValueText, bar1, LV_ALIGN_CENTER, 0, 0);
  disp_print_mm();
  disp_bed_temp();
  // disp_fan_speed();
  // disp_filament_speed();
  disp_move_speed();
  disp_print_time();
  disp_Zpos();
  disp_ext_temp();
  lv_img_set_src(printing_page.title_time_img,"F:/png_time.bin");

}
void disp_pre_cover_up()
{
    mks_printer.user_src2 = lv_scr_create(mks_printer.user_src, 60,19,201,201);
    lv_obj_set_style(mks_printer.user_src2,&printing_page.gcode_pre_block_style);
}
void disp_status()
{
  if (uiCfg.print_state == PAUSED)
    lv_imgbtn_set_src_both(printing_page.state_imgbtn, "F:/png_continue.bin");
  // if (uiCfg.print_state == PAUSED)
  //   lv_imgbtn_set_src_both(printing_page.state_imgbtn, "F:/png_continue.bin");
}
void disp_print_mm()
{
  // memset(public_buf_l,0,sizeof(public_buf_l));
  dtostrf(feedrate_mm_s, 1, 2, public_buf_l);
  strcat(public_buf_l,"mm/s");
  // sprintf(public_buf_l, "%smm/s", public_buf_l);
  lv_label_set_text(printing_page.title_speed_label, public_buf_l);
}
void disp_print_time() {
  #if BOTH(LCD_SET_PROGRESS_MANUALLY, USE_M73_REMAINING_TIME)
    const uint32_t r = ui.get_remaining_time();
    sprintf_P(public_buf_l, PSTR("%02d:%02d R"), r / 3600, (r % 3600) / 60);
  #else
    sprintf_P(public_buf_l, PSTR("%d%d:%d%d:%d%d"), print_time.hours / 10, print_time.hours % 10, print_time.minutes / 10, print_time.minutes % 10, print_time.seconds / 10, print_time.seconds % 10);
  #endif
  lv_label_set_text(printing_page.title_time_label, public_buf_l);
}
void disp_Zpos() {
  char buff[20];
  dtostrf(current_position.z, 1, 3, public_buf_l);
  sprintf_P(buff, PSTR("Z %s"), public_buf_l);
  lv_label_set_text(printing_page.z_label, buff);
}

void disp_ext_temp() {

  sprintf(public_buf_l, "%d°C/%d°C", thermalManager.wholeDegHotend(0), thermalManager.degTargetHotend(0));  //printing_menu.temp1
  lv_label_set_text(printing_page.ex1_label, public_buf_l);
  lv_obj_align(printing_page.ex1_label, printing_page.ex1_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  #if HAS_MULTI_EXTRUDER
    sprintf(public_buf_l, printing_menu.temp1, thermalManager.wholeDegHotend(1), thermalManager.degTargetHotend(1));
    lv_label_set_text(printing_page.ex2_label, public_buf_l);
  #endif
}

void disp_bed_temp() {
  #if HAS_HEATED_BED
    sprintf(public_buf_l, "%d°C/%d°C", thermalManager.wholeDegBed(), thermalManager.degTargetBed());  //printing_menu.bed_temp
    lv_label_set_text(printing_page.bed_label, public_buf_l);
    lv_obj_align(printing_page.bed_label, printing_page.bed_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
  #endif
}
// void disp_fan_speed() {

//   sprintf_P(public_buf_l, PSTR("%d%%"), (int)thermalManager.fanSpeedPercent(0));
//   lv_label_set_text(printing_page.fan_label, public_buf_l);
//   lv_obj_align(printing_page.fan_label, printing_page.fan_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

// }

// void disp_filament_speed()
// {
//   sprintf_P(public_buf_l, PSTR("%d%%"), planner.flow_percentage[uiCfg.extruderIndex]);
//   lv_label_set_text(printing_page.filament_label, public_buf_l);
//   lv_obj_align(printing_page.filament_label, printing_page.filament_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

// }

void disp_move_speed()
{
  sprintf_P(public_buf_l, PSTR("%d%%"), feedrate_percentage);
  lv_label_set_text(printing_page.speed_label, public_buf_l);
  lv_obj_align(printing_page.speed_label, printing_page.speed_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
}
void reset_print_time() {
  print_time.hours   = 0;
  print_time.minutes = 0;
  print_time.seconds = 0;
  print_time.ms_10   = 0;
}

void start_print_time() { print_time.start = 1; }

void stop_print_time() { print_time.start = 0; }

void setProBarRate() {
  int rate;
  volatile long long rate_tmp_r;

  if (!gCfgItems.from_flash_pic) {
    #if ENABLED(SDSUPPORT)
      rate_tmp_r = (long long)card.getIndex() * 100;
    #endif
    rate = rate_tmp_r / gCfgItems.curFilesize;
  }
  else {
    #if ENABLED(SDSUPPORT)
      rate_tmp_r = (long long)card.getIndex();
    #endif
    rate = (rate_tmp_r - (PREVIEW_SIZE + To_pre_view)) * 100 / (gCfgItems.curFilesize - (PREVIEW_SIZE + To_pre_view));
  }

  if (rate <= 0) {
    uiCfg.print_progress = 0;

    return;
  }

  uiCfg.print_progress = rate;


  if (disp_state == PRINTING_UI) {
    lv_bar_set_value(printing_page.bar, rate, LV_ANIM_ON);
    sprintf_P(public_buf_l, "%d%%", rate);
    lv_label_set_text(printing_page.bar_value_label, public_buf_l);
    lv_obj_align(printing_page.bar_value_label, printing_page.bar, LV_ALIGN_IN_RIGHT_MID, -20, 0);

    // if (marlin_state == MF_SD_COMPLETE) {
    //   if (once_flag == 0) {
    //     stop_print_time();

    //     flash_preview_begin = false;
    //     default_preview_flg = false;
    //     lv_clear_printing();
    //     lv_draw_dialog(DIALOG_TYPE_FINISH_PRINT);

    //     once_flag = true;

    //     #if HAS_SUICIDE
    //       if (gCfgItems.finish_power_off) {
    //         gcode.process_subcommands_now_P(PSTR("M1001"));
    //         queue.inject_P(PSTR("M81"));
    //         marlin_state = MF_RUNNING;
    //       }
    //     #endif
    //   }
    // }
  }
}

void lv_clear_printing() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);

}

#endif // HAS_TFT_LVGL_UI
