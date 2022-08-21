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

#include "../../../module/temperature.h"
#include "../../../gcode/gcode.h"
#include "../../../module/motion.h"
#include "../../../module/planner.h"
#include "../../../inc/MarlinConfig.h"

extern lv_group_t *g;
extern bool gcode_preview_over, flash_preview_begin, default_preview_flg;
extern bool no_filamet_print_flag;
static lv_obj_t *scr;
static lv_obj_t *buttonType;
static lv_obj_t *labelType;
static lv_obj_t *tempText1;

typedef struct{
  
  lv_obj_t *add_imgbtn;
  lv_obj_t *dec_imgbtn;
  lv_obj_t *heat_imgbtn;
  lv_obj_t *heat_label;
  lv_obj_t *stop_imgbtn;
  lv_obj_t *stop_label;
  lv_obj_t *step1_btn;
  lv_obj_t *step5_btn;
  lv_obj_t *step10_btn;
  lv_obj_t *step1_label;
  lv_obj_t *step5_label;
  lv_obj_t *step10_label;
  lv_obj_t *png_loop_off_imgbtn;
  lv_obj_t *speed_label;
  lv_obj_t *speed_imgbtn;
  lv_obj_t *ex_img;

  lv_obj_t *temp_label;
  lv_obj_t *back_imgbtn;
  lv_style_t step_style;
  lv_style_t step_style_rel;

  lv_style_t heat_bg_style;
  lv_style_t stop_bg_style;

}FILAMNT_UI_PAGE;

FILAMNT_UI_PAGE filamnt_page;

FILAMNT_DATA fila_data;
enum {
  ID_FILAMNT_IN = 1,
  ID_FILAMNT_OUT,
  ID_FILAMNT_TYPE,
  ID_FILAMNT_HEAT,
  ID_FILAMNT_STOP,
  ID_FILAMNT_STEP,
  ID_FILAMNT_STEP1,
  ID_FILAMNT_STEP5,
  ID_FILAMNT_STEP10,
  ID_FILAMNT_png_loop_off,
  ID_FILAMNT_RETURN
};
// static int32_t extrudeAmount;


static void event_handler(lv_obj_t *obj, lv_event_t event) {
   char buff[50];
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
    button_sound_end();
    switch (obj->mks_obj_id) {
       case ID_FILAMNT_IN:
      if (ABS(thermalManager.degTargetHotend(uiCfg.extruderIndex) - thermalManager.wholeDegHotend(uiCfg.extruderIndex)) <= 10
          || gCfgItems.filament_limit_temp <= thermalManager.wholeDegHotend(uiCfg.extruderIndex)
        ) {
        
        if(fila_data.contimued)
        {
            WRITE(E0_DIR_PIN,1);
            OUT_WRITE(E0_ENABLE_PIN, LOW);
            fila_data.contimued_open = true;
        }
        else
        {
          WRITE(E0_DIR_PIN,1);
          sprintf_P((char *)public_buf_l, PSTR("G91\nG1 E%d F%d\nG90"), uiCfg.extruStep, 60 * uiCfg.extruSpeed);
          queue.inject(public_buf_l);
        }

      }
      break;
    case ID_FILAMNT_OUT:
       if (thermalManager.degTargetHotend(uiCfg.extruderIndex)
            && (ABS(thermalManager.degTargetHotend(uiCfg.extruderIndex) - thermalManager.wholeDegHotend(uiCfg.extruderIndex)) <= 10
                || thermalManager.wholeDegHotend(uiCfg.extruderIndex) >= gCfgItems.filament_limit_temp)
        ) {
       
         if(fila_data.contimued)
        {
            // if((fila_data.contimued_open)&&(fila_data.contimued_dir)){
            // WRITE(E0_DIR_PIN,1);
            // sprintf_P((char *)public_buf_l, PSTR("G91\nG1 E%d F%d\nG90"), 3, (gCfgItems.filamentchange_load_speed - 850));
            //  gcode.process_subcommands_now(public_buf_l);
            // }
            WRITE(E0_DIR_PIN,0);
            OUT_WRITE(E0_ENABLE_PIN, LOW);
            fila_data.contimued_open = true;
        }
        else
        {
          WRITE(E0_DIR_PIN,0);
          sprintf_P((char *)public_buf_l, PSTR("G91\nG1 E%d F%d\nG90"), 0 - uiCfg.extruStep, 60 * uiCfg.extruSpeed);
          queue.inject(public_buf_l);
        }
      }
      break;
      case ID_FILAMNT_TYPE:
        #if HAS_MULTI_EXTRUDER
          uiCfg.extruderIndex = !uiCfg.extruderIndex;
        #endif
        disp_filament_type();
        break;
      case ID_FILAMNT_HEAT:
      if (thermalManager.degTargetHotend(uiCfg.extruderIndex) < gCfgItems.filament_limit_temp) {
            thermalManager.setTargetHotend(gCfgItems.filament_limit_temp, uiCfg.extruderIndex);
            thermalManager.start_watching_hotend(uiCfg.extruderIndex);
          }

      break;
      case ID_FILAMNT_STEP1:
          if(fila_data.contimued == false)
          {lv_btn_set_style_both(filamnt_page.step1_btn,&filamnt_page.step_style_rel);
          lv_btn_set_style_both(filamnt_page.step5_btn,&filamnt_page.step_style);
          lv_btn_set_style_both(filamnt_page.step10_btn,&filamnt_page.step_style);
          uiCfg.extruStep = EX_LOW_SPEED;}
      break;
      case ID_FILAMNT_STEP5:
          if(fila_data.contimued == false)
          {lv_btn_set_style_both(filamnt_page.step1_btn,&filamnt_page.step_style);
          lv_btn_set_style_both(filamnt_page.step5_btn,&filamnt_page.step_style_rel);
          lv_btn_set_style_both(filamnt_page.step10_btn,&filamnt_page.step_style);
          uiCfg.extruStep = EX_MID_SPEED;}
      break;
      case ID_FILAMNT_STEP10:
          if(fila_data.contimued == false)
          {lv_btn_set_style_both(filamnt_page.step1_btn,&filamnt_page.step_style);
          lv_btn_set_style_both(filamnt_page.step5_btn,&filamnt_page.step_style);
          lv_btn_set_style_both(filamnt_page.step10_btn,&filamnt_page.step_style_rel);
          uiCfg.extruStep = EX_HIGH_SPEED;}
      break;
      case ID_FILAMNT_STOP:
      fila_data.contimued_open = false;
      // queue.clear();
      thermalManager.setTargetHotend(uiCfg.hotendTargetTempBak, uiCfg.extruderIndex);
      break;
      case ID_FILAMNT_png_loop_off:
          if(!fila_data.contimued)
          {
            fila_data.contimued = true;
            lv_btn_set_style_both(filamnt_page.step1_btn,&filamnt_page.step_style);
            lv_btn_set_style_both(filamnt_page.step5_btn,&filamnt_page.step_style);
            lv_btn_set_style_both(filamnt_page.step10_btn,&filamnt_page.step_style);
            lv_imgbtn_set_src_both(filamnt_page.png_loop_off_imgbtn,"F:/png_loop_on.bin");
          }
          else
          {
            if(fila_data.contimued_open){
              fila_data.contimued_open = false;
            }
            fila_data.contimued = false;
            lv_imgbtn_set_src_both(filamnt_page.png_loop_off_imgbtn,"F:/png_loop_off.bin");
            if(uiCfg.extruStep == EX_LOW_SPEED)
            {
              lv_btn_set_style_both(filamnt_page.step1_btn,&filamnt_page.step_style_rel);
              lv_btn_set_style_both(filamnt_page.step5_btn,&filamnt_page.step_style);
              lv_btn_set_style_both(filamnt_page.step10_btn,&filamnt_page.step_style);    
            }
            else if(uiCfg.extruStep == EX_MID_SPEED)
            {
              lv_btn_set_style_both(filamnt_page.step1_btn,&filamnt_page.step_style);
              lv_btn_set_style_both(filamnt_page.step5_btn,&filamnt_page.step_style_rel);
              lv_btn_set_style_both(filamnt_page.step10_btn,&filamnt_page.step_style);
            }
            else
            {
              lv_btn_set_style_both(filamnt_page.step1_btn,&filamnt_page.step_style);
              lv_btn_set_style_both(filamnt_page.step5_btn,&filamnt_page.step_style);
              lv_btn_set_style_both(filamnt_page.step10_btn,&filamnt_page.step_style_rel);
            }
          }

      break;
      case ID_FILAMNT_RETURN:
          thermalManager.setTargetHotend(uiCfg.hotendTargetTempBak, uiCfg.extruderIndex);
          feedrate_mm_s = (float)uiCfg.moveSpeed_bak;
          if(fila_data.contimued_open){
              fila_data.contimued_open = false;
            }
        if (uiCfg.print_state == PAUSED)
          planner.set_e_position_mm((destination.e = current_position.e = uiCfg.current_e_position_bak));
        if((uiCfg.print_state == PAUSING)||(uiCfg.print_state == PAUSED))
        {
          set_e_dir_odr();
          if (gCfgItems.from_flash_pic)
            flash_preview_begin = true;
          else
            default_preview_flg = true;
            clear_cur_ui();
            lv_draw_printing();
        }
        else
        {
          if(no_filamet_print_flag)
          {
            feedrate_mm_s = (float)uiCfg.moveSpeed_bak;
             if (READ(MT_DET_1_PIN) != MT_DET_PIN_STATE)
             {
              no_filamet_print_flag = false;
              clear_cur_ui();
              lv_draw_dialog(DIALOG_TYPE_PRINT_FILE);
             }
             else
             {
              clear_cur_ui();
              lv_draw_dialog(DIALOG_NO_FILAMENT_TIPS);
             }
          }
          else
          {
            feedrate_mm_s = (float)uiCfg.moveSpeed_bak;
            clear_cur_ui();
            draw_return_ui();
          }


        }
        break;
    }
  }
}
void lv_draw_filament_change() {
  lv_screen_distribution(FILAMENTCHANGE_UI);

  fila_data.e_dir_odr = ret_e_dir_odr();
  
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(FILAMENTCHANGE_UI),TITLE_LABEL_X);
  // mks_printer.user_src = lv_scr_create(mks_printer.user_src,0,TITLE_H,TITLE_W,TFT_HEIGHT-TITLE_H);
  lv_refr_now(lv_refr_get_disp_refreshing());

  lv_style_copy(&filamnt_page.step_style, &lv_style_scr);
  filamnt_page.step_style.body.main_color = LV_COLOR_MAKE(0xb5,0xb5,0xb5);
  filamnt_page.step_style.body.grad_color = LV_COLOR_MAKE(0xb5,0xb5,0xb5);
  filamnt_page.step_style.body.radius = 5;

  lv_style_copy(&filamnt_page.step_style_rel, &lv_style_scr);
  filamnt_page.step_style_rel.body.main_color = LV_COLOR_MAKE(0x41,0x85,0xf7);
  filamnt_page.step_style_rel.body.grad_color = LV_COLOR_MAKE(0x41,0x85,0xf7);
  filamnt_page.step_style_rel.body.radius = 5;

  lv_style_copy(&filamnt_page.heat_bg_style, &lv_style_scr);
  filamnt_page.heat_bg_style.body.main_color = LV_COLOR_MAKE(0xff,0xa2,0x10);
  filamnt_page.heat_bg_style.body.grad_color = LV_COLOR_MAKE(0xff,0xa2,0x10);
  filamnt_page.heat_bg_style.body.radius = 5;

  lv_style_copy(&filamnt_page.stop_bg_style, &lv_style_scr);
  filamnt_page.stop_bg_style.body.main_color = LV_COLOR_MAKE(0xea,0x43,0x35);
  filamnt_page.stop_bg_style.body.grad_color = LV_COLOR_MAKE(0xea,0x43,0x35);
  filamnt_page.stop_bg_style.body.radius = 5;

  filamnt_page.back_imgbtn = lv_imgbtn_align_create(mks_printer.user_src1, "F:/png_back.bin", LV_ALIGN_IN_LEFT_MID, 0, 0, event_handler, ID_FILAMNT_RETURN);

  filamnt_page.step1_btn = lv_btn_create(mks_printer.user_src,NULL);
  lv_obj_set_size(filamnt_page.step1_btn,72,60);
  lv_obj_set_pos(filamnt_page.step1_btn,10,78);
  lv_obj_set_event_cb_mks(filamnt_page.step1_btn, event_handler, ID_FILAMNT_STEP1, "", 0);
  if((uiCfg.extruStep == EX_LOW_SPEED)&&!fila_data.contimued)
  lv_btn_set_style_both(filamnt_page.step1_btn,&filamnt_page.step_style_rel);
  else
    lv_btn_set_style_both(filamnt_page.step1_btn,&filamnt_page.step_style);

  filamnt_page.step1_label = lv_label_create(filamnt_page.step1_btn,"1mm");
  lv_label_set_style(filamnt_page.step1_label,LV_LABEL_STYLE_MAIN,&User_style.ready_style);

  filamnt_page.step5_btn = lv_btn_create(mks_printer.user_src,filamnt_page.step1_btn);
  lv_obj_set_pos(filamnt_page.step5_btn,85,78);
  lv_obj_set_event_cb_mks(filamnt_page.step5_btn, event_handler, ID_FILAMNT_STEP5, "", 0);
   if((uiCfg.extruStep == EX_MID_SPEED)&&!fila_data.contimued)
  lv_btn_set_style_both(filamnt_page.step5_btn,&filamnt_page.step_style_rel);
  else
    lv_btn_set_style_both(filamnt_page.step5_btn,&filamnt_page.step_style);

  filamnt_page.step5_label = lv_label_create(filamnt_page.step5_btn,"5mm");
  lv_label_set_style(filamnt_page.step5_label,LV_LABEL_STYLE_MAIN,&User_style.ready_style);

  filamnt_page.step10_btn = lv_btn_create(mks_printer.user_src,filamnt_page.step1_btn);
  lv_obj_set_pos(filamnt_page.step10_btn,161,78);
  lv_obj_set_event_cb_mks(filamnt_page.step10_btn, event_handler, ID_FILAMNT_STEP10, "", 0);
  if((uiCfg.extruStep == EX_HIGH_SPEED)&&!fila_data.contimued)
  lv_btn_set_style_both(filamnt_page.step10_btn,&filamnt_page.step_style_rel);
  else
    lv_btn_set_style_both(filamnt_page.step10_btn,&filamnt_page.step_style);

  filamnt_page.step10_label = lv_label_create(filamnt_page.step10_btn,"10mm");
  lv_label_set_style(filamnt_page.step10_label,LV_LABEL_STYLE_MAIN,&User_style.ready_style);

  if(fila_data.contimued)
    filamnt_page.png_loop_off_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_loop_on.bin", 236, 78, event_handler, ID_FILAMNT_png_loop_off);
  else
    filamnt_page.png_loop_off_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_loop_off.bin", 236, 78, event_handler, ID_FILAMNT_png_loop_off);

  filamnt_page.ex_img = lv_img_create(mks_printer.user_src,NULL);
  lv_img_set_src(filamnt_page.ex_img,"F:/png_filament_img.bin");
  lv_obj_set_pos(filamnt_page.ex_img,96,211);


  char buf[20] = {0};
  memset(public_buf_l,0,sizeof(public_buf_l));
  // strcat(public_buf_l, uiCfg.extruderIndex < 1 ? preheat_menu.ext1 : preheat_menu.ext2);
  sprintf(buf, "%d°C/%d°C", thermalManager.wholeDegHotend(uiCfg.extruderIndex), thermalManager.degTargetHotend(uiCfg.extruderIndex));

  // strcat_P(public_buf_l, PSTR(": "));
  strcat(public_buf_l, buf);

  filamnt_page.temp_label = lv_label_create(mks_printer.user_src,public_buf_l);
  // lv_obj_set_pos(filamnt_page.temp_label,67,115);
  lv_obj_align(filamnt_page.temp_label,mks_printer.user_src,LV_ALIGN_CENTER,0,-50);
  lv_label_set_style(filamnt_page.temp_label,LV_LABEL_STYLE_MAIN,&User_style.my_text_black_style);

  



  filamnt_page.add_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_squeeze_in.bin", 5, 214, event_handler, ID_FILAMNT_OUT);
  filamnt_page.dec_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_squeeze.bin", 228, 214, event_handler, ID_FILAMNT_IN);


  filamnt_page.heat_imgbtn = lv_btn_create(mks_printer.user_src,NULL);
  lv_obj_set_size(filamnt_page.heat_imgbtn,116,80);
  lv_obj_set_pos(filamnt_page.heat_imgbtn,15,382);
  lv_obj_set_event_cb_mks(filamnt_page.heat_imgbtn, event_handler, ID_FILAMNT_HEAT, "", 0);
  lv_btn_set_style_both(filamnt_page.heat_imgbtn,&filamnt_page.heat_bg_style);
  filamnt_page.heat_label = lv_label_create(filamnt_page.heat_imgbtn,filament_menu.heating_nozzle);
  lv_label_set_style(filamnt_page.heat_label,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);

  filamnt_page.stop_imgbtn = lv_btn_create(mks_printer.user_src,NULL);
  lv_obj_set_size(filamnt_page.stop_imgbtn,116,80);
  lv_obj_set_pos(filamnt_page.stop_imgbtn,190,382);
  lv_obj_set_event_cb_mks(filamnt_page.stop_imgbtn, event_handler, ID_FILAMNT_STOP, "", 0);
  lv_btn_set_style_both(filamnt_page.stop_imgbtn,&filamnt_page.stop_bg_style);
  filamnt_page.stop_label = lv_label_create(filamnt_page.stop_imgbtn,filament_menu.stop);
  lv_label_set_style(filamnt_page.stop_label,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);

  // filamnt_page.speed_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/filament_speed.bin", 173, 249, event_handler, ID_FILAMNT_STEP);

  // if(fila_data.cur_speed_step == LOW_SPEED)
  // filamnt_page.speed_label = lv_label_create(mks_printer.user_src,"low");
  // else
  // filamnt_page.speed_label = lv_label_create(mks_printer.user_src,"high");
  // lv_label_set_style(filamnt_page.speed_label,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);
  // lv_obj_align(filamnt_page.speed_label,filamnt_page.speed_imgbtn,LV_ALIGN_IN_BOTTOM_MID,0,0);

}

void disp_filament_type() {
  if (uiCfg.extruderIndex == 1) {
    lv_imgbtn_set_src_both(buttonType, "F:/bmp_extru2.bin");
    if (gCfgItems.multiple_language) {
      lv_label_set_text(labelType, preheat_menu.ext2);
      lv_obj_align(labelType, buttonType, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
  }
  else {
    lv_imgbtn_set_src_both(buttonType, "F:/bmp_extru1.bin");
    if (gCfgItems.multiple_language) {
      lv_label_set_text(labelType, preheat_menu.ext1);
      lv_obj_align(labelType, buttonType, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
  }
}

void disp_filament_temp() {
  char buf[20] = {0};
  char buff[50];
  public_buf_l[0] = '\0';

  memset(public_buf_l,0,sizeof(public_buf_l));
  // strcat(public_buf_l, uiCfg.extruderIndex < 1 ? preheat_menu.ext1 : preheat_menu.ext2);
  sprintf(buf, "%d°C/%d°C", thermalManager.wholeDegHotend(uiCfg.extruderIndex), thermalManager.degTargetHotend(uiCfg.extruderIndex));

  // strcat_P(public_buf_l, PSTR(": "));
  strcat(public_buf_l, buf);
  lv_label_set_text(filamnt_page.temp_label, public_buf_l);
  lv_obj_align(filamnt_page.temp_label,mks_printer.user_src,LV_ALIGN_CENTER,0,-50);

  // lv_obj_align(tempText1, nullptr, LV_ALIGN_CENTER, 0, -50);
}

void lv_clear_filament_change() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);
}

#endif // HAS_TFT_LVGL_UI
