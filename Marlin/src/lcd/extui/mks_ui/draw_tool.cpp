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
#include "../../../module/temperature.h"
#include "../../../inc/MarlinConfig.h"

extern lv_group_t *g;

#ifndef USE_NEW_LVGL_CONF
static lv_obj_t *scr;
#endif

#define tool_btn_w    155
#define tool_btn_h    108

#define tool_btn_y_first_offset    5
#define tool_btn_x_first_offset    4

#define tool_btn_y_offset    2
#define tool_btn_x_offset    2

typedef struct{

  lv_obj_t *level_imgbtn;
  lv_obj_t *tempsetting_imgbtn;
  lv_obj_t *wifi_imgbtn;
  lv_obj_t *advances_imgbtn;
  lv_obj_t *endstop_imgbtn;
  lv_obj_t *voice_imgbtn;
  lv_obj_t *language_imgbtn;
  lv_obj_t *about_imgbtn;

  lv_obj_t *level_label;
  lv_obj_t *tempsetting_label;
  lv_obj_t *wifi_label;
  lv_obj_t *advances_label;
  lv_obj_t *endstop_label;
  lv_obj_t *voice_label;
  lv_obj_t *language_label;
  lv_obj_t *about_label;


  lv_obj_t *back_imgbtn;
}TOOL_PAGE;

TOOL_PAGE tool_page;

enum {
  ID_T_PRE_HEAT = 1,
  ID_T_EXTRUCT,
  ID_T_VOICE,
  ID_T_ENDSTOP,
  ID_T_TEMPSETTING,
  ID_T_LEVELING,
  ID_T_LANGUAGE,
  ID_T_MORE,
  ID_T_TEMP,
  ID_T_WIFI,
  ID_T_ABOUT,
  ID_T_ADVANCES,
  ID_T_RETURN
};

#if ENABLED(MKS_TEST)
  extern uint8_t current_disp_ui;
#endif

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
      button_sound_end();
    // if (TERN1(AUTO_BED_LEVELING_BILINEAR, obj->mks_obj_id != ID_T_LEVELING))
    lv_clear_tool();
    switch (obj->mks_obj_id) {
      case ID_T_LEVELING: uiCfg.leveling_first_time = true;lv_draw_manualLevel();break;
      // case ID_T_PRE_HEAT: lv_draw_preHeat(); break;
      // case ID_T_EXTRUCT:  lv_draw_extrusion(); break;
      case ID_T_VOICE:      lv_draw_voice(); break;
      case ID_T_ENDSTOP:     lv_draw_endstop(); break;
      case ID_T_TEMPSETTING: lv_draw_tempsetting();break;        
      
      #if ENABLED(MKS_WIFI_MODULE)
        case ID_T_WIFI:
          if (gCfgItems.wifi_mode_sel == STA_MODEL) {
            if (wifi_link_state == WIFI_CONNECTED) {
              last_disp_state = TOOL_UI;
              lv_draw_wifi();
            }
            else {
              if (uiCfg.command_send) {
                // SERIAL_ECHOLNPGM("hello!\r\n");
                uint8_t cmd_wifi_list[] = { 0xA5, 0x07, 0x00, 0x00, 0xFC };
                raw_send_to_wifi(cmd_wifi_list, COUNT(cmd_wifi_list));
                last_disp_state = TOOL_UI;
                lv_draw_wifi_list();
              }
              else {
                // SERIAL_ECHOLNPGM("wait!\r\n");
                last_disp_state = TOOL_UI;
                wifi_connect_data.start = true;
                lv_draw_dialog(DIALOG_WIFI_ENABLE_TIPS);
              }
            }
          }
          else {
            last_disp_state = TOOL_UI;
            lv_draw_wifi();
          }
          break;
      #endif
      // // break;
      case ID_T_LANGUAGE: lv_draw_language();break;
      
      // case ID_T_LEVELING:
      //   #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
      //     get_gcode_command(AUTO_LEVELING_COMMAND_ADDR, (uint8_t *)public_buf_m);
      //     public_buf_m[sizeof(public_buf_m) - 1] = 0;
      //     queue.inject_P(PSTR(public_buf_m));
      //   #else
      //     uiCfg.leveling_first_time = true;
      //     lv_draw_manualLevel();
      //   #endif
      //   break;
      // case ID_T_FILAMENT:
      //   uiCfg.hotendTargetTempBak = thermalManager.degTargetHotend(uiCfg.extruderIndex);
      //   lv_draw_filament_change();
      //   break;
      case ID_T_MORE:
        lv_draw_more();
        break;
      case ID_T_ADVANCES:
        lv_draw_advances();
        // mks_draw_simple_num_kb(1,GCODE);
        break;
      case ID_T_ABOUT:
        lv_draw_about();
        break;
      case ID_T_RETURN:
        TERN_(MKS_TEST, current_disp_ui = 1);
        lv_draw_ready_print();
        break;
    }
  }
}

void lv_draw_tool() {
#ifdef USE_NEW_LVGL_CONF
  mks_ui.src_main = lv_set_scr_id_title(mks_ui.src_main, TOOL_UI, "");
#else
  lv_screen_distribution(TOOL_UI); 
  
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(TOOL_UI),TITLE_LABEL_X);
  // scr = lv_screen_create(TOOL_UI);
#endif
#ifdef USE_NEW_LVGL_CONF
  lv_big_button_create(mks_ui.src_main, "F:/bmp_preHeat.bin", tool_menu.preheat, INTERVAL_V, titleHeight, event_handler, ID_T_PRE_HEAT);
  lv_big_button_create(mks_ui.src_main, "F:/bmp_extruct.bin", tool_menu.extrude, BTN_X_PIXEL + INTERVAL_V * 2, titleHeight, event_handler, ID_T_EXTRUCT);
  lv_big_button_create(mks_ui.src_main, "F:/bmp_mov.bin", tool_menu.move, BTN_X_PIXEL * 2 + INTERVAL_V * 3, titleHeight, event_handler, ID_T_MOV);
  lv_big_button_create(mks_ui.src_main, "F:/bmp_zero.bin", tool_menu.home, BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_T_HOME);
  lv_big_button_create(mks_ui.src_main, "F:/bmp_leveling.bin", tool_menu.TERN(AUTO_BED_LEVELING_BILINEAR, autoleveling, leveling), INTERVAL_V, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_T_LEVELING);
  lv_big_button_create(mks_ui.src_main, "F:/bmp_filamentchange.bin", tool_menu.filament, BTN_X_PIXEL + INTERVAL_V * 2, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_T_FILAMENT);
  lv_big_button_create(mks_ui.src_main, "F:/bmp_more.bin", tool_menu.more, BTN_X_PIXEL * 2 + INTERVAL_V * 3, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_T_MORE);
  lv_big_button_create(mks_ui.src_main, "F:/bmp_return.bin", common_menu.text_back, BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_T_RETURN);
#else
  lv_refr_now(lv_refr_get_disp_refreshing());
  tool_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_back.bin", TITLE_BACK_X, TITLE_BACK_Y, event_handler, ID_T_RETURN);
  tool_page.level_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_leveling.bin", tool_btn_x_first_offset, tool_btn_y_first_offset + 42, event_handler, ID_T_LEVELING);
  tool_page.tempsetting_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_tempsetting.bin", tool_btn_x_first_offset, tool_btn_y_first_offset + tool_btn_h + tool_btn_y_offset + 42, event_handler, ID_T_TEMPSETTING);
  tool_page.wifi_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_wifi.bin", tool_btn_x_first_offset, tool_btn_y_first_offset + tool_btn_h * 2 + tool_btn_y_offset * 2 + 42, event_handler, ID_T_WIFI);
  tool_page.advances_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_advances.bin", tool_btn_x_first_offset, tool_btn_y_first_offset + tool_btn_h * 3 + tool_btn_y_offset * 3 + 42, event_handler, ID_T_ADVANCES);
  
  tool_page.endstop_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_endstop.bin", tool_btn_x_first_offset + tool_btn_w + tool_btn_x_offset, tool_btn_y_first_offset + 42, event_handler, ID_T_ENDSTOP);
  tool_page.voice_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_voice.bin", tool_btn_x_first_offset + tool_btn_w + tool_btn_x_offset, tool_btn_y_first_offset + tool_btn_h + tool_btn_y_offset + 42, event_handler, ID_T_VOICE);
  tool_page.language_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_language.bin", tool_btn_x_first_offset + tool_btn_w + tool_btn_x_offset, tool_btn_y_first_offset + tool_btn_h * 2 + tool_btn_y_offset * 2 + 42, event_handler, ID_T_LANGUAGE);
  tool_page.about_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_about.bin", tool_btn_x_first_offset + tool_btn_w + tool_btn_x_offset, tool_btn_y_first_offset + tool_btn_h * 3 + tool_btn_y_offset * 3 + 42, event_handler, ID_T_ABOUT);

  // tool_page.level_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_leveling.bin", 0, 0, event_handler, ID_T_LEVELING);
  // tool_page.language_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_language.bin", tool_btn_w + tool_btn_x_offset, 0, event_handler, ID_T_LANGUAGE);
  // tool_page.endstop_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_endstop.bin",  0, tool_btn_y_offset + tool_btn_h, event_handler, ID_T_ENDSTOP);
  // tool_page.filament_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/tool_filament.bin",tool_btn_w + tool_btn_x_offset,  tool_btn_y_offset + tool_btn_h, event_handler, ID_T_FILAMENT);
  // tool_page.voice_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/tool_voice.bin",  0, tool_btn_y_offset*2 + tool_btn_h*2, event_handler, ID_T_VOICE);
  // tool_page.tempsetting_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/tool_tempsetting.bin",  tool_btn_w + tool_btn_x_offset, tool_btn_y_offset*2 + tool_btn_h*2, event_handler, ID_T_TEMPSETTING);
  // tool_page.about_imgbtn =  lv_imgbtn_create(mks_printer.user_src, "F:/tool_about.bin", 0, tool_btn_y_offset*3 + tool_btn_h*3, event_handler, ID_T_ABOUT);
  // tool_page.back_imgbtn_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/tool_return.bin", tool_btn_w + tool_btn_x_offset, tool_btn_y_offset*3 + tool_btn_h*3, event_handler, ID_T_RETURN);
  // lv_imgbtn_label_align_create(mks_printer.user_src, "F:/tool_gcode.bin", "",LV_ALIGN_IN_BOTTOM_MID, 0, tool_btn_y_offset*4 + tool_btn_h*4,0, -5, event_handler, ID_T_ADVANCES);
  // lv_imgbtn_label_align_create(mks_printer.user_src, "F:/tool_about.bin", "",LV_ALIGN_IN_BOTTOM_MID, tool_btn_w + tool_btn_x_offset, tool_btn_y_offset*4 + tool_btn_h*4,0, -5, event_handler, ID_T_ABOUT);
  
  // if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  // {

  //  lv_label_align_create(tool_page.level_imgbtn ,tool_menu.leveling, 0, -2,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text_white_style);
  //  lv_label_align_create(tool_page.language_imgbtn ,tool_menu.language, 0, -2,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text_white_style);
  //  lv_label_align_create(tool_page.filament_imgbtn ,tool_menu.filament, 0, -2,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text_white_style);
  //  lv_label_align_create(tool_page.voice_imgbtn ,tool_menu.voice, 0, -2,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text_white_style);
  //  lv_label_align_create(tool_page.endstop_imgbtn ,tool_menu.endstop, 0, -2,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text_white_style);
  //  if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  //  lv_label_align_create(tool_page.about_imgbtn ,tool_menu.about, -25, 0,LV_ALIGN_IN_RIGHT_MID,  &User_style.text_white_style);
  //  else
  //  lv_label_align_create(tool_page.about_imgbtn ,tool_menu.about, -10, 0,LV_ALIGN_IN_RIGHT_MID,  &User_style.text_white_style);

  //  lv_label_align_create(tool_page.tempsetting_imgbtn ,tool_menu.tempsetting, 0, 0,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text_white_style);
  // //  lv_label_align_create(tool_page.refrig_imgbtn ,"Refrigeration", -40, 0,LV_ALIGN_IN_BOTTOM_MID,  &User_style.small_text_white_style);
  //  lv_label_align_create(tool_page.back_imgbtn_imgbtn ,tool_menu.back, 0, -5,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text_white_style);

  // }
  // else
  // {
   lv_label_align_create(tool_page.level_imgbtn ,tool_menu.leveling, -3, -5,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text16_white_style);
   lv_label_align_create(tool_page.tempsetting_imgbtn ,tool_menu.tempsetting, -5, -5,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text16_white_style);
   lv_label_align_create(tool_page.wifi_imgbtn ,tool_menu.wifi, -1, -5,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text16_white_style);
   lv_label_align_create(tool_page.advances_imgbtn ,tool_menu.advances, -4, -5,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text16_white_style);
   
   lv_label_align_create(tool_page.endstop_imgbtn ,tool_menu.endstop, -2, -5,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text16_white_style);
   lv_label_align_create(tool_page.voice_imgbtn ,tool_menu.voice, -5, -5,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text16_white_style);
   lv_label_align_create(tool_page.language_imgbtn ,tool_menu.language, -3, -5,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text16_white_style);
   lv_label_align_create(tool_page.about_imgbtn ,tool_menu.about, -5, -5,LV_ALIGN_IN_BOTTOM_MID,  &User_style.text16_white_style);

  //  char buff[20];
  //  ZERO(buff);
  //  sprintf(buff,"test %d\r\n",gCfgItems.test);
  //  SERIAL_ECHOPGM(buff);
  // }
  // lv_label_set_style(tool_page.level_label,LV_LABEL_STYLE_MAIN,&User_style.small_text_white_style);
  // lv_label_set_style(tool_page.language_label,LV_LABEL_STYLE_MAIN,&User_style.small_text_white_style);
  // lv_label_set_style(tool_page.endstop_label,LV_LABEL_STYLE_MAIN,&User_style.small_text_white_style);
  // lv_label_set_style(tool_page.filament_label,LV_LABEL_STYLE_MAIN,&User_style.small_text_white_style);
  // lv_label_set_style(tool_page.voice_label,LV_LABEL_STYLE_MAIN,&User_style.small_text_white_style);
  // lv_label_set_style(tool_page.tempsetting_label,LV_LABEL_STYLE_MAIN,&User_style.small_text_white_style);
  // lv_label_set_style(tool_page.refrig_label,LV_LABEL_STYLE_MAIN,&User_style.small_text_white_style);
  // lv_label_set_style(tool_page.back_imgbtn_label,LV_LABEL_STYLE_MAIN,&User_style.small_text_white_style);
  // lv_big_button_create(scr, "F:/bmp_preHeat.bin", tool_menu.preheat, INTERVAL_V, titleHeight, event_handler, ID_T_PRE_HEAT);
  // lv_big_button_create(scr, "F:/bmp_extruct.bin", tool_menu.extrude, BTN_X_PIXEL + INTERVAL_V * 2, titleHeight, event_handler, ID_T_EXTRUCT);
  // lv_big_button_create(scr, "F:/bmp_mov.bin", tool_menu.move, BTN_X_PIXEL * 2 + INTERVAL_V * 3, titleHeight, event_handler, ID_T_MOV);
  // lv_big_button_create(scr, "F:/bmp_zero.bin", tool_menu.home, BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_T_HOME);
  // lv_big_button_create(scr, "F:/bmp_leveling.bin", tool_menu.TERN(AUTO_BED_LEVELING_BILINEAR, autoleveling, leveling), INTERVAL_V, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_T_LEVELING);
  // lv_big_button_create(scr, "F:/bmp_filamentchange.bin", tool_menu.filament, BTN_X_PIXEL + INTERVAL_V * 2, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_T_FILAMENT);
  // lv_big_button_create(scr, "F:/bmp_more.bin", tool_menu.more, BTN_X_PIXEL * 2 + INTERVAL_V * 3, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_T_MORE);
  // lv_big_button_create(scr, "F:/bmp_return.bin", common_menu.text_back, BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_T_RETURN);

  // lv_refr_now(lv_refr_get_disp_refreshing());
  lv_imgbtn_set_src_both(tool_page.back_imgbtn, "F:/png_back.bin");
  // tool_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_back.bin", TITLE_BACK_X, TITLE_BACK_Y, event_handler, ID_T_RETURN);
#endif

}

void lv_clear_tool() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
#ifdef USE_NEW_LVGL_CONF
  lv_obj_clean(mks_ui.src_main);
#else
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);
#endif
}

#endif // HAS_TFT_LVGL_UI
