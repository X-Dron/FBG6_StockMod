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
bool flame_tri_flag = false;
bool open_fan = false;
extern bool more_open_fan;
extern uint32_t list_off_time;

typedef struct{
  
  lv_obj_t *floodlight_label;
  lv_obj_t *fan5051_label;
  lv_obj_t *flame_alarm_label;
  lv_obj_t *no_filament_label;

  lv_obj_t *btn_sound_imgbtn;
  lv_obj_t *endstop_imgbtn;
  lv_obj_t *print_imgbtn;
  lv_obj_t *no_filament_imgbtn;

  lv_obj_t *back_imgbtn;

}ADVANCES_UI_PAGE;
ADVANCES_UI_PAGE advances_page;

typedef enum{
  BTN_Y_FIRST_OFFSET = 126,
  BTN_Y_OFFSET = 107,
  X_LEFT_ALIGN = 10,
  X_RIGTH_ALIGN = -10
}ADVANCES_BTN_LAOUT;


ADVANCES_UI_DATA advances_data;



enum {
  ID_V_LIGHT = 1,
  ID_V_FAN,
  ID_V_FLAME,
  ID_V_BACK
};

void flame_beep_sound_start()
{
  pinMode(PA2,OUTPUT);
  digitalWrite(PA2, HIGH);
}
void flame_beep_sound_end()
{
  pinMode(PA2,OUTPUT);
  digitalWrite(PA2, LOW);
}
static void event_handler(lv_obj_t *obj, lv_event_t event) {
 if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
    button_sound_end();
    switch (obj->mks_obj_id) {
      case ID_V_BACK:   lv_clear_advances(); lv_draw_tool(); break;
      case ID_V_LIGHT:
      if(gCfgItems.advances_light)
      {
        gCfgItems.advances_light = false;
        digitalWrite(PE6, HIGH);
        lv_imgbtn_set_src_both(advances_page.btn_sound_imgbtn,"F:/png_off.bin");
      }
      else
      {
        gCfgItems.advances_light = true;
        digitalWrite(PE6, LOW);
        lv_imgbtn_set_src_both(advances_page.btn_sound_imgbtn,"F:/png_on.bin");
      }
        break;
      case ID_V_FLAME:
      if(gCfgItems.advances_flame)
      {
        gCfgItems.advances_flame = false;
        lv_imgbtn_set_src_both(advances_page.print_imgbtn,"F:/png_off.bin");
      }
      else
      {
        gCfgItems.advances_flame = true;
        lv_imgbtn_set_src_both(advances_page.print_imgbtn,"F:/png_on.bin");
      }
      break;
      case ID_V_FAN:
       if(gCfgItems.advances_5010fan)
      {
        gCfgItems.advances_5010fan = false;
        lv_imgbtn_set_src_both(advances_page.endstop_imgbtn,"F:/png_off.bin");
        // digitalWrite(BEEPER_PIN, LOW);
      }
      else
      {
        gCfgItems.advances_5010fan = true;
        lv_imgbtn_set_src_both(advances_page.endstop_imgbtn,"F:/png_on.bin");
      }
      break;
    }
    update_spi_flash();
  }
}


void lv_draw_advances() {
  static lv_style_t advances_black_style;
  lv_style_copy(&advances_black_style, &User_style.text_black_style);
  advances_black_style.text.font         = &DengXian_16;

  lv_screen_distribution(ADVANCES_UI);
  // thermalManager.set_fan_speed(1,255);
  digitalWrite(PE7, LOW);
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(ADVANCES_UI),TITLE_LABEL_X);
  // mks_printer.user_src = lv_scr_create(mks_printer.user_src,0,TITLE_H,TITLE_W,TFT_HEIGHT-TITLE_H);
  lv_refr_now(lv_refr_get_disp_refreshing());

  advances_page.floodlight_label = lv_label_align_create(mks_printer.user_src,advances_menu.floodlight,X_LEFT_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*0,LV_ALIGN_IN_TOP_LEFT,&advances_black_style);
  advances_page.fan5051_label = lv_label_align_create(mks_printer.user_src,advances_menu.fan5051,X_LEFT_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*1,LV_ALIGN_IN_TOP_LEFT,&advances_black_style);
  advances_page.flame_alarm_label = lv_label_align_create(mks_printer.user_src,advances_menu.flame_alarm,X_LEFT_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*2,LV_ALIGN_IN_TOP_LEFT,&advances_black_style);
  // advances_page.no_filament_label = lv_label_align_create(mks_printer.user_src,advances_menu.no_filament_tone_sound,X_LEFT_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*3,LV_ALIGN_IN_TOP_LEFT,&User_style.text_black_style);

  if(gCfgItems.advances_light)
    advances_page.btn_sound_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_on.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*0, event_handler, ID_V_LIGHT);
  else
    advances_page.btn_sound_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_off.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*0, event_handler, ID_V_LIGHT);

  if(gCfgItems.advances_flame)
    advances_page.print_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_on.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*2, event_handler, ID_V_FLAME);
  else
    advances_page.print_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_off.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*2, event_handler, ID_V_FLAME);


  if(gCfgItems.advances_5010fan)
  advances_page.endstop_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_on.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*1, event_handler, ID_V_FAN);
  else
  advances_page.endstop_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_off.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*1, event_handler, ID_V_FAN);

  advances_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src,"F:/png_back.bin",TITLE_BACK_X,TITLE_BACK_Y, event_handler, ID_V_BACK);
//  endstop_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src,"F:/png_back.bin",TITLE_BACK_X,TITLE_BACK_Y, event_handler, ID_E_BACK);

  // lv_imgbtn_set_src_both(advances_page.btn_sound_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(advances_page.endstop_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(advances_page.print_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(advances_page.no_filament_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(advances_page.back_imgbtn,  "F:/return.bin");
// lv_obj_t *lv_imgbtn_align_create(lv_obj_t *par, const char *img,lv_align_t align1,lv_coord_t x, lv_coord_t y, lv_event_cb_t cb, const int id)

  // lv_obj_t *lv_imgbtn_align_create(lv_obj_t *par, const char *img,lv_align_t align1,lv_coord_t x, lv_coord_t y, lv_event_cb_t cb, const int id)

}
void lv_fan5051()
{
  if (uiCfg.print_state == WORKING)
  {
    if(gCfgItems.advances_5010fan)
    {
      #if 0
      if((thermalManager.wholeDegBed() >= 80))
      {
        if(!open_fan)
        {
          thermalManager.set_fan_speed(1,255);
          open_fan = true;
        }
      }
      else
      {
          if(open_fan)
          {
            thermalManager.set_fan_speed(1,0);
            open_fan = false;
          }
      }
      #else
      if(print_time.hours > 0)
      {
        if(!open_fan && more_open_fan)
        {
          thermalManager.set_fan_speed(1,255);
          open_fan = true;
        }
      }

      if(!more_open_fan)
      {
        if((list_off_time != print_time.minutes) && (print_time.hours < 1))
        {
          more_open_fan = true;
        }
      }
      
      #endif
    }
  }
}

void lv_flame()
{
  if(gCfgItems.advances_flame)
  {
    if(!READ(MT_DET_2_PIN)) //火焰报警器触发
    {
      if(thermalManager.wholeDegHotend(0) >= 290)
      {
        flame_tri_flag = true;
        flame_beep_sound_start();
      }
      else
      {
        // flame_tri_flag = false;
        // flame_beep_sound_end();
      }
    }
    else
    {
      // flame_tri_flag = false;
      // flame_beep_sound_end();
    }
  }
  else
  {
    flame_beep_sound_end();
  }
}

void lv_clear_advances() {
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);

}

#endif // HAS_TFT_LVGL_UI
