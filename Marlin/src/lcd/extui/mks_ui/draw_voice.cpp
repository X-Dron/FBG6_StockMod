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

typedef struct{
  
  lv_obj_t *btn_sound_label;
  lv_obj_t *endstop_label;
  lv_obj_t *print_label;
  lv_obj_t *no_filament_label;

  lv_obj_t *btn_sound_imgbtn;
  lv_obj_t *endstop_imgbtn;
  lv_obj_t *print_imgbtn;
  lv_obj_t *no_filament_imgbtn;

  lv_obj_t *back_imgbtn;

}VOICE_UI_PAGE;
VOICE_UI_PAGE voice_page;

typedef enum{
  BTN_Y_FIRST_OFFSET = 96,
  BTN_Y_OFFSET = 97,
  X_LEFT_ALIGN = 10,
  X_RIGTH_ALIGN = -10
}VOICE_BTN_LAOUT;

typedef struct{
  
  bool button_flag = true;

  bool endstop_set_flag = true;
  bool endstop_flag = false;

  bool print_tone_set_flag = true;
  bool print_tone_flag = false;

  bool no_filament_set_flag = true;
  bool no_filament_flag = false;

  uint16_t endstop_start_time = 0;
  uint16_t endstop_end_time = 0;
  uint16_t print_tone_start_time = 0;
  uint16_t print_tone_end_time = 0;
  uint16_t no_filament_start_time = 0;
  uint16_t no_filament_end_time = 0;

}VOICE_UI_DATA;
VOICE_UI_DATA voice_data;

void button_sound_start()
{
  if(gCfgItems.voice_bottom)
  {
      digitalWrite(BEEPER_PIN, HIGH);
  }
}
void button_sound_end()
{
  if(gCfgItems.voice_bottom)
  {
    digitalWrite(BEEPER_PIN, LOW);
  }
}

void endstop_sound_start()
{
  char buff[20] ;
    voice_data.endstop_flag = true;
    if(gCfgItems.voice_endstop)
    {
        digitalWrite(BEEPER_PIN, HIGH);
        voice_data.endstop_start_time = millis();
        // sprintf_P(buff, PSTR(" %d"), voice_data.endstop_start_time );
        // SERIAL_ECHO_MSG("start",((const char *)buff),"\r\n");
    }
    else
      voice_data.endstop_flag = false;
}
void endstop_sound_end()
{
  char buff[20] ;
  if(voice_data.endstop_flag)
  {
    if(gCfgItems.voice_endstop)
    {
      voice_data.endstop_end_time = millis();
      // sprintf_P(buff, PSTR(" %d"), voice_data.endstop_end_time );
      // SERIAL_ECHO_MSG("end",((const char *)buff),"\r\n");
      if((voice_data.endstop_end_time - voice_data.endstop_start_time) >= 500)
      {
        digitalWrite(BEEPER_PIN, LOW);
        voice_data.endstop_end_time = 0;
        voice_data.endstop_start_time = 0;
        voice_data.endstop_flag = false;
      }
    }
    else
      digitalWrite(BEEPER_PIN, LOW);
  }
}

void print_tone_sound_start()
{
  char buff[20] ;
    voice_data.print_tone_flag = true;
    if(gCfgItems.voice_print_complete)
    {
        digitalWrite(BEEPER_PIN, HIGH);
        voice_data.print_tone_start_time = millis();
        // sprintf_P(buff, PSTR(" %d"), voice_data.endstop_start_time );
        // SERIAL_ECHO_MSG("start",((const char *)buff),"\r\n");
    }
    else
      voice_data.print_tone_flag = false;
}
void print_tone_sound_end()
{
  char buff[20] ;
  if(voice_data.print_tone_flag)
  {
    if(gCfgItems.voice_print_complete)
    {
      voice_data.print_tone_end_time = millis();
      // sprintf_P(buff, PSTR(" %d"), voice_data.endstop_end_time );
      // SERIAL_ECHO_MSG("end",((const char *)buff),"\r\n");
      if((voice_data.print_tone_end_time - voice_data.print_tone_start_time) >= 500)
      {
        digitalWrite(BEEPER_PIN, LOW);
        voice_data.print_tone_start_time = 0;
        voice_data.print_tone_end_time = 0;
        voice_data.print_tone_flag = false;
      }
    }
    else
    {
        digitalWrite(BEEPER_PIN, LOW);
    }
    
  }
}

void no_filament_sound_start()
{
  char buff[20] ;
    voice_data.no_filament_flag = true;
    if(gCfgItems.voice_no_filament)
    {
        digitalWrite(BEEPER_PIN, HIGH);
        voice_data.no_filament_start_time = millis();
        // sprintf_P(buff, PSTR(" %d"), voice_data.endstop_start_time );
        // SERIAL_ECHO_MSG("start",((const char *)buff),"\r\n");
    }
    else
      voice_data.no_filament_flag = false;

}
void no_filament_sound_end()
{
  char buff[20] ;
  if(voice_data.no_filament_flag)
  {
    if(gCfgItems.voice_no_filament)
    {
      voice_data.no_filament_end_time = millis();
      // sprintf_P(buff, PSTR(" %d"), voice_data.endstop_end_time );
      // SERIAL_ECHO_MSG("end",((const char *)buff),"\r\n");
      if((voice_data.no_filament_end_time - voice_data.no_filament_start_time) >= 500)
      {
        digitalWrite(BEEPER_PIN, LOW);
        voice_data.no_filament_end_time = 0;
        voice_data.no_filament_start_time = 0;
        voice_data.no_filament_flag = false;
      }
    }
    else
    {
        digitalWrite(BEEPER_PIN, LOW);
    }
    
  }
}

enum {
  ID_V_BTN = 1,
  ID_V_ENDSTOP,
  ID_V_TONE,
  ID_V_NO_FILAMENT,
  ID_V_BACK
};



static void event_handler(lv_obj_t *obj, lv_event_t event) {
  
 if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
    button_sound_end();
    
    switch (obj->mks_obj_id) {
      case ID_V_BACK:   lv_clear_voice(); lv_draw_tool(); break;
      case ID_V_BTN:
      if(gCfgItems.voice_bottom)
      {
        gCfgItems.voice_bottom = false;
        lv_imgbtn_set_src_both(voice_page.btn_sound_imgbtn,"F:/png_off.bin");
      }
      else
      {
        gCfgItems.voice_bottom = true;
        lv_imgbtn_set_src_both(voice_page.btn_sound_imgbtn,"F:/png_on.bin");
      }
        break;
      case ID_V_TONE:
      if(gCfgItems.voice_print_complete)
      {
        gCfgItems.voice_print_complete = false;
        lv_imgbtn_set_src_both(voice_page.print_imgbtn,"F:/png_off.bin");
      }
      else
      {
        gCfgItems.voice_print_complete = true;
        lv_imgbtn_set_src_both(voice_page.print_imgbtn,"F:/png_on.bin");
      }
      break;
      case ID_V_NO_FILAMENT:
      if(gCfgItems.voice_no_filament)
      {
        gCfgItems.voice_no_filament = false;
        lv_imgbtn_set_src_both(voice_page.no_filament_imgbtn,"F:/png_off.bin");
      }
      else
      {
        gCfgItems.voice_no_filament = true;
        lv_imgbtn_set_src_both(voice_page.no_filament_imgbtn,"F:/png_on.bin");
      }
      break;
      case ID_V_ENDSTOP:
       if(gCfgItems.voice_endstop)
      {
        gCfgItems.voice_endstop = false;
        lv_imgbtn_set_src_both(voice_page.endstop_imgbtn,"F:/png_off.bin");
        digitalWrite(BEEPER_PIN, LOW);
      }
      else
      {
        gCfgItems.voice_endstop = true;
        lv_imgbtn_set_src_both(voice_page.endstop_imgbtn,"F:/png_on.bin");
      }
      break;
    }
      update_spi_flash();
  }
}
void lv_draw_voice() {
  static lv_style_t voice_style;
  lv_style_copy(&voice_style, &User_style.text_black_style);
  voice_style.text.font         = &DengXian_16;

  lv_screen_distribution(VOICE_UI);


  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(VOICE_UI),TITLE_LABEL_X);
  // mks_printer.user_src = lv_scr_create(mks_printer.user_src,0,TITLE_H,TITLE_W,TFT_HEIGHT-TITLE_H);
  lv_refr_now(lv_refr_get_disp_refreshing());

  voice_page.btn_sound_label = lv_label_align_create(mks_printer.user_src,voice_menu.btn_sound,X_LEFT_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*0-25,LV_ALIGN_IN_TOP_LEFT,&voice_style);//
  voice_page.endstop_label = lv_label_align_create(mks_printer.user_src,voice_menu.endstop_sound,X_LEFT_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*1-25,LV_ALIGN_IN_TOP_LEFT,&voice_style);//
  voice_page.print_label = lv_label_align_create(mks_printer.user_src,voice_menu.print_tone_sound,X_LEFT_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*2-25,LV_ALIGN_IN_TOP_LEFT,&voice_style);//
  voice_page.no_filament_label = lv_label_align_create(mks_printer.user_src,voice_menu.no_filament_tone_sound,X_LEFT_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*3-25,LV_ALIGN_IN_TOP_LEFT,&voice_style);

  if(gCfgItems.voice_bottom)
    voice_page.btn_sound_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_on.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*0, event_handler, ID_V_BTN);
  else
    voice_page.btn_sound_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_off.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*0, event_handler, ID_V_BTN);

  if(gCfgItems.voice_print_complete)
    voice_page.print_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_on.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*2, event_handler, ID_V_TONE);
  else
    voice_page.print_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_off.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*2, event_handler, ID_V_TONE);

   if(gCfgItems.voice_no_filament)
      voice_page.no_filament_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_on.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*3, event_handler, ID_V_NO_FILAMENT);
  else
      voice_page.no_filament_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_off.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*3, event_handler, ID_V_NO_FILAMENT);

  if(gCfgItems.voice_endstop)
  voice_page.endstop_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_on.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*1, event_handler, ID_V_ENDSTOP);
  else
  voice_page.endstop_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_off.bin",LV_ALIGN_IN_TOP_RIGHT,X_RIGTH_ALIGN,BTN_Y_FIRST_OFFSET + BTN_Y_OFFSET*1, event_handler, ID_V_ENDSTOP);

  voice_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src,"F:/png_back.bin",TITLE_BACK_X,TITLE_BACK_Y, event_handler, ID_V_BACK);
//  endstop_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src,"F:/png_back.bin",TITLE_BACK_X,TITLE_BACK_Y, event_handler, ID_E_BACK);

  // lv_imgbtn_set_src_both(voice_page.btn_sound_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(voice_page.endstop_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(voice_page.print_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(voice_page.no_filament_imgbtn,  "F:/png_on.bin");
  // lv_imgbtn_set_src_both(voice_page.back_imgbtn,  "F:/return.bin");
// lv_obj_t *lv_imgbtn_align_create(lv_obj_t *par, const char *img,lv_align_t align1,lv_coord_t x, lv_coord_t y, lv_event_cb_t cb, const int id)

  // lv_obj_t *lv_imgbtn_align_create(lv_obj_t *par, const char *img,lv_align_t align1,lv_coord_t x, lv_coord_t y, lv_event_cb_t cb, const int id)

}

void lv_clear_voice() {
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);

}

#endif // HAS_TFT_LVGL_UI
