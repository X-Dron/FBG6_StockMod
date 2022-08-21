/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based png_on Sprinter and grbl.
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
#include "../../../module/planner.h"        // for synchronize
#include "../../../gcode/gcode.h"
extern bool open_fan;

bool more_open_fan = true;
uint32_t list_off_time = 0;

typedef struct{
  lv_obj_t *scr1;
  
  lv_obj_t *fan_imgbtn;
  lv_obj_t *filament_imgbtn;
  lv_obj_t *z_offset_imgbtn;
  lv_obj_t *layer_imgbtn;
  lv_obj_t *fan_5010_imgbtn;
  lv_obj_t *fan_5010_img;

  lv_obj_t *back_imgbtn;
  lv_obj_t *fan_label;
  lv_obj_t *filament_label;
  lv_obj_t *layer_label;
  lv_obj_t *z_offset_label;
  lv_obj_t *fan_5010_label;
  lv_obj_t *fan_5010_state_label;
}MORE_UI_PAGE;
MORE_UI_PAGE more_page;


enum {
  ID_M_FILAMENT = 1,
  ID_M_FAN,
  ID_M_ZOFFSET,
  ID_M_LAYER,
  ID_M_BACK,
  ID_M_FAN_5010,
};

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if(event == LV_EVENT_PRESSED)
      {
        button_sound_start();
      }
    if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
      button_sound_end();
      switch (obj->mks_obj_id) {
        case ID_M_FAN:
          disp_pre_cover_up();
          lv_clear_printing();
          mks_draw_simple_num_kb(0,FAN_SPEED);
          break;
        case ID_M_ZOFFSET:
          disp_pre_cover_up();
          lv_clear_printing();
          lv_draw_baby_stepping();
          break;
        case ID_M_LAYER:
          disp_pre_cover_up();
          lv_clear_printing();
          lv_draw_layer_stop();
          break;
        case ID_M_FILAMENT:
          disp_pre_cover_up();
          lv_clear_printing();
          mks_draw_simple_num_kb(0,EX_SPEED);
        break;
        case ID_M_FAN_5010:
          if(open_fan)
          {
            lv_imgbtn_set_src_both(more_page.fan_5010_imgbtn,"F:/png_fan_5010_off.bin");
            // gCfgItems.fan_5010_state = false;
            // update_spi_flash();
            lv_label_set_text(more_page.fan_5010_state_label, "off");
            // thermalManager.set_fan_speed(1,0);
            gcode.process_subcommands_now_P("M106 P1 S0");
            list_off_time = print_time.minutes;
            open_fan = false;
            more_open_fan = false;
          }
          else
          {
            lv_imgbtn_set_src_both(more_page.fan_5010_imgbtn,"F:/png_fan_5010_on.bin");
            // gCfgItems.fan_5010_state = true;
            // update_spi_flash();
            lv_label_set_text(more_page.fan_5010_state_label, "on");
            // thermalManager.set_fan_speed(1,255);
            gcode.process_subcommands_now_P("M106 P1 S255");
            open_fan = true;
            more_open_fan = true;
          }
          break;
        case ID_M_BACK:
          clear_cur_ui();
          draw_return_ui();
        break;
    }
  }
}


void lv_draw_more() {
  static lv_style_t style_fan_5010;
  lv_style_copy(&style_fan_5010, &User_style.title_bg_style);
  style_fan_5010.body.main_color = LV_COLOR_WHITE;
  style_fan_5010.body.grad_color = LV_COLOR_WHITE;
  style_fan_5010.body.radius = 5;
  style_fan_5010.body.border.width = 2;
  // style_fan_5010.body.border.opa = 100;
  style_fan_5010.body.border.color = LV_COLOR_BLACK;
  lv_screen_distribution(MORE_UI);
  // thermalManager.set_fan_speed(1,255);
  lv_refr_now(lv_refr_get_disp_refreshing());

  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(MORE_UI),TITLE_LABEL_X);
  more_page.scr1 = lv_obj_create(mks_printer.user_src, nullptr);
  lv_obj_set_size(more_page.scr1, 243, 100);
  lv_obj_set_style(more_page.scr1, &style_fan_5010);
  lv_obj_set_pos(more_page.scr1 , 39, 341);
  // mks_printer.user_src = lv_scr_create(mks_printer.user_src,0,TITLE_H,TITLE_W,TFT_HEIGHT-TITLE_H);
  lv_refr_now(lv_refr_get_disp_refreshing());

  more_page.layer_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_layer.bin",63, 96, event_handler, ID_M_LAYER);
  more_page.filament_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_ex_factor.bin",214, 96, event_handler, ID_M_FILAMENT);
  more_page.z_offset_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_z_offset.bin",38, 217, event_handler, ID_M_ZOFFSET);
  more_page.fan_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_fan_5010.bin",205, 217, event_handler, ID_M_FAN);

  more_page.fan_label = lv_label_create_empty(mks_printer.user_src);
  lv_label_set_text(more_page.fan_label, "100%");
  lv_obj_align(more_page.fan_label, more_page.fan_imgbtn, LV_ALIGN_CENTER, 0, 43);
  lv_obj_set_style(more_page.fan_label,&User_style.text22_black_style);


  more_page.filament_label = lv_label_create_empty(mks_printer.user_src);
  lv_label_set_text(more_page.filament_label, "100%");
  lv_obj_align(more_page.filament_label, more_page.filament_imgbtn, LV_ALIGN_CENTER, 0, 47);
  lv_obj_set_style(more_page.filament_label,&User_style.text22_black_style);


  more_page.layer_label = lv_label_create_empty(mks_printer.user_src);
  lv_label_set_text(more_page.layer_label, move_menu.layout_stop);
  if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  lv_obj_align(more_page.layer_label, more_page.layer_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, -40, 3);
  else
  lv_obj_align(more_page.layer_label, more_page.layer_imgbtn, LV_ALIGN_CENTER, 0, 52);
  // lv_obj_align(more_page.layer_label, more_page.layer_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 12, 3);
  lv_obj_set_style(more_page.layer_label,&User_style.text22_black_style);

  more_page.z_offset_label = lv_label_create_empty(mks_printer.user_src);
  lv_label_set_text(more_page.z_offset_label, move_menu.zoffset);
  if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  lv_obj_align(more_page.z_offset_label, more_page.z_offset_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, -40, 3);
  else
  lv_obj_align(more_page.z_offset_label, more_page.z_offset_imgbtn, LV_ALIGN_CENTER, 0, 40);
  // lv_obj_align(more_page.z_offset_label, more_page.z_offset_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 5, 3);
  lv_obj_set_style(more_page.z_offset_label,&User_style.text22_black_style);

  
  more_page.fan_5010_label = lv_label_create_empty(mks_printer.user_src);
  lv_label_set_text(more_page.fan_5010_label, advances_menu.fan5051);
  lv_obj_set_style(more_page.fan_5010_label, &User_style.text22_black_style);
  lv_obj_set_pos(more_page.fan_5010_label, 152, 402);
  // lv_obj_align(more_page.fan_5010_label, more_page.fan_5010_imgbtn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);
  
  more_page.fan_5010_img = lv_img_create(more_page.scr1, nullptr);
  lv_img_set_src(more_page.fan_5010_img, "F:/png_fan_temp.bin");
  lv_obj_align(more_page.fan_5010_img, nullptr, LV_ALIGN_IN_LEFT_MID, 10, 0);
  // lv_obj_set_pos(more_page.fan_5010_img , 60, 360);
  if(open_fan)
  {
    more_page.fan_5010_imgbtn = lv_imgbtn_create(more_page.scr1,"F:/png_fan_5010_on.bin",event_handler, ID_M_FAN_5010);
    lv_obj_align(more_page.fan_5010_imgbtn, more_page.fan_5010_img, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    more_page.fan_5010_state_label = lv_label_create_empty(more_page.fan_5010_imgbtn);
    lv_label_set_text(more_page.fan_5010_state_label, "on");
    lv_obj_align(more_page.fan_5010_state_label, nullptr, LV_ALIGN_CENTER, 10, 0);
  }
  else
  {
    more_page.fan_5010_imgbtn = lv_imgbtn_create(more_page.scr1,"F:/png_fan_5010_off.bin",event_handler, ID_M_FAN_5010);
    lv_obj_align(more_page.fan_5010_imgbtn, more_page.fan_5010_img, LV_ALIGN_OUT_RIGHT_TOP, 20, 0);
    more_page.fan_5010_state_label = lv_label_create_empty(more_page.fan_5010_imgbtn);
    lv_label_set_text(more_page.fan_5010_state_label, "off");
    lv_obj_align(more_page.fan_5010_state_label, nullptr, LV_ALIGN_CENTER, 10, 0);
  }
  lv_label_set_style(more_page.fan_5010_state_label,LV_LABEL_STYLE_MAIN,&User_style.text_title_white_style);

  // more_page.fan_5010_label = lv_label_create_empty(more_page.scr1);
  // lv_label_set_text(more_page.fan_5010_label, advances_menu.fan5051);
  // lv_obj_set_style(more_page.fan_5010_label, &User_style.text22_black_style);
  // lv_obj_align(more_page.fan_5010_label, more_page.fan_5010_imgbtn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);

  lv_refr_now(lv_refr_get_disp_refreshing());
  lv_img_set_src(more_page.fan_5010_img, "F:/png_fan_temp.bin");
  more_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src,"F:/png_back.bin",TITLE_BACK_X,TITLE_BACK_Y, event_handler, ID_M_BACK);
  lv_imgbtn_set_src_both(more_page.back_imgbtn, "F:/png_back.bin");

  disp_fan_speed();
  disp_filament_speed();
//  endstop_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src,"F:/png_back.bin",TITLE_BACK_X,TITLE_BACK_Y, event_handler, ID_E_BACK);

  // lv_imgbtn_set_src_both(more_page.btn_sound_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(more_page.endstop_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(more_page.print_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(more_page.no_filament_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(more_page.back_imgbtn,  "F:/return.bin");
// lv_obj_t *lv_imgbtn_align_create(lv_obj_t *par, const char *img,lv_align_t align1,lv_coord_t x, lv_coord_t y, lv_event_cb_t cb, const int id)

  // lv_obj_t *lv_imgbtn_align_create(lv_obj_t *par, const char *img,lv_align_t align1,lv_coord_t x, lv_coord_t y, lv_event_cb_t cb, const int id)

}

void disp_fan_speed() {

  sprintf_P(public_buf_l, PSTR("%d%%"), (int)thermalManager.fanSpeedPercent(0));
  lv_label_set_text(more_page.fan_label, public_buf_l);
  lv_obj_align(more_page.fan_label, more_page.fan_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

}

void disp_fan_5010_state()
{
  if(open_fan)
  {
    lv_imgbtn_set_src_both(more_page.fan_5010_imgbtn,"F:/png_fan_5010_on.bin");
    lv_label_set_text(more_page.fan_5010_state_label, "on");
  }
  else
  {
    lv_imgbtn_set_src_both(more_page.fan_5010_imgbtn,"F:/png_fan_5010_off.bin");
    lv_label_set_text(more_page.fan_5010_state_label, "off");
  }
}

void disp_filament_speed()
{
  sprintf_P(public_buf_l, PSTR("%d%%"), planner.flow_percentage[uiCfg.extruderIndex]);
  lv_label_set_text(more_page.filament_label, public_buf_l);
  lv_obj_align(more_page.filament_label, more_page.filament_imgbtn, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);

}

void lv_clear_more() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);

}
#endif // HAS_TFT_LVGL_UI
