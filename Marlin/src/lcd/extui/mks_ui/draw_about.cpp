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

#include "../../../inc/MarlinConfig.h"

extern lv_group_t *g;
static lv_obj_t *scr;
static lv_obj_t *fw_type, *board;

enum { ID_A_RETURN = 1 };
typedef struct{
  
  lv_obj_t *model_label;
  lv_obj_t *size_label;
  lv_obj_t *diameter_label;
  lv_obj_t *voltage_label;
  lv_obj_t *support_label;
  lv_obj_t *contact_label;
  lv_obj_t *version_label;
  lv_obj_t *sales_label;

  lv_obj_t *model_label_value;
  lv_obj_t *size_label_value;
  lv_obj_t *diameter_label_value;
  lv_obj_t *voltage_label_value;
  lv_obj_t *support_label_value;
  lv_obj_t *support1_label_value;
  lv_obj_t *Sales_label_value;
  lv_obj_t *contact_label_value;
  lv_obj_t *version_label_value;
  lv_obj_t *back_imgbtn;

}ABOUT_UI_PAGE;

ABOUT_UI_PAGE about_page;

static void event_handler(lv_obj_t *obj, lv_event_t event) {
   if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
    button_sound_end();
    switch (obj->mks_obj_id) {
      case ID_A_RETURN:
        clear_cur_ui();
        draw_return_ui();
        break;
    }
  }
}





void lv_draw_about() {
  static lv_style_t about_grey_style;
  lv_style_copy(&about_grey_style, &User_style.text_grey_style);
  about_grey_style.text.font         = &DengXian_16;
  lv_screen_distribution(ABOUT_UI);
  

  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(ABOUT_UI),TITLE_LABEL_X);
  mks_printer.user_src2 = lv_scr_create(mks_printer.user_src,0,50,320,TFT_HEIGHT-50);
  about_page.back_imgbtn = lv_imgbtn_align_create(mks_printer.user_src1, "F:/png_back.bin", LV_ALIGN_IN_LEFT_MID, 0, 0, event_handler, ID_A_RETURN);
  about_page.model_label = lv_label_align_create(mks_printer.user_src2,about_menu.model, 10, 10,LV_ALIGN_IN_TOP_LEFT,  &User_style.text_black_style);
  about_page.size_label = lv_label_align_create(mks_printer.user_src2,about_menu.size, 10, 75,LV_ALIGN_IN_TOP_LEFT,  &User_style.text_black_style);
  about_page.diameter_label = lv_label_align_create(mks_printer.user_src2,about_menu.diameter, 10, 140,LV_ALIGN_IN_TOP_LEFT,  &User_style.text_black_style);
  // about_page.voltage_label = lv_label_align_create(mks_printer.user_src2,about_menu.voltage, 10, 205,LV_ALIGN_IN_TOP_LEFT,  &User_style.text_black_style);
  about_page.version_label = lv_label_align_create(mks_printer.user_src2,about_menu.version, 10, 205,LV_ALIGN_IN_TOP_LEFT,  &User_style.text_black_style);//270
  about_page.support_label = lv_label_align_create(mks_printer.user_src2,about_menu.support, 10, 270,LV_ALIGN_IN_TOP_LEFT,  &User_style.text_black_style);//336
  about_page.sales_label  = lv_label_align_create(mks_printer.user_src2,about_menu.sales, 10, 336,LV_ALIGN_IN_TOP_LEFT,  &User_style.text_black_style);


  about_page.model_label_value = lv_label_align(mks_printer.user_src2,about_page.model_label_value,about_page.model_label, "Flying Bear Ghost 6",LV_ALIGN_OUT_LEFT_BOTTOM,0, 5,&about_grey_style);
  about_page.size_label_value = lv_label_align(mks_printer.user_src2,about_page.size_label_value,about_page.size_label, "255mm*210mm*210mm",LV_ALIGN_OUT_LEFT_BOTTOM,0, 5,&about_grey_style);
  about_page.diameter_label_value = lv_label_align(mks_printer.user_src2,about_page.diameter_label_value,about_page.diameter_label, "1.75mm",LV_ALIGN_OUT_LEFT_BOTTOM,0, 5,&about_grey_style);
  // about_page.voltage_label_value = lv_label_align(mks_printer.user_src2,about_page.voltage_label_value,about_page.voltage_label, "AC220V/AC110V",LV_ALIGN_OUT_LEFT_BOTTOM,0, 5,&User_style.text_grey_style);
  about_page.version_label_value = lv_label_align(mks_printer.user_src2,about_page.version_label_value,about_page.version_label, "GHOST6-V1.0_09X06_014",LV_ALIGN_OUT_LEFT_BOTTOM,0, 5,&about_grey_style);
  about_page.support_label_value = lv_label_align(mks_printer.user_src2,about_page.support_label_value,about_page.support_label, "support@3dflyingbear.com",LV_ALIGN_OUT_LEFT_BOTTOM,0, 5,&about_grey_style);
  // about_page.support1_label_value = lv_label_align(mks_printer.user_src2,about_page.support_label_value,about_page.support_label, "adam@3dflyingbear.com",LV_ALIGN_OUT_LEFT_BOTTOM,0, 30,&User_style.text_grey_style);
  about_page.support1_label_value = lv_label_align(mks_printer.user_src2,about_page.support1_label_value,about_page.sales_label, "adam@3dflyingbear.com",LV_ALIGN_OUT_LEFT_BOTTOM,0, 5,&about_grey_style);
  about_page.Sales_label_value = lv_label_align(mks_printer.user_src2,about_page.Sales_label_value,about_page.support1_label_value, "www.3dflyingbear.com",LV_ALIGN_OUT_LEFT_BOTTOM,0, 5,&about_grey_style);
}

void lv_clear_about() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_clean(mks_printer.user_src);
}

#endif // HAS_TFT_LVGL_UI
