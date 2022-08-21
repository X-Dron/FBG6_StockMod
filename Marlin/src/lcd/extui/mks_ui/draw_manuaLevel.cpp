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

extern const char G28_STR[];

extern lv_group_t *g;
static lv_obj_t *scr;
extern float z_offset_add;
extern bool baby_step_set_data;

typedef struct{
  
  lv_obj_t *one_imgbtn;
  lv_obj_t *two_imgbtn;
  lv_obj_t *three_imgbtn;
  lv_obj_t *four_imgbtn;

  lv_obj_t *z_imgbtn;
  lv_obj_t *back_imgbtn;
  lv_obj_t *zoffset_imgbtn;

}LEVELING_UI_PAGE;

LEVELING_UI_PAGE leveling_page;

enum {
  ID_M_POINT1 = 1,
  ID_M_POINT2,
  ID_M_POINT3,
  ID_M_POINT4,
  ID_M_POINT5,
  ID_M_ZHOME,
  ID_MANUAL_RETURN,
  ID_M_ZOFFSET,
};

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  char buf[20];
  char str_1[16];
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
    button_sound_end();
    switch (obj->mks_obj_id) {
      case ID_M_POINT1 ... ID_M_POINT5:
        if (!queue.ring_buffer.full(1)) {
          if (uiCfg.leveling_first_time) {
            uiCfg.leveling_first_time = false;
            baby_step_set_data = true;
            queue.inject_P(G28_STR);
            // sprintf_P(buf,PSTR("M290 Z%s"),dtostrf(z_offset_add, 1, 3, str_1));
            // gcode.process_subcommands_now_P(PSTR(buf));
            // const int ind = obj->mks_obj_id - ID_M_POINT1;
            // sprintf_P(public_buf_l, PSTR("G1Z10F500\nG1X%dY%dF1500\nG1Z%fF500"), gCfgItems.trammingPos[ind].x, gCfgItems.trammingPos[ind].y,z_offset_add);
            // queue.inject(public_buf_l);
          }
            // if(baby_step_set_data)
            // {
            //   sprintf_P(buf,PSTR("M290 Z%s"),dtostrf(z_offset_add, 1, 3, str_1));
            //   queue.inject(PSTR(buf));
            //   baby_step_set_data = false;
            // }
            const int ind = obj->mks_obj_id - ID_M_POINT1;
            sprintf_P(public_buf_l, PSTR("G1Z10F500\nG1X%dY%dF1500\nG1Z0F500"), gCfgItems.trammingPos[ind].x, gCfgItems.trammingPos[ind].y);
            queue.inject(public_buf_l);
        }
        break;
      case ID_M_ZHOME:
        queue.inject_P(PSTR("G28Z"));
        break;
      case ID_M_ZOFFSET:
        lv_clear_manualLevel();
        lv_draw_baby_stepping();
        break;
      case ID_MANUAL_RETURN:
        lv_clear_manualLevel();
        lv_draw_tool();
        break;
    }
  }
}
void lv_draw_manualLevel() {

  // lv_screen_distribution(TOOL_UI); 
  
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(LEVELING_UI),TITLE_LABEL_X);//lv_title_offset_n_create(mks_printer.user_src,get_title_text(LEVELING_UI),10);

  lv_screen_distribution(LEVELING_UI);
  mks_printer.user_src3 = lv_scr_create(mks_printer.user_src, 10,67,300,299);
  lv_obj_set_style(mks_printer.user_src3,&User_style.level_bg_style);
  leveling_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_back.bin", TITLE_BACK_X, TITLE_BACK_Y, event_handler, ID_MANUAL_RETURN);
  lv_refr_now(lv_refr_get_disp_refreshing());

  leveling_page.four_imgbtn = lv_imgbtn_align_create(mks_printer.user_src3, "F:/png_level_four.bin", LV_ALIGN_IN_TOP_LEFT, 5, 5, event_handler, ID_M_POINT4);
  leveling_page.three_imgbtn = lv_imgbtn_align_create(mks_printer.user_src3, "F:/png_level_three.bin", LV_ALIGN_IN_TOP_RIGHT, -5, 5, event_handler, ID_M_POINT3);
  leveling_page.one_imgbtn = lv_imgbtn_align_create(mks_printer.user_src3, "F:/png_level_one.bin", LV_ALIGN_IN_BOTTOM_LEFT, 5, -5, event_handler, ID_M_POINT1);
  leveling_page.two_imgbtn = lv_imgbtn_align_create(mks_printer.user_src3, "F:/png_level_two.bin", LV_ALIGN_IN_BOTTOM_RIGHT, -5, -5, event_handler, ID_M_POINT2);

  lv_refr_now(lv_refr_get_disp_refreshing());
  leveling_page.z_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_level_z.bin", LV_ALIGN_IN_BOTTOM_LEFT, 10, -10, event_handler, ID_M_ZHOME);
  leveling_page.zoffset_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_level_zoffset.bin", LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10, event_handler, ID_M_ZOFFSET);


  lv_imgbtn_set_src_both(leveling_page.four_imgbtn,  "F:/png_level_four.bin");
  lv_imgbtn_set_src_both(leveling_page.one_imgbtn,  "F:/png_level_one.bin");
  lv_imgbtn_set_src_both(leveling_page.three_imgbtn,  "F:/png_level_three.bin");
  lv_imgbtn_set_src_both(leveling_page.two_imgbtn,  "F:/png_level_two.bin");


  // scr = lv_screen_create(LEVELING_UI);
  // // Create an Image button
  // lv_obj_t *buttonPoint1 = lv_big_button_create(scr, "F:/bmp_leveling1.bin", leveling_menu.position1, INTERVAL_V, titleHeight, event_handler, ID_M_POINT1);
  // lv_obj_clear_protect(buttonPoint1, LV_PROTECT_FOLLOW);
  // lv_big_button_create(scr, "F:/bmp_leveling2.bin", leveling_menu.position2, BTN_X_PIXEL + INTERVAL_V * 2, titleHeight, event_handler, ID_M_POINT2);
  // lv_big_button_create(scr, "F:/bmp_leveling3.bin", leveling_menu.position3, BTN_X_PIXEL * 2 + INTERVAL_V * 3, titleHeight, event_handler, ID_M_POINT3);
  // lv_big_button_create(scr, "F:/bmp_leveling4.bin", leveling_menu.position4, BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_M_POINT4);
  // lv_big_button_create(scr, "F:/bmp_leveling5.bin", leveling_menu.position5, INTERVAL_V, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_M_POINT5);
  // lv_big_button_create(scr, "F:/bmp_return.bin", common_menu.text_back, BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_MANUAL_RETURN);
}

void lv_clear_manualLevel() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);
}

#endif // HAS_TFT_LVGL_UI
