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


#define first_btn_offset  134
#define btn_y_offset  69

typedef struct{
  
  lv_obj_t *x_endstop_label;
  lv_obj_t *y_endstop_label;
  lv_obj_t *z_endstop_label;
  lv_obj_t *det_break_label;
  lv_obj_t *flame_sensor_label;
  lv_obj_t *trigger_label;
  lv_obj_t *open_label;

  lv_obj_t *x_endstop_open_imgbtn;
  lv_obj_t *y_endstop_open_imgbtn;
  lv_obj_t *z_endstop_open_imgbtn;
  lv_obj_t *det_break_open_imgbtn;
  lv_obj_t *flame_sensor_open_imgbtn;

  lv_obj_t *x_endstop_trigger_imgbtn;
  lv_obj_t *y_endstop_trigger_imgbtn;
  lv_obj_t *z_endstop_trigger_imgbtn;
  lv_obj_t *det_break_trigger_imgbtn;
  lv_obj_t *flame_sensor_trigger_imgbtn;

  lv_obj_t *back_imgbtn;

}ENDSTOP_UI_PAGE;

ENDSTOP_UI_PAGE endstop_page;

enum {
  ID_E_X_OPEN = 1,
  ID_E_Y_OPEN,
  ID_E_Z_OPEN,
  ID_E_M_OPEN,

  ID_E_X_TRI,
  ID_E_Y_TRI,
  ID_E_Z_TRI,
  ID_E_M_TRI,

  ID_E_BACK
};

enum{
  ENDSTOP_STATE_ON = 1,
  ENDSTOP_STATE_OFF,
  ENDSTOP_STATE_NONE
};
enum{
  ENDSTOP_AXIS_X,
  ENDSTOP_AXIS_Y,
  ENDSTOP_AXIS_Z
};
typedef struct{
  uint8_t x = ENDSTOP_STATE_ON;
  uint8_t y = ENDSTOP_STATE_ON;
  uint8_t z = ENDSTOP_STATE_ON;
  uint8_t no_filament = ENDSTOP_STATE_ON;
  uint8_t flame = ENDSTOP_STATE_ON;
}ENDSTOP_DATA;

ENDSTOP_DATA CUR_ENDSTOP_STATE,LAST_ENDSTOP_STATE;
static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
    button_sound_end();
    lv_clear_endstop();
    switch (obj->mks_obj_id) {
      case ID_E_BACK: lv_draw_tool(); break;
    }
  }
}
void lv_draw_endstop() {
  static lv_style_t endstop_style;
  lv_style_copy(&endstop_style, &User_style.text_black_style);
  endstop_style.text.font         = &DengXian_16;


  lv_screen_distribution(ENDSTOP_UI);
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(ENDSTOP_UI),TITLE_LABEL_X);
  // mks_printer.user_src = lv_scr_create(mks_printer.user_src,0,TITLE_H,TITLE_W,TFT_HEIGHT-TITLE_H);
  lv_refr_now(lv_refr_get_disp_refreshing());

  endstop_page.x_endstop_label = lv_label_align_create(mks_printer.user_src,endstop_menu.x_endstop,10,first_btn_offset + btn_y_offset*0 ,LV_ALIGN_IN_TOP_LEFT,&endstop_style);
  endstop_page.y_endstop_label = lv_label_align_create(mks_printer.user_src,endstop_menu.y_endstop,10,first_btn_offset + btn_y_offset*1 ,LV_ALIGN_IN_TOP_LEFT,&endstop_style);
  endstop_page.z_endstop_label = lv_label_align_create(mks_printer.user_src,endstop_menu.z_endstop,10,first_btn_offset + btn_y_offset*2 ,LV_ALIGN_IN_TOP_LEFT,&endstop_style);
  endstop_page.det_break_label = lv_label_align_create(mks_printer.user_src,endstop_menu.det_break,10,first_btn_offset + btn_y_offset*3 ,LV_ALIGN_IN_TOP_LEFT,&endstop_style);
  endstop_page.flame_sensor_label = lv_label_align_create(mks_printer.user_src,endstop_menu.flame_sensor,10,first_btn_offset + btn_y_offset*4 ,LV_ALIGN_IN_TOP_LEFT,&endstop_style);

  // endstop_page.m_dection_label = lv_label_align_create(mks_printer.user_src,endstop_menu.m_dection,5,first_btn_offset + btn_y_offset*3 - 5,LV_ALIGN_IN_TOP_LEFT,&User_style.text_black_style);

  endstop_page.x_endstop_open_imgbtn = lv_img_align_create(mks_printer.user_src,endstop_page.x_endstop_open_imgbtn ,"F:/png_dis.bin",LV_ALIGN_IN_TOP_RIGHT,-10,first_btn_offset + btn_y_offset*0);
  endstop_page.y_endstop_open_imgbtn = lv_img_align_create(mks_printer.user_src,endstop_page.y_endstop_open_imgbtn,"F:/png_dis.bin",LV_ALIGN_IN_TOP_RIGHT,-10,first_btn_offset + btn_y_offset*1);
  endstop_page.z_endstop_open_imgbtn = lv_img_align_create(mks_printer.user_src,endstop_page.z_endstop_open_imgbtn,"F:/png_dis.bin",LV_ALIGN_IN_TOP_RIGHT,-10,first_btn_offset + btn_y_offset*2);
  endstop_page.det_break_open_imgbtn = lv_img_align_create(mks_printer.user_src,endstop_page.det_break_open_imgbtn,"F:/png_dis.bin",LV_ALIGN_IN_TOP_RIGHT,-10,first_btn_offset + btn_y_offset*3);
  endstop_page.flame_sensor_open_imgbtn = lv_img_align_create(mks_printer.user_src,endstop_page.flame_sensor_open_imgbtn,"F:/png_dis.bin",LV_ALIGN_IN_TOP_RIGHT,-10,first_btn_offset + btn_y_offset*4);

  // endstop_page.m_endstop_open_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_en.bin",LV_ALIGN_IN_TOP_RIGHT,-10,first_btn_offset + btn_y_offset*3, event_handler, ID_E_M_OPEN);

  endstop_page.x_endstop_trigger_imgbtn = lv_img_align_create(mks_printer.user_src,endstop_page.x_endstop_trigger_imgbtn,"F:/png_en.bin",LV_ALIGN_IN_TOP_RIGHT,-80,first_btn_offset + btn_y_offset*0);
  endstop_page.y_endstop_trigger_imgbtn = lv_img_align_create(mks_printer.user_src,endstop_page.y_endstop_trigger_imgbtn,"F:/png_en.bin",LV_ALIGN_IN_TOP_RIGHT,-80,first_btn_offset + btn_y_offset*1);
  endstop_page.z_endstop_trigger_imgbtn = lv_img_align_create(mks_printer.user_src,endstop_page.z_endstop_trigger_imgbtn,"F:/png_en.bin",LV_ALIGN_IN_TOP_RIGHT,-80,first_btn_offset + btn_y_offset*2);
  endstop_page.det_break_trigger_imgbtn = lv_img_align_create(mks_printer.user_src,endstop_page.det_break_trigger_imgbtn,"F:/png_en.bin",LV_ALIGN_IN_TOP_RIGHT,-80,first_btn_offset + btn_y_offset*3);
  endstop_page.flame_sensor_trigger_imgbtn = lv_img_align_create(mks_printer.user_src,endstop_page.flame_sensor_trigger_imgbtn,"F:/png_en.bin",LV_ALIGN_IN_TOP_RIGHT,-80,first_btn_offset + btn_y_offset*4);

  // endstop_page.m_endstop_trigger_imgbtn = lv_imgbtn_align_create(mks_printer.user_src,"F:/png_en.bin",LV_ALIGN_IN_TOP_RIGHT,-80,first_btn_offset + btn_y_offset*3, event_handler, ID_E_M_TRI);

  endstop_page.trigger_label =  lv_label_create(mks_printer.user_src, endstop_menu.trigger);
  lv_obj_align(endstop_page.trigger_label, endstop_page.x_endstop_trigger_imgbtn, LV_ALIGN_OUT_TOP_MID, 0 ,-30);
  lv_label_set_style(endstop_page.trigger_label,LV_LABEL_STYLE_MAIN,&endstop_style);
  endstop_page.open_label = lv_label_create(mks_printer.user_src, endstop_menu.open);
  lv_obj_align(endstop_page.open_label, endstop_page.x_endstop_open_imgbtn, LV_ALIGN_OUT_TOP_MID, 0 ,-30);
  lv_label_set_style(endstop_page.open_label,LV_LABEL_STYLE_MAIN,&endstop_style);

  endstop_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src,"F:/png_back.bin",TITLE_BACK_X,TITLE_BACK_Y, event_handler, ID_E_BACK);
  // tool_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_back.bin", TITLE_BACK_X, TITLE_BACK_Y, event_handler, ID_T_RETURN);

  if (READ(MT_DET_1_PIN) == MT_DET_PIN_STATE)
  {
        lv_img_set_src(endstop_page.det_break_trigger_imgbtn,"F:/png_en.bin");
        lv_img_set_src(endstop_page.det_break_open_imgbtn,"F:/png_dis.bin");
        LAST_ENDSTOP_STATE.no_filament = ENDSTOP_STATE_ON;
  }
  else
  {
        lv_img_set_src(endstop_page.det_break_trigger_imgbtn,"F:/png_dis.bin");
        lv_img_set_src(endstop_page.det_break_open_imgbtn,"F:/png_en.bin");
        LAST_ENDSTOP_STATE.no_filament = ENDSTOP_STATE_OFF;
  }
  if (READ(MT_DET_2_PIN) == MT_DET_PIN_STATE)
  {
        lv_img_set_src(endstop_page.flame_sensor_trigger_imgbtn,"F:/png_en.bin");
        lv_img_set_src(endstop_page.flame_sensor_open_imgbtn,"F:/png_dis.bin");
        LAST_ENDSTOP_STATE.flame = ENDSTOP_STATE_ON;
  }
  else
  {
        lv_img_set_src(endstop_page.flame_sensor_trigger_imgbtn,"F:/png_dis.bin");
        lv_img_set_src(endstop_page.flame_sensor_open_imgbtn,"F:/png_en.bin");
        LAST_ENDSTOP_STATE.flame = ENDSTOP_STATE_OFF;
  }
  // disp_x_endstop();
  // disp_y_endstop();
  // disp_z_endstop();
  disp_endstop();
  // if(!X_MIN_ENDSTOP_INVERTING)
  // {
  //   if(digitalRead(X_MIN_PIN))  //gao
  //    {
  //       lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_en.bin");
  //       lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_dis.bin");
  //       LAST_ENDSTOP_STATE.x = ENDSTOP_STATE_ON;
  //    }
  //    else
  //    {
  //       lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_dis.bin");
  //       lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_en.bin");
  //       LAST_ENDSTOP_STATE.x = ENDSTOP_STATE_OFF;
  //    }
  // }
  // else
  // {
  //   if(!digitalRead(X_MIN_PIN))  //di
  //    {
  //       lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_en.bin");
  //       lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_dis.bin");
  //       LAST_ENDSTOP_STATE.x = ENDSTOP_STATE_ON;
  //    }
  //    else
  //    {
  //       lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_dis.bin");
  //       lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_en.bin");
  //       LAST_ENDSTOP_STATE.x = ENDSTOP_STATE_OFF;
  //    }
  // }

 
  // if(!Y_MIN_ENDSTOP_INVERTING)
  // {
  //   if(digitalRead(Y_MIN_PIN))  //gao
  //    {
  //       lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_en.bin");
  //       lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_dis.bin");
  //       LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_ON;
  //    }
  //    else
  //    {
  //       lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_dis.bin");
  //       lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_en.bin");
  //       LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_OFF;
  //    }
  // }
  // else
  // {
  //   if(!digitalRead(Y_MIN_PIN))  //di
  //    {
  //       lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_en.bin");
  //       lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_dis.bin");
  //       LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_ON;
  //    }
  //    else
  //    {
  //       lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_dis.bin");
  //       lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_en.bin");
  //       LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_OFF;
  //    }
  // }

  // if(!Z_MIN_ENDSTOP_INVERTING)
  // {
  //   if(digitalRead(Z_MIN_PIN))  //gao
  //    {
  //       lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_en.bin");
  //       lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_dis.bin");
  //       LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_ON;
  //    }
  //    else
  //    {
  //       lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_dis.bin");
  //       lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_en.bin");
  //       LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_OFF;
  //    }
  // }
  // else
  // {
  //   if(!digitalRead(Z_MIN_PIN))  //di
  //    {
  //       lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_en.bin");
  //       lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_dis.bin");
  //       LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_ON;
  //    }
  //    else
  //    {
  //       lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_dis.bin");
  //       lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_en.bin");
  //       LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_OFF;
  //    }
  // }


}
void disp_flame()
{
  if (READ(MT_DET_2_PIN) == MT_DET_PIN_STATE)
  {
      CUR_ENDSTOP_STATE.flame = ENDSTOP_STATE_ON;
      if(CUR_ENDSTOP_STATE.flame != LAST_ENDSTOP_STATE.flame)
       {
        lv_img_set_src(endstop_page.flame_sensor_trigger_imgbtn,"F:/png_en.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());

        lv_img_set_src(endstop_page.flame_sensor_open_imgbtn,"F:/png_dis.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        LAST_ENDSTOP_STATE.flame = ENDSTOP_STATE_ON;
       }
  }
  else
  {
      CUR_ENDSTOP_STATE.flame = ENDSTOP_STATE_OFF;
      if(CUR_ENDSTOP_STATE.flame != LAST_ENDSTOP_STATE.flame)
       {
        lv_img_set_src(endstop_page.flame_sensor_trigger_imgbtn,"F:/png_dis.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        lv_img_set_src(endstop_page.flame_sensor_open_imgbtn,"F:/png_en.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        LAST_ENDSTOP_STATE.flame = ENDSTOP_STATE_OFF;
       }
  }
}
void disp_no_filament()
{
  if (READ(MT_DET_1_PIN) == MT_DET_PIN_STATE)
  {
      CUR_ENDSTOP_STATE.no_filament = ENDSTOP_STATE_ON;
      if(CUR_ENDSTOP_STATE.no_filament != LAST_ENDSTOP_STATE.no_filament)
       {
        lv_img_set_src(endstop_page.det_break_trigger_imgbtn,"F:/png_en.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        lv_img_set_src(endstop_page.det_break_open_imgbtn,"F:/png_dis.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        LAST_ENDSTOP_STATE.no_filament = ENDSTOP_STATE_ON;
       }
  }
  else
  {
        CUR_ENDSTOP_STATE.no_filament = ENDSTOP_STATE_OFF;
        if(CUR_ENDSTOP_STATE.no_filament != LAST_ENDSTOP_STATE.no_filament)
        {
        lv_img_set_src(endstop_page.det_break_trigger_imgbtn,"F:/png_dis.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        lv_img_set_src(endstop_page.det_break_open_imgbtn,"F:/png_en.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        LAST_ENDSTOP_STATE.no_filament = ENDSTOP_STATE_OFF;
        }
  }
}

void disp_x_endstop()
{
  if(!X_MIN_ENDSTOP_INVERTING){
    if(!READ(X_MIN_PIN))
    {
      delay(100);
      if(!READ(X_MIN_PIN))
        LAST_ENDSTOP_STATE.x = 1;
    }
    else
    {
      delay(100);
      if(READ(X_MIN_PIN))
        LAST_ENDSTOP_STATE.x = 2;
    }
    if(LAST_ENDSTOP_STATE.x != CUR_ENDSTOP_STATE.x){
    switch (LAST_ENDSTOP_STATE.x)
    {
    case 1:
      //if(!digitalRead(X_MIN_PIN))
      {
          lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_dis.bin");
          lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_en.bin");
          //LAST_ENDSTOP_STATE.x = ENDSTOP_STATE_OFF;
          // LAST_ENDSTOP_STATE.x = 3;
          CUR_ENDSTOP_STATE.x = 1;
      }
      break;
    case 2:
      //if(digitalRead(X_MIN_PIN))
      {
          lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_en.bin");
          lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_dis.bin");
          //LAST_ENDSTOP_STATE.x = ENDSTOP_STATE_ON;
          // LAST_ENDSTOP_STATE.x = 3;
          CUR_ENDSTOP_STATE.x = 2;
      }
      break;
    default:
      break;
    }
  }
  }else
  {
    if(READ(X_MIN_PIN))
    {
      delay(100);
      if(READ(X_MIN_PIN))
      LAST_ENDSTOP_STATE.x = 1;
    }
    else
    {
      delay(100);
      if(!READ(X_MIN_PIN))
      LAST_ENDSTOP_STATE.x = 2;
    }
  switch (LAST_ENDSTOP_STATE.x)
  {
  case 1:
    //if(!digitalRead(X_MIN_PIN))
    {
        lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_dis.bin");
        lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_en.bin");
        //LAST_ENDSTOP_STATE.x = ENDSTOP_STATE_OFF;
        LAST_ENDSTOP_STATE.x = 3;
    }
    break;
  case 2:
    //if(digitalRead(X_MIN_PIN))
    {
        lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_en.bin");
        lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_dis.bin");
        //LAST_ENDSTOP_STATE.x = ENDSTOP_STATE_ON;
        LAST_ENDSTOP_STATE.x = 3;
    }
    break;
  default:
    break;
  }
  // if(!X_MIN_ENDSTOP_INVERTING)
  // {
  //   if(digitalRead(X_MIN_PIN))  //gao
  //    {
  //      CUR_ENDSTOP_STATE.x = ENDSTOP_STATE_ON;
  //      if(CUR_ENDSTOP_STATE.x != LAST_ENDSTOP_STATE.x)
  //      {
  //       lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_en.bin");
  //       lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_dis.bin");
  //       LAST_ENDSTOP_STATE.x = ENDSTOP_STATE_ON;
  //       lv_refr_now(lv_refr_get_disp_refreshing());

  //      }
  //    }
  //    else
  //    {
  //      CUR_ENDSTOP_STATE.x = ENDSTOP_STATE_OFF;
  //      if(CUR_ENDSTOP_STATE.x != LAST_ENDSTOP_STATE.x)
  //      {
  //       lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_dis.bin");
  //       lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_en.bin");
  //       LAST_ENDSTOP_STATE.x = ENDSTOP_STATE_OFF;
  //         lv_refr_now(lv_refr_get_disp_refreshing());

  //      }
  //    }
  // }
  // else
  // {

  // }
  }

}
// void disp_y_endstop()
// {
//   if(!Y_MIN_ENDSTOP_INVERTING)
//   {
//     if(digitalRead(Y_MIN_PIN))  //gao
//      {
//        CUR_ENDSTOP_STATE.y = ENDSTOP_STATE_ON;
//        if(CUR_ENDSTOP_STATE.y != LAST_ENDSTOP_STATE.y)
//        {
//         lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_en.bin");
//         lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_dis.bin");
//         LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_ON;
//           lv_refr_now(lv_refr_get_disp_refreshing());

//        }
//      }
//      else
//      {
//        CUR_ENDSTOP_STATE.y = ENDSTOP_STATE_OFF;
//        if(CUR_ENDSTOP_STATE.y != LAST_ENDSTOP_STATE.y)
//        {
//         lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_dis.bin");
//         lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_en.bin");
//         LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_OFF;
//           lv_refr_now(lv_refr_get_disp_refreshing());

//        }
//      }
//   }
//   else
//   {
//     if(!digitalRead(Y_MIN_PIN))  //di
//      {
//        CUR_ENDSTOP_STATE.y = ENDSTOP_STATE_ON;
//        if(CUR_ENDSTOP_STATE.y != LAST_ENDSTOP_STATE.y)
//        {
//         lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_en.bin");
//         lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_dis.bin");
//         LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_ON;
//           lv_refr_now(lv_refr_get_disp_refreshing());

//        }
//      }
//      else
//      {
//        CUR_ENDSTOP_STATE.y = ENDSTOP_STATE_OFF;
//        if(CUR_ENDSTOP_STATE.y != LAST_ENDSTOP_STATE.y)
//        {
//         lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_dis.bin");
//         lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_en.bin");
//         LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_OFF;
//           lv_refr_now(lv_refr_get_disp_refreshing());

//        }
//      }
//   }

// }

void disp_endstop_img(int axis,uint8_t state)
{
  switch (axis)
  {
  case ENDSTOP_AXIS_X:
    if(state)
    {
        lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_en.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_dis.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
    }
    else
    {
        lv_img_set_src(endstop_page.x_endstop_trigger_imgbtn,"F:/png_dis.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        lv_img_set_src(endstop_page.x_endstop_open_imgbtn,"F:/png_en.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
    }
    break;
  case ENDSTOP_AXIS_Y:
    if(state)
    {
        lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_en.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_dis.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
    }
    else
    {
        lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_dis.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_en.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
    }
    break;
  case ENDSTOP_AXIS_Z:
    if(state)
    {
        lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_en.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_dis.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
    }
    else
    {
        lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_dis.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
        lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_en.bin");
        lv_refr_now(lv_refr_get_disp_refreshing());
    }
    break;
  default:
    break;
  }
}
void disp_endstop()
{
  uint8_t mask = 0;
  mask = READ(X_MIN_PIN);
  mask |= READ(Y_MIN_PIN) << 1;
  mask |= READ(Z_MIN_PIN) << 2;

  if(!Z_MIN_ENDSTOP_INVERTING)
    disp_endstop_img(ENDSTOP_AXIS_Z,mask&0x04?1:0);
  else
    disp_endstop_img(ENDSTOP_AXIS_Z,mask&0x04?0:1);

  // lv_refr_now(lv_refr_get_disp_refreshing());

  if(!X_MIN_ENDSTOP_INVERTING)
    disp_endstop_img(ENDSTOP_AXIS_X,mask&0x01?1:0);
  else
    disp_endstop_img(ENDSTOP_AXIS_X,mask&0x01?0:1);

  // lv_refr_now(lv_refr_get_disp_refreshing());

  if(!Y_MIN_ENDSTOP_INVERTING)
    disp_endstop_img(ENDSTOP_AXIS_Y,mask&0x02?1:0);
  else
    disp_endstop_img(ENDSTOP_AXIS_Y,mask&0x02?0:1);
  
  
}
void disp_y_endstop()
{
  if(!Y_MIN_ENDSTOP_INVERTING){
    if(!READ(Y_MIN_PIN))
    {
      delay(100);
      if(!READ(Y_MIN_PIN))
        LAST_ENDSTOP_STATE.y = 1;
    }
    else
    {
      delay(100);
      if(READ(Y_MIN_PIN))
        LAST_ENDSTOP_STATE.y = 2;
    }
  switch (LAST_ENDSTOP_STATE.y)
  {
  case 1:
    //if(!digitalRead(Y_MIN_PIN))
    {
        lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_dis.bin");
        lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_en.bin");
        //LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_OFF;
        LAST_ENDSTOP_STATE.y = 3;
    }
    break;
  case 2:
    //if(digitalRead(Y_MIN_PIN))
    {
        lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_en.bin");
        lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_dis.bin");
        //LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_ON;
        LAST_ENDSTOP_STATE.y = 3;
    }
    break;
  default:
    break;
  }
  }else
  {
    if(READ(Y_MIN_PIN))
    {
      delay(100);
      if(READ(Y_MIN_PIN))
      LAST_ENDSTOP_STATE.y = 1;
    }
    else
    {
      delay(100);
      if(!READ(Y_MIN_PIN))
      LAST_ENDSTOP_STATE.y = 2;
    }
  switch (LAST_ENDSTOP_STATE.y)
  {
  case 1:
    //if(!digitalRead(Y_MIN_PIN))
    {
        lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_dis.bin");
        lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_en.bin");
        //LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_OFF;
        LAST_ENDSTOP_STATE.y = 3;
    }
    break;
  case 2:
    //if(digitalRead(Y_MIN_PIN))
    {
        lv_img_set_src(endstop_page.y_endstop_trigger_imgbtn,"F:/png_en.bin");
        lv_img_set_src(endstop_page.y_endstop_open_imgbtn,"F:/png_dis.bin");
        //LAST_ENDSTOP_STATE.y = ENDSTOP_STATE_ON;
        LAST_ENDSTOP_STATE.y = 3;
    }
    break;
  default:
    break;
  }
  }

}
// void disp_z_endstop()
// {
//   if(!Z_MIN_ENDSTOP_INVERTING)
//   {
//     if(digitalRead(Z_MIN_PIN))  //gao
//      {
//        CUR_ENDSTOP_STATE.z = ENDSTOP_STATE_ON;
//        if(CUR_ENDSTOP_STATE.z != LAST_ENDSTOP_STATE.z)
//        {
//         lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_en.bin");
//         lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_dis.bin");
//         LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_ON;
//           lv_refr_now(lv_refr_get_disp_refreshing());

//        }
//      }
//      else
//      {
//        CUR_ENDSTOP_STATE.z = ENDSTOP_STATE_OFF;
//        if(CUR_ENDSTOP_STATE.z != LAST_ENDSTOP_STATE.z)
//        {
//         lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_dis.bin");
//         lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_en.bin");
//         LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_OFF;
//           lv_refr_now(lv_refr_get_disp_refreshing());
//        }
//      }
//   }
//   else
//   {
//     if(!digitalRead(Z_MIN_PIN))  //gao
//      {
//        CUR_ENDSTOP_STATE.z = ENDSTOP_STATE_ON;
//        if(CUR_ENDSTOP_STATE.z != LAST_ENDSTOP_STATE.z)
//        {
//         lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_en.bin");
//         lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_dis.bin");
//         LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_ON;
//           lv_refr_now(lv_refr_get_disp_refreshing());

//        }
//      }
//      else
//      {
//        CUR_ENDSTOP_STATE.z = ENDSTOP_STATE_OFF;
//        if(CUR_ENDSTOP_STATE.z != LAST_ENDSTOP_STATE.z)
//        {
//         lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_dis.bin");
//         lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_en.bin");
//         LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_OFF;
//           lv_refr_now(lv_refr_get_disp_refreshing());
//        }
//      }
//   }

// }

void disp_z_endstop()
{
  if(!Z_MIN_ENDSTOP_INVERTING){
    if(!READ(Z_MIN_PIN))
    {
      delay(100);
      if(!READ(Z_MIN_PIN))
        LAST_ENDSTOP_STATE.z = 1;
    }
    else
    {
      delay(100);
      if(READ(Z_MIN_PIN))
        LAST_ENDSTOP_STATE.z = 2;
    }
  switch (LAST_ENDSTOP_STATE.z)
  {
  case 1:
    //if(!digitalRead(Z_MIN_PIN))
    {
        lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_dis.bin");
        lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_en.bin");
        //LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_OFF;
        LAST_ENDSTOP_STATE.z = 3;
    }
    break;
  case 2:
    //if(digitalRead(Z_MIN_PIN))
    {
        lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_en.bin");
        lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_dis.bin");
        //LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_ON;
        LAST_ENDSTOP_STATE.z = 3;
    }
    break;
  default:
    break;
  }
  }else
  {
    if(READ(Z_MIN_PIN))
    {
      delay(100);
      if(READ(Z_MIN_PIN))
      LAST_ENDSTOP_STATE.z = 1;
    }
    else
    {
      delay(100);
      if(!READ(Z_MIN_PIN))
      LAST_ENDSTOP_STATE.z = 2;
    }
  switch (LAST_ENDSTOP_STATE.z)
  {
  case 1:
    //if(!digitalRead(Z_MIN_PIN))
    {
        lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_dis.bin");
        lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_en.bin");
        //LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_OFF;
        LAST_ENDSTOP_STATE.z = 3;
    }
    break;
  case 2:
    //if(digitalRead(Z_MIN_PIN))
    {
        lv_img_set_src(endstop_page.z_endstop_trigger_imgbtn,"F:/png_en.bin");
        lv_img_set_src(endstop_page.z_endstop_open_imgbtn,"F:/png_dis.bin");
        //LAST_ENDSTOP_STATE.z = ENDSTOP_STATE_ON;
        LAST_ENDSTOP_STATE.z = 3;
    }
    break;
  default:
    break;
  }
  }

}
void lv_clear_endstop() {
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);

}

#endif // HAS_TFT_LVGL_UI
