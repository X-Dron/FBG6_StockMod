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

// extern TEMPSET temp_seting;

#define first_btn_offset  50
#define btn_y_offset  50

#define temp_window_w 99
#define temp_window_h 69

#define temp_window_e_x 71
#define temp_window_b_x 202

#define temp_window_p_y 189
#define temp_window_a_y 300


typedef struct{
  
  lv_obj_t *extrusion1_label;
  lv_obj_t *extrusion2_label;
  lv_obj_t *bed_label;
  lv_obj_t *pla_label;
  lv_obj_t *abs_label;
  lv_obj_t *ex_pla_temp_label;
  lv_obj_t *bed_pla_temp_label;
  lv_obj_t *ex_abs_temp_label;
  lv_obj_t *bed_abs_temp_label;

  lv_obj_t *ex_pla_temp_scr;
  lv_obj_t *bed_pla_temp_scr;
  lv_obj_t *ex_abs_temp_scr;
  lv_obj_t *bed_abs_temp_scr;

  lv_obj_t *back_imgbtn;
  lv_style_t style_window;

}TEMPSETTING_UI_PAGE;

TEMPSETTING_UI_PAGE tempsetting_page;

enum {
  ID_T_PLA_EX1 = 1,
  ID_T_PLA_BED ,
  ID_T_ABS_EX1 ,
  ID_T_ABS_BED ,


  ID_T_BACK
};



static void event_handler(lv_obj_t *obj, lv_event_t event) {
 if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
      button_sound_end();
      lv_clear_tempsetting();
    switch (obj->mks_obj_id) {
      case ID_T_BACK: lv_draw_tool(); break;
      case ID_T_PLA_EX1: mks_draw_simple_num_kb(0,SET_PLA_TEMP);break;
      case ID_T_PLA_BED: mks_draw_simple_num_kb(0,SET_PLA_BED_TEMP);break;
      case ID_T_ABS_EX1: mks_draw_simple_num_kb(0,SET_ABS_TEMP);break;
      case ID_T_ABS_BED: mks_draw_simple_num_kb(0,SET_ABS_BED_TEMP);break;
    }
  }
}




void lv_draw_tempsetting() {



  // lv_screen_distribution(TEMPSETTING_UI);
  // mks_printer.user_src1 = lv_title_create(mks_printer.user_src,get_title_text(TEMPSETTING_UI));
  lv_screen_distribution(TEMPSETTING_UI); 
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(TEMPSETTING_UI),TITLE_LABEL_X);
  // mks_printer.user_src = lv_scr_create(mks_printer.user_src,0,TITLE_H,TITLE_W,TFT_HEIGHT-TITLE_H);
  lv_refr_now(lv_refr_get_disp_refreshing());

  lv_style_copy(&tempsetting_page.style_window, &lv_style_scr);
  tempsetting_page.style_window.body.main_color = LV_COLOR_MAKE(0xE1,0xDF,0xDF);
  tempsetting_page.style_window.body.grad_color = LV_COLOR_MAKE(0xE1,0xDF,0xDF);
  tempsetting_page.style_window.body.radius = 5;

 
  tempsetting_page.pla_label =  lv_label_create(mks_printer.user_src, tempsetting_menu.pla);
  lv_obj_align(tempsetting_page.pla_label, mks_printer.user_src, LV_ALIGN_IN_LEFT_MID, 10 ,-20);
  lv_label_set_style(tempsetting_page.pla_label,LV_LABEL_STYLE_MAIN,&User_style.text28_black_style);

  tempsetting_page.abs_label =  lv_label_create(mks_printer.user_src, tempsetting_menu.abs);
  lv_obj_align(tempsetting_page.abs_label, mks_printer.user_src, LV_ALIGN_IN_LEFT_MID, 10 ,10+80);
  lv_label_set_style(tempsetting_page.abs_label,LV_LABEL_STYLE_MAIN,&User_style.text28_black_style); //num_bg_style
  // operation_menu.temp
  sprintf(public_buf_l, "%d°C",temp_seting.pla_ex1);
  tempsetting_page.ex_pla_temp_label =  lv_region_style_align_create(mks_printer.user_src,public_buf_l,temp_window_w, temp_window_h, LV_ALIGN_IN_TOP_LEFT, temp_window_e_x, temp_window_p_y, &tempsetting_page.style_window,event_handler,ID_T_PLA_EX1);
  sprintf(public_buf_l, "%d°C",temp_seting.pla_bed);
  tempsetting_page.bed_pla_temp_label =  lv_region_style_align_create(mks_printer.user_src,public_buf_l,temp_window_w, temp_window_h, LV_ALIGN_IN_TOP_LEFT, temp_window_b_x, temp_window_p_y, &tempsetting_page.style_window,event_handler,ID_T_PLA_BED);


  sprintf(public_buf_l, "%d°C",temp_seting.abs_ex1);
  tempsetting_page.ex_abs_temp_label =  lv_region_style_align_create(mks_printer.user_src,public_buf_l,temp_window_w, temp_window_h, LV_ALIGN_IN_TOP_LEFT, temp_window_e_x, temp_window_a_y, &tempsetting_page.style_window,event_handler,ID_T_ABS_EX1);
  sprintf(public_buf_l, "%d°C",temp_seting.abs_bed);
  tempsetting_page.bed_abs_temp_label =  lv_region_style_align_create(mks_printer.user_src,public_buf_l,temp_window_w, temp_window_h, LV_ALIGN_IN_TOP_LEFT, temp_window_b_x, temp_window_a_y, &tempsetting_page.style_window,event_handler,ID_T_ABS_BED);


  tempsetting_page.extrusion1_label =  lv_label_create(mks_printer.user_src, tempsetting_menu.ex1);
  lv_obj_align(tempsetting_page.extrusion1_label, tempsetting_page.ex_pla_temp_label, LV_ALIGN_OUT_TOP_MID, -15 ,-50);
  
 
  tempsetting_page.bed_label = lv_label_create(mks_printer.user_src, tempsetting_menu.bed);
  lv_obj_align(tempsetting_page.bed_label, tempsetting_page.bed_pla_temp_label, LV_ALIGN_OUT_TOP_MID, -10 ,-50);



  lv_label_set_style(tempsetting_page.extrusion1_label,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
  lv_label_set_style(tempsetting_page.bed_label,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);

  // mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(VOICE_UI),TITLE_LABEL_X);
  tempsetting_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src,"F:/png_back.bin",TITLE_BACK_X,TITLE_BACK_Y, event_handler, ID_T_BACK);
  // tempsetting_page.back_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/back.bin",LV_ALIGN_IN_BOTTOM_RIGHT,-10,-10, event_handler, ID_T_BACK);



  // lv_imgbtn_set_src_both(tempsetting_page.back_imgbtn,  "F:/return.bin");
// lv_obj_t *lv_imgbtn_align_create(lv_obj_t *par, const char *img,lv_align_t align1,lv_coord_t x, lv_coord_t y, lv_event_cb_t cb, const int id)

  // lv_obj_t *lv_imgbtn_align_create(lv_obj_t *par, const char *img,lv_align_t align1,lv_coord_t x, lv_coord_t y, lv_event_cb_t cb, const int id)

}

void lv_clear_tempsetting() {
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);

}

#endif // HAS_TFT_LVGL_UI
