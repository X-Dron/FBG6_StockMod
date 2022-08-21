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

extern lv_group_t *g;
extern uint8_t layer_stop_flag;


enum {
  ID_LAYER_STOP_ADD = 1,
  ID_LAYER_STOP_DEC,
  ID_LAYER_STOP_SAVE,
  ID_LAYER_STOP_NUM,
  ID_LAYER_STOP_RETURN,
};
typedef struct{
  lv_obj_t *btn_add;
  lv_obj_t *btn_dec;
  lv_obj_t *label_add;
  lv_obj_t *label_dec;
  lv_obj_t *btn_save;
  lv_obj_t *imgbtn_back;
  // lv_obj_t *label_title_name;
  lv_obj_t *label_layer_stop;
  lv_obj_t *label_layer_stop_num;
  lv_obj_t *label_save;

  lv_obj_t *block_bg1;
  lv_obj_t *block_bg2;
  lv_obj_t *block_save;

  lv_style_t style_disylay_border;
  lv_style_t style_disylay_btn;
  lv_style_t style_btn;
  lv_style_t style_blue_btn;


}LAYER_STOP_PAGE;

LAYER_STOP_PAGE layer_stop_page;

int32_t disp_layer_stop_num = 0;
int32_t save_disp_layer_stop_num = 0;

int32_t save_layer_stop_num = 0;
bool gb_kb_input_layout_num_flag = 0;
int32_t gb_kb_input_layout_num = 0;

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  // if (event != LV_EVENT_RELEASED) return;

    if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
        button_sound_end();
    switch (obj->mks_obj_id) {
      case ID_LAYER_STOP_ADD:
          disp_layer_stop_num++; 
            disp_label_layer_stop_num();
      break;

      case ID_LAYER_STOP_DEC:
          disp_layer_stop_num--;
          if(disp_layer_stop_num < 0)
          {
            disp_layer_stop_num = 0;
          }
            disp_label_layer_stop_num();
      break;

      case ID_LAYER_STOP_SAVE:
        layer_stop_flag = LAYER_STOP_NO_TRIGGERED;
        if((Layout_stop_num.data > 0)&&(Layout_stop_num.bottom_data > 0))
        {
          if((disp_layer_stop_num >= 1))
          {
            save_disp_layer_stop_num = disp_layer_stop_num;
            save_layer_stop_num = (save_disp_layer_stop_num - 1) * Layout_stop_num.data * 100;
            save_layer_stop_num += Layout_stop_num.bottom_data * 100;

            // char buff[50];
            // dtostrf(save_layer_stop_num, 1, 1, buff);
            // // sprintf(buff,"%s ll0\r\n",);
            // SERIAL_ECHOPGM(buff);
            // SERIAL_ECHOPGM("\r\n");
            lv_clear_layer_stop();
            draw_return_ui();
          }
          else
          {
            save_disp_layer_stop_num = 0;
            save_layer_stop_num = 0;
          }
        }
      break;
      case ID_LAYER_STOP_NUM:
           lv_clear_layer_stop();
           mks_draw_simple_num_kb(0,SET_LAYER_STOP);
           break;
      case ID_LAYER_STOP_RETURN:
            gb_kb_input_layout_num_flag = 0;
            lv_clear_layer_stop();
            draw_return_ui();
      break;
    }
  }
}
void lv_draw_layer_stop() {

  lv_screen_distribution(LAYER_STOP_UI); 
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(LAYER_STOP_UI),20);



  lv_style_copy(&layer_stop_page.style_disylay_btn, &lv_style_scr);
  layer_stop_page.style_disylay_btn.body.main_color = LV_COLOR_MAKE(0xf3,0xf3,0xf3);
  layer_stop_page.style_disylay_btn.body.grad_color = LV_COLOR_MAKE(0xf3,0xf3,0xf3);
  layer_stop_page.style_disylay_btn.body.border.color = LV_COLOR_MAKE(0Xc9,0Xc9,0Xc9);
  layer_stop_page.style_disylay_btn.body.border.width = 2;
  layer_stop_page.style_disylay_btn.body.radius = 10;

  lv_style_copy(&layer_stop_page.style_disylay_border, &lv_style_scr);
  layer_stop_page.style_disylay_border.body.main_color = LV_COLOR_MAKE(0xf9,0xf9,0xf9);
  layer_stop_page.style_disylay_border.body.grad_color = LV_COLOR_MAKE(0xf9,0xf9,0xf9);
  layer_stop_page.style_disylay_border.body.border.color = LV_COLOR_MAKE(0Xc9,0Xc9,0Xc9);
  layer_stop_page.style_disylay_border.body.border.width = 2;

  lv_style_copy(&layer_stop_page.style_btn, &lv_style_scr);
  layer_stop_page.style_btn.body.main_color = LV_COLOR_MAKE(0xf3,0xf3,0xf3);
  layer_stop_page.style_btn.body.grad_color = LV_COLOR_MAKE(0xf3,0xf3,0xf3);
  layer_stop_page.style_btn.text.font = &lv_font_roboto_28;
  layer_stop_page.style_btn.text.color = LV_COLOR_BLACK;
  layer_stop_page.style_btn.body.opa = 0;

  lv_style_copy(&layer_stop_page.style_blue_btn, &lv_style_scr);
  layer_stop_page.style_blue_btn.body.main_color = LV_COLOR_MAKE(0x41,0x85,0xf7);
  layer_stop_page.style_blue_btn.body.grad_color = LV_COLOR_MAKE(0x41,0x85,0xf7);
  layer_stop_page.style_blue_btn.body.radius = 10;

  layer_stop_page.block_bg1 = lv_obj_create(mks_printer.user_src,NULL);
  lv_obj_set_pos(layer_stop_page.block_bg1,17,234);
  lv_obj_set_size(layer_stop_page.block_bg1,286,60);
  lv_obj_set_style(layer_stop_page.block_bg1,&layer_stop_page.style_disylay_btn);

  layer_stop_page.block_bg2 = lv_btn_create(layer_stop_page.block_bg1,event_handler,ID_LAYER_STOP_NUM);//lv_btn_create(layer_stop_page.block_bg1,event_handler,ID_LAYER_STOP_NUM);
  lv_obj_set_pos(layer_stop_page.block_bg2,65,0);
  lv_obj_set_size(layer_stop_page.block_bg2,156,60);
  lv_btn_set_style_both(layer_stop_page.block_bg2,&layer_stop_page.style_disylay_border);
  // lv_obj_set_style(layer_stop_page.block_bg2,&layer_stop_page.style_disylay_border);
  // lv_obj_set_event_cb_mks(layer_stop_page.block_bg2, event_handler, ID_LAYER_STOP_NUM, "", 0);


  layer_stop_page.btn_add = lv_btn_create(layer_stop_page.block_bg1,event_handler,ID_LAYER_STOP_DEC);
  lv_obj_set_size(layer_stop_page.btn_add,50,50);
  lv_obj_align(layer_stop_page.btn_add,layer_stop_page.block_bg1,LV_ALIGN_IN_LEFT_MID,5,0);
  lv_obj_set_style(layer_stop_page.btn_add,&layer_stop_page.style_btn);
  layer_stop_page.label_add = lv_label_create(layer_stop_page.btn_add,LV_SYMBOL_LEFT);
  // lv_obj_set_style(layer_stop_page.label_add,&layer_stop_page.style_btn);
  lv_label_set_style(layer_stop_page.label_add,LV_LABEL_STYLE_MAIN,&layer_stop_page.style_btn);

  layer_stop_page.btn_dec = lv_btn_create(layer_stop_page.block_bg1,event_handler,ID_LAYER_STOP_ADD);
  lv_obj_set_size(layer_stop_page.btn_dec,50,50);
  lv_obj_align(layer_stop_page.btn_dec,layer_stop_page.block_bg1,LV_ALIGN_IN_RIGHT_MID,-5,0);
  lv_obj_set_style(layer_stop_page.btn_dec,&layer_stop_page.style_btn);
  layer_stop_page.label_dec = lv_label_create(layer_stop_page.btn_dec,LV_SYMBOL_RIGHT);
  // lv_obj_set_style(layer_stop_page.label_dec,&layer_stop_page.style_btn);
  lv_label_set_style(layer_stop_page.label_dec,LV_LABEL_STYLE_MAIN,&layer_stop_page.style_btn);


  layer_stop_page.imgbtn_back = lv_imgbtn_create(mks_printer.user_src, "F:/png_back2.bin", 223, 387, event_handler, ID_LAYER_STOP_RETURN);
  


  layer_stop_page.btn_save = lv_btn_create(mks_printer.user_src,NULL);//event_handler,ID_LAYER_STOP_SAVE
  lv_obj_set_event_cb_mks(layer_stop_page.btn_save, event_handler, ID_LAYER_STOP_SAVE, "", 0);
  lv_obj_set_size(layer_stop_page.btn_save,135,75);
  lv_obj_set_pos(layer_stop_page.btn_save,16,387);
  lv_btn_set_style_both(layer_stop_page.btn_save,  &layer_stop_page.style_blue_btn);
  lv_label_align_create(layer_stop_page.btn_save,"save", 0, 0,LV_ALIGN_CENTER,   &User_style.text28_white_style);

  char buf[30];
  ZERO(buf);
  if(gb_kb_input_layout_num_flag)
    sprintf(buf,"%d",gb_kb_input_layout_num);
  else
    sprintf(buf,"%d",save_disp_layer_stop_num);
  
  layer_stop_page.label_layer_stop_num = lv_label_align_create(layer_stop_page.block_bg1,buf, 0, 0,LV_ALIGN_CENTER,   &User_style.text_black_style);


  lv_label_align(mks_printer.user_src,layer_stop_page.label_layer_stop,layer_stop_page.block_bg1, "Layer",LV_ALIGN_OUT_TOP_MID,0, -30,&layer_stop_page.style_btn);

  if(gb_kb_input_layout_num_flag)
    disp_layer_stop_num = gb_kb_input_layout_num;
  else
    disp_layer_stop_num = save_disp_layer_stop_num;


}


void disp_label_layer_stop_num()
{
    char buf[30];
    ZERO(buf);
    sprintf(buf,"%d",disp_layer_stop_num);
    lv_label_set_text(layer_stop_page.label_layer_stop_num,buf);
    lv_obj_align(layer_stop_page.label_layer_stop_num, layer_stop_page.block_bg1, LV_ALIGN_CENTER, 0, 0);
}

void lv_clear_layer_stop() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_clean(mks_printer.user_src);
}

#endif // HAS_TFT_LVGL_UI
