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
#include "../../../inc/MarlinConfig.h"



#include "../../../gcode/gcode.h"
#include "../../../module/motion.h"
#include "../../../module/planner.h"

#ifndef USE_NEW_LVGL_CONF
  static lv_obj_t *scr;
#endif

typedef struct{
  lv_obj_t *ex1_temp_label;
  lv_obj_t *ex2_temp_label;
  lv_obj_t *bed_temp_label;

  lv_obj_t *ex1_temp_title_label;
  lv_obj_t *ex2_temp_title_label;
  lv_obj_t *bed_temp_title_label;
  

  lv_obj_t *ext_add_imgbtn;
  lv_obj_t *ext_dec_imgbtn;

  lv_obj_t *bed_add_imgbtn;
  lv_obj_t *bed_dec_imgbtn;

  lv_obj_t *pla_imgbtn;
  lv_obj_t *temp_imgbtn;
  lv_obj_t *back_imgbtn;

  lv_obj_t *ex1_label;
  lv_obj_t *ex2_label;
  lv_obj_t *bed_label;
  lv_obj_t *ex1_img;
  lv_obj_t *ex2_img;
  lv_obj_t *bed_img;
}PREHEAT_UI_PAGE;

PREHEAT_UI_PAGE preheat_page;

extern lv_group_t*  g;
static lv_obj_t *buttonType, *buttonStep, *buttonAdd, *buttonDec;
static lv_obj_t *labelType;
static lv_obj_t *labelStep;
static lv_obj_t *tempText1;
static lv_obj_t *btn_pla;
static lv_obj_t *btn_abs;
static lv_obj_t *label_abs;
static lv_obj_t *label_pla;

static lv_style_t btn_style_pre;
static lv_style_t btn_style_rel;

enum {
  ID_P_ADD = 1,
  ID_P_DEC,
  ID_P_BED_ADD,
  ID_P_BED_DEC,
  ID_P_TYPE,
  ID_P_STEP,
  ID_P_OFF,
  ID_P_RETURN,
  ID_P_ICE,
  ID_P_ABS,
  ID_P_PLA
};

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  int16_t max_target;
  char buff[20];
 if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
      button_sound_end();
      switch (obj->mks_obj_id) {
        case ID_P_BED_ADD:
          #if HAS_HEATED_BED
              // constexpr int16_t max_target = BED_MAXTEMP - (WATCH_BED_TEMP_INCREASE + TEMP_BED_HYSTERESIS + 1);
              max_target = BED_MAXTEMP - (WATCH_BED_TEMP_INCREASE + TEMP_BED_HYSTERESIS + 1);
              // max_target = BED_MAXTEMP - ( TEMP_BED_HYSTERESIS + 1);
              thermalManager.temp_bed.target += uiCfg.stepHeat;
              sprintf_P(buff, PSTR(" %d"), thermalManager.temp_bed.target);
              if(thermalManager.temp_bed.target > 140)
              thermalManager.temp_bed.target = 140;
              // SERIAL_ECHO_MSG("temp",((const char *)buff),"\r\n");
              if (thermalManager.degTargetBed() > max_target)
                thermalManager.setTargetBed(max_target);
              thermalManager.start_watching_bed();
            disp_pre_bed_temp();
            #endif
            break;
        case ID_P_ADD:
          // if (uiCfg.curTempType == 0) {
            thermalManager.temp_hotend[uiCfg.extruderIndex].target += uiCfg.stepHeat;
            if (uiCfg.extruderIndex == 0)
              max_target = HEATER_0_MAXTEMP - (WATCH_TEMP_INCREASE + TEMP_HYSTERESIS + 1);
            else {
              #if HAS_MULTI_HOTEND
                max_target = HEATER_1_MAXTEMP - (WATCH_TEMP_INCREASE + TEMP_HYSTERESIS + 1);
              #endif
            }
            if(max_target > 275)
            max_target = 275;
            if (thermalManager.degTargetHotend(uiCfg.extruderIndex) > max_target)
              thermalManager.setTargetHotend(max_target, uiCfg.extruderIndex);
            thermalManager.start_watching_hotend(uiCfg.extruderIndex);
            disp_desire_temp();
        break;
        case ID_P_BED_DEC:
            #if HAS_HEATED_BED
              if (thermalManager.degTargetBed() > uiCfg.stepHeat)
                thermalManager.temp_bed.target -= uiCfg.stepHeat;
              else
                thermalManager.setTargetBed(0);

              thermalManager.start_watching_bed();
            #endif
            disp_pre_bed_temp();
        break;
        case ID_P_DEC:
          // if (uiCfg.curTempType == 0) {
            if (thermalManager.degTargetHotend(uiCfg.extruderIndex) > uiCfg.stepHeat)
              thermalManager.temp_hotend[uiCfg.extruderIndex].target -= uiCfg.stepHeat;
            else
              thermalManager.setTargetHotend(0, uiCfg.extruderIndex);
            thermalManager.start_watching_hotend(uiCfg.extruderIndex);
          // }
          // else {
          //   #if HAS_HEATED_BED
          //     if (thermalManager.degTargetBed() > uiCfg.stepHeat)
          //       thermalManager.temp_bed.target -= uiCfg.stepHeat;
          //     else
          //       thermalManager.setTargetBed(0);

          //     thermalManager.start_watching_bed();
          //   #endif
          // }
          disp_desire_temp();
          break;
        case ID_P_TYPE:
          if (uiCfg.curTempType == 0) {
            if (ENABLED(HAS_MULTI_EXTRUDER)) {
              if (uiCfg.extruderIndex == 0) {
                uiCfg.extruderIndex = 1;
              }
              else if (uiCfg.extruderIndex == 1) {
                if (ENABLED(HAS_HEATED_BED)) {
                  uiCfg.curTempType = 1;
                }
                else {
                  uiCfg.curTempType = 0;
                  uiCfg.extruderIndex = 0;
                }
              }
            }
            else if (uiCfg.extruderIndex == 0) {
              uiCfg.curTempType = TERN(HAS_HEATED_BED, 1, 0);
            }
          }
          else if (uiCfg.curTempType == 1) {
            uiCfg.extruderIndex = 0;
            uiCfg.curTempType = 0;
          }
          // disp_temp_type();
          break;
        case ID_P_STEP:
          switch (uiCfg.stepHeat) {
            case  1: uiCfg.stepHeat =  5; break;
            case  5: uiCfg.stepHeat = 10; break;
            case 10: uiCfg.stepHeat =  1; break;
            default: break;
          }
          disp_step_heat();
          break;
        case ID_P_OFF:
          if (uiCfg.curTempType == 0) {
            thermalManager.setTargetHotend(0, uiCfg.extruderIndex);
            thermalManager.start_watching_hotend(uiCfg.extruderIndex);
          }
          else {
            #if HAS_HEATED_BED
              thermalManager.temp_bed.target = 0;
              thermalManager.start_watching_bed();
            #endif
          }
          disp_desire_temp();
          break;
        case ID_P_RETURN:
          clear_cur_ui();
          draw_return_ui();
          break;
        case ID_P_ABS:
          // if (uiCfg.curTempType == 0)
            thermalManager.setTargetHotend(temp_seting.abs_ex1, 0);
          // else if (uiCfg.curTempType == 1)
            #if HAS_HEATED_BED
            thermalManager.setTargetBed(temp_seting.abs_bed);
            #endif
          break;
        case ID_P_PLA:
          // if (uiCfg.curTempType == 0)
            thermalManager.setTargetHotend(temp_seting.pla_ex1, 0);
          // else if (uiCfg.curTempType == 1)
            #if HAS_HEATED_BED
            thermalManager.setTargetBed(temp_seting.pla_bed);
            #endif
          break;
        case ID_P_ICE:
          thermalManager.setTargetHotend(0, 0);
          thermalManager.setTargetBed(0);
        break;
    }
  }
}

void disp_add_dec() {
  // Create image buttons
#ifndef USE_NEW_LVGL_CONF
  buttonAdd = lv_big_button_create(scr, "F:/bmp_Add.bin", preheat_menu.add, INTERVAL_V, titleHeight, event_handler, ID_P_ADD);
  buttonDec = lv_big_button_create(scr, "F:/bmp_Dec.bin", preheat_menu.dec, BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_P_DEC);
#else
  buttonAdd = lv_big_button_create(mks_ui.src_main, "F:/bmp_Add.bin", preheat_menu.add, INTERVAL_V, titleHeight, event_handler, ID_P_ADD);
  buttonDec = lv_big_button_create(mks_ui.src_main, "F:/bmp_Dec.bin", preheat_menu.dec, BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_P_DEC);
#endif
}

void lv_draw_preHeat() {
  char buff[30];

  char buf[20] = { 0 };
  lv_screen_distribution(PRE_HEAT_UI);
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src);
  lv_refr_now(lv_refr_get_disp_refreshing());
  preheat_page.back_imgbtn = lv_imgbtn_create(mks_printer.user_src, "F:/png_back.bin", TITLE_BACK_X, TITLE_BACK_Y, event_handler, ID_P_RETURN);


  sprintf(buf," %d°C", thermalManager.wholeDegHotend(uiCfg.extruderIndex));
  preheat_page.ex1_temp_title_label =  lv_label_align_img(mks_printer.user_src1, "F:/png_temp_e1.bin", buf,LV_ALIGN_OUT_RIGHT_MID,100, 16,5, 0,&User_style.my_text_white_style);//User_style.my_text_white_style
  sprintf(buf," %d°C", thermalManager.wholeDegBed());
  preheat_page.bed_temp_title_label =  lv_label_align_img(mks_printer.user_src1, "F:/png_temp_b1.bin", buf,LV_ALIGN_OUT_RIGHT_MID,207, 19,5, 0,&User_style.my_text_white_style);
  
  lv_refr_now(lv_refr_get_disp_refreshing());

  preheat_page.ext_add_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_temp_add.bin", LV_ALIGN_IN_TOP_LEFT, 11, 104, event_handler, ID_P_ADD);
  preheat_page.ext_dec_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_temp_dec.bin", LV_ALIGN_IN_TOP_RIGHT, -11, 104, event_handler, ID_P_DEC);

  preheat_page.bed_add_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_temp_add.bin", LV_ALIGN_IN_TOP_LEFT, 11, 244, event_handler, ID_P_BED_ADD);
  preheat_page.bed_dec_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_temp_dec.bin", LV_ALIGN_IN_TOP_RIGHT, -11, 244, event_handler, ID_P_BED_DEC);

  preheat_page.pla_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_pla.bin", LV_ALIGN_IN_BOTTOM_LEFT, 10, -10, event_handler, ID_P_PLA);
  preheat_page.temp_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_abs.bin", LV_ALIGN_IN_BOTTOM_MID, 0, -10, event_handler, ID_P_ABS);
  preheat_page.back_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_cool.bin", LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10, event_handler, ID_P_ICE);

  sprintf(buf, preheat_menu.value_state, thermalManager.degTargetHotend(uiCfg.extruderIndex));
  preheat_page.ex1_temp_label = lv_label_align_create(mks_printer.user_src,buf, 0, 125,LV_ALIGN_IN_TOP_MID,  &User_style.my_text_black_style);//User_style.my_text_black_style
  sprintf(buf, preheat_menu.value_state, thermalManager.degTargetBed());
  preheat_page.bed_temp_label = lv_label_align_create(mks_printer.user_src,buf, -10, 275,LV_ALIGN_IN_TOP_MID,  &User_style.my_text_black_style);//User_style.my_text_black_style
  lv_obj_align(preheat_page.ex1_temp_label, mks_printer.user_src, LV_ALIGN_IN_TOP_MID, 0, 135);
  lv_obj_align(preheat_page.bed_temp_label, mks_printer.user_src, LV_ALIGN_IN_TOP_MID, 0, 275);
  // preheat_page.ex1_label = lv_label_align_img(mks_printer.user_src, "F:/preheat_ex.bin", "Extrusion1",LV_ALIGN_OUT_RIGHT_MID,70, 100+30,5, 5,&User_style.small_text_black_style);
  // preheat_page.bed_label = lv_label_align_img(mks_printer.user_src, "F:/preheat_bed.bin", "HotBed",LV_ALIGN_OUT_RIGHT_MID,50, 190+30,5, 0,&User_style.small_text_black_style);


  lv_refr_now(lv_refr_get_disp_refreshing());
  preheat_page.ex1_img = lv_img_create(mks_printer.user_src,NULL);
  lv_img_set_src(preheat_page.ex1_img,"F:/png_temp_e2.bin");
  lv_obj_set_pos(preheat_page.ex1_img, 95, 180);
  preheat_page.ex1_label  = lv_label_create_empty(mks_printer.user_src);
  lv_label_set_text(preheat_page.ex1_label,preheat_menu.ext1);
  if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  {
    lv_obj_align(preheat_page.ex1_label, preheat_page.ex1_img, LV_ALIGN_OUT_RIGHT_MID, 10, 5);
    lv_label_set_style(preheat_page.ex1_label,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
  }
  else{
    lv_obj_align(preheat_page.ex1_label, preheat_page.ex1_img, LV_ALIGN_OUT_RIGHT_MID, 5, 5);
    lv_label_set_style(preheat_page.ex1_label,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
  }

  
  
  preheat_page.bed_img = lv_img_create(mks_printer.user_src,NULL);
  lv_img_set_src(preheat_page.bed_img,"F:/png_temp_b2.bin");
  lv_obj_set_pos(preheat_page.bed_img, 95, 320);
  preheat_page.bed_label  = lv_label_create_empty(mks_printer.user_src);
  lv_label_set_text(preheat_page.bed_label,preheat_menu.hotbed);
  if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  {
    lv_obj_align(preheat_page.bed_label, preheat_page.bed_img, LV_ALIGN_OUT_RIGHT_MID, 5, 5);
    lv_label_set_style(preheat_page.bed_label,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
  }
  else
  {
    lv_obj_align(preheat_page.bed_label, preheat_page.bed_img, LV_ALIGN_OUT_RIGHT_MID, 5, 5);
    lv_label_set_style(preheat_page.bed_label,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
  }
}

void disp_ext_heart() {
#ifndef USE_NEW_LVGL_CONF
  btn_abs = lv_btn_create(scr, 160, 40, 80, 40, event_handler, ID_P_ABS);
  btn_pla = lv_btn_create(scr, 260, 40, 80, 40, event_handler, ID_P_PLA);
#else
  btn_abs = lv_btn_create(mks_ui.src_main, 160, 40, 80, 40, event_handler, ID_P_ABS);
  btn_pla = lv_btn_create(mks_ui.src_main, 260, 40, 80, 40, event_handler, ID_P_PLA);
#endif

  lv_btn_set_style(btn_abs, LV_BTN_STYLE_PR, &btn_style_pre);
  lv_btn_set_style(btn_abs, LV_BTN_STYLE_REL, &btn_style_rel);
  lv_btn_set_style(btn_pla, LV_BTN_STYLE_PR, &btn_style_pre);
  lv_btn_set_style(btn_pla, LV_BTN_STYLE_REL, &btn_style_rel);

  label_abs = lv_label_create(btn_abs, PREHEAT_2_LABEL);
  label_pla = lv_label_create(btn_pla, PREHEAT_1_LABEL);
}

void disp_temp_type() {

    char buf[20] = { 0 };
    sprintf(buf," %d°C", thermalManager.wholeDegHotend(uiCfg.extruderIndex));
    lv_label_set_text(preheat_page.ex1_temp_title_label, buf);
  #if HAS_HEATED_BED
    memset(buf,0,sizeof(buf));
    sprintf(buf," %d°C", thermalManager.wholeDegBed());
    lv_label_set_text(preheat_page.bed_temp_title_label, buf);
  #endif
}

void disp_desire_temp() {
  char buf[20] = { 0 };
  public_buf_l[0] = '\0';

  sprintf(buf, preheat_menu.value_state,  thermalManager.degTargetHotend(uiCfg.extruderIndex));
  strcat(public_buf_l, buf);
  lv_label_set_text(preheat_page.ex1_temp_label, public_buf_l);
  lv_obj_align(preheat_page.ex1_temp_label, mks_printer.user_src, LV_ALIGN_IN_TOP_MID, 0, 135);
}
void disp_pre_bed_temp()
{
 char buf[20] = { 0 };
  public_buf_l[0] = '\0';


  #if HAS_HEATED_BED
    sprintf(buf, preheat_menu.value_state,  thermalManager.degTargetBed());
  #endif

  strcat(public_buf_l, buf);
  lv_label_set_text(preheat_page.bed_temp_label, public_buf_l);
  lv_obj_align(preheat_page.bed_temp_label, mks_printer.user_src, LV_ALIGN_IN_TOP_MID, 0, 275);

}

void disp_step_heat() {
  if (uiCfg.stepHeat == 1) {
    lv_imgbtn_set_src_both(buttonStep, "F:/bmp_step1_degree.bin");
  }
  else if (uiCfg.stepHeat == 5) {
    lv_imgbtn_set_src_both(buttonStep, "F:/bmp_step5_degree.bin");
  }
  else if (uiCfg.stepHeat == 10) {
    lv_imgbtn_set_src_both(buttonStep, "F:/bmp_step10_degree.bin");
  }

  if (gCfgItems.multiple_language) {
    if (uiCfg.stepHeat == 1) {
      lv_label_set_text(labelStep, preheat_menu.step_1c);
      lv_obj_align(labelStep, buttonStep, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
    else if (uiCfg.stepHeat == 5) {
      lv_label_set_text(labelStep, preheat_menu.step_5c);
      lv_obj_align(labelStep, buttonStep, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
    else if (uiCfg.stepHeat == 10) {
      lv_label_set_text(labelStep, preheat_menu.step_10c);
      lv_obj_align(labelStep, buttonStep, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }
  }
}

void lv_clear_preHeat() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
#ifndef USE_NEW_LVGL_CONF
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);
#else
  lv_obj_clean(mks_ui.src_main);
#endif
}

#endif // HAS_TFT_LVGL_UI
