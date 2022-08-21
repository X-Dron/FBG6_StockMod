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

/**
 * draw_dialog.cpp
 */

#include "../../../inc/MarlinConfigPre.h"

#if HAS_TFT_LVGL_UI

#include "draw_ui.h"
#include <lv_conf.h>

#include "../../../sd/cardreader.h"
#include "../../../gcode/queue.h"
#include "../../../module/temperature.h"
#include "../../../module/planner.h"
#include "../../../gcode/gcode.h"
#include "../../../inc/MarlinConfig.h"
#include "../../../../src/module/stepper.h"
#include "../../../../src/module/endstops.h"

#if ENABLED(EEPROM_SETTINGS)
  #include "../../../module/settings.h"
#endif

#if ENABLED(POWER_LOSS_RECOVERY)
  #include "../../../feature/powerloss.h"
#endif

#if ENABLED(PARK_HEAD_ON_PAUSE)
  #include "../../../feature/pause.h"
#endif

#if ENABLED(TOUCH_SCREEN_CALIBRATION)
  #include "../../tft_io/touch_calibration.h"
  #include "draw_touch_calibration.h"
#endif

#if ENABLED(AUTO_BED_LEVELING_BILINEAR)
  // #include "../../../../feature/bedlevel/bedlevel.h"
  #include "../../../../src/feature/bedlevel/bedlevel.h"

#endif

#if ENABLED(AUTO_BED_LEVELING_BILINEAR)
  extern bed_mesh_t z_values;
#endif
extern bool g_emergency_endstop;

extern uint8_t layer_stop_flag;
extern bool g_emergency_stop_flag;
extern bool g_emergency_endstop_flag;
extern lv_group_t *g;
static lv_obj_t *dialog_img, *tempText1, *filament_bar;
// static lv_obj_t *dialog_img, *tempText1, *filament_bar;
extern uint8_t sel_id;
extern bool once_flag, gcode_preview_over;
extern int upload_result;
extern uint32_t upload_time_sec;
extern uint32_t upload_size;
extern bool temps_update_flag;
float z_offset_add = 0;
extern bool flash_preview_begin, default_preview_flg, gcode_preview_over;
extern bool open_fan, more_open_fan;
extern bool no_filamet_print_flag;
//#define CANCEL_ON_RIGHT   // Put 'Cancel' on the right (as it was before)

#define BTN_OK_X      TERN(CANCEL_ON_RIGHT, 100, 280)
#define BTN_CANCEL_X  TERN(CANCEL_ON_RIGHT, 280, 100)
#define BTN_OK_Y      180
#define BTN_CANCEL_Y  180

#define BTN_DIALOG_W 217
#define BTN_DIALOG_H 76

#define BTN_DIALOG_X 51
#define BTN_DIALOG_Y1 265
#define BTN_DIALOG_Y2 355
typedef struct
{
  lv_style_t style_btn; //橙
  lv_style_t style_btn1;//粉
  lv_style_t style_btn2;//蓝
  lv_style_t style_btn3;//
  lv_style_t style_btn4; //绿
}DIALOG_UI_PAGE;
extern int32_t save_layer_stop_num ;
extern int32_t save_disp_layer_stop_num;
DIALOG_UI_PAGE dialog_page;

static void btn_back_event_cb(lv_obj_t *btn, lv_event_t event) {
   if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
      button_sound_end();
      wifi_connect_data.start = false;
      clear_cur_ui();
      draw_return_ui();
  }
}
static void btn_ok_event_cb(lv_obj_t *btn, lv_event_t event) {
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
      button_sound_end();

  if (DIALOG_IS(TYPE_PRINT_FILE)) {
    #if HAS_GCODE_PREVIEW
      preview_gcode_prehandle(list_file.file_name[sel_id]);
    #endif
    reset_print_time();
    start_print_time();

    uiCfg.print_state = WORKING;
    layer_stop_flag = LAYER_STOP_NO_TRIGGERED;
    if(Layout_stop_num.already_obtained_flag)
      Layout_stop_num.already_obtained_flag = false;

    if(Layout_stop_num.bottom_already_obtained_flag)
      Layout_stop_num.bottom_already_obtained_flag = false;
    baby_step_set_data = true;
    lv_clear_dialog();
    open_fan = false;
    more_open_fan = true;
    lv_draw_printing();
    save_disp_layer_stop_num = 0;
    save_layer_stop_num = 0;
      // z_offset_add = 0;
      // gCfgItems.babystep_data = z_offset_add;
      //   update_spi_flash();
    #if ENABLED(SDSUPPORT)
      if (!gcode_preview_over) {
        char *cur_name;
        cur_name = strrchr(list_file.file_name[sel_id], '/');

        SdFile file, *curDir;
        card.abortFilePrintNow();
        const char * const fname = card.diveToFile(false, curDir, cur_name);
        if (!fname) return;
        if (file.open(curDir, fname, O_READ)) {
          gCfgItems.curFilesize = file.fileSize();
          file.close();
          update_spi_flash();
        }
        card.openFileRead(cur_name);
        if (card.isFileOpen()) {
          feedrate_percentage = 100;
          planner.flow_percentage[0] = 100;
          planner.e_factor[0] = planner.flow_percentage[0] * 0.01f;
          #if HAS_MULTI_EXTRUDER
            planner.flow_percentage[1] = 100;
            planner.e_factor[1] = planner.flow_percentage[1] * 0.01f;
          #endif
          card.startOrResumeFilePrinting();
          TERN_(POWER_LOSS_RECOVERY, recovery.prepare());
          once_flag = false;
        }
      }
    #endif
    // send_m290();
  }
  else if (DIALOG_IS(TYPE_STOP)) {
    if(Layout_stop_num.already_obtained_flag)
      Layout_stop_num.already_obtained_flag = false;
    if(Layout_stop_num.bottom_already_obtained_flag)
      Layout_stop_num.bottom_already_obtained_flag = false;
    wait_for_heatup = false;
    baby_step_set_data = false;

    stop_print_time();
    lv_clear_dialog();
    lv_draw_ready_print();
    recoveryed_flag = false;

    #if ENABLED(SDSUPPORT)
      uiCfg.print_state = IDLE;
      card.abortFilePrintSoon();
    #endif

    #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
      if (uiCfg.adjustZoffset) {
        #if DISABLED(Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN)
          for (uint8_t x = 0; x < GRID_MAX_POINTS_X; x++)
            for (uint8_t y = 0; y < GRID_MAX_POINTS_Y; y++)
              z_values[x][y] = z_values[x][y] + uiCfg.babyStepZoffsetDiff;
        #endif
        TERN_(EEPROM_SETTINGS, (void)settings.save());
        uiCfg.babyStepZoffsetDiff = 0;
        uiCfg.adjustZoffset       = 0;
      }
    #endif
  }
  else if (DIALOG_IS(TYPE_FINISH_PRINT)) {
    if(Layout_stop_num.already_obtained_flag)
    Layout_stop_num.already_obtained_flag = false;
    if(Layout_stop_num.bottom_already_obtained_flag)
      Layout_stop_num.bottom_already_obtained_flag = false;
    recoveryed_flag = false;
    baby_step_set_data = false;
    clear_cur_ui();
    lv_draw_ready_print();
    #if ENABLED(AUTO_BED_LEVELING_BILINEAR)
      if (uiCfg.adjustZoffset) {
        #if DISABLED(Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN)
          for (uint8_t x = 0; x < GRID_MAX_POINTS_X; x++)
            for (uint8_t y = 0; y < GRID_MAX_POINTS_Y; y++)
              z_values[x][y] = z_values[x][y] + uiCfg.babyStepZoffsetDiff;
        #endif
        TERN_(EEPROM_SETTINGS, (void)settings.save());
        uiCfg.babyStepZoffsetDiff = 0;
        uiCfg.adjustZoffset       = 0;
      }
    #endif
  }
  #if ENABLED(ADVANCED_PAUSE_FEATURE)
    else if (DIALOG_IS(PAUSE_MESSAGE_WAITING, PAUSE_MESSAGE_INSERT, PAUSE_MESSAGE_HEAT))
      wait_for_user = false;
    else if (DIALOG_IS(PAUSE_MESSAGE_OPTION))
      pause_menu_response = PAUSE_RESPONSE_EXTRUDE_MORE;
    else if (DIALOG_IS(PAUSE_MESSAGE_RESUME)) {
      clear_cur_ui();
      draw_return_ui();
    }
  #endif
  else if (DIALOG_IS(STORE_EEPROM_TIPS)) {
    TERN_(EEPROM_SETTINGS, (void)settings.save());
    clear_cur_ui();
    draw_return_ui();
  }
  else if(DIALOG_IS(TYPE_RECOVERY_TIPS))    {
      baby_step_set_data = true;
    if (gCfgItems.from_flash_pic)
        flash_preview_begin = true;
      else
        default_preview_flg = true;

      uiCfg.print_state = REPRINTING;
      lv_clear_dialog();  
      layer_stop_flag = LAYER_STOP_NO_TRIGGERED;
      open_fan = false;
      more_open_fan = true;
      lv_draw_printing();
    }
  else if (DIALOG_IS(READ_EEPROM_TIPS)) {
    TERN_(EEPROM_SETTINGS, (void)settings.load());
    clear_cur_ui();
    draw_return_ui();
  }
  else if (DIALOG_IS(REVERT_EEPROM_TIPS)) {
    TERN_(EEPROM_SETTINGS, (void)settings.reset());
    clear_cur_ui();
    #if ENABLED(TOUCH_SCREEN_CALIBRATION)
      const bool do_draw_cal = touch_calibration.need_calibration();
      if (do_draw_cal) {
        disp_state_stack._disp_index--; // We are asynchronous from the dialog, so let's remove the dialog from the stack
        lv_draw_touch_calibration_screen();
      }
    #else
      constexpr bool do_draw_cal = false;
    #endif
    if (!do_draw_cal) draw_return_ui();
  }
  else if (DIALOG_IS(WIFI_CONFIG_TIPS)) {
    uiCfg.configWifi = true;
    clear_cur_ui();
    draw_return_ui();
  }
  else if(DIALOG_IS(EMERGENCY_TIPS))
  {
    // sei();
    // stepper.init();
    // _SET_OUTPUT(X_STEP_PIN);
    // _SET_OUTPUT(Y_STEP_PIN);
    // _SET_OUTPUT(Z_STEP_PIN);
    // _SET_OUTPUT(E0_STEP_PIN);
    // _SET_OUTPUT(E1_STEP_PIN);
    // endstops.enable(true); 
    g_emergency_endstop = false;
    g_emergency_endstop_flag = false;
    g_emergency_stop_flag = false;
    clear_cur_ui();
    draw_return_ui();
  }
  else if(DIALOG_IS(NO_FILAMENT_TIPS))
  {
    clear_cur_ui();

    lv_draw_filament_change();
    // start_print_time();
    // if (gCfgItems.from_flash_pic)
    //     flash_preview_begin = true;
    //   else
    //     default_preview_flg = true;

    // uiCfg.print_state = RESUMING;
    // lv_draw_printing();


    // if (gCfgItems.from_flash_pic)
    //   flash_preview_begin = true;
    // else
    //   default_preview_flg = true;
  }
  else if (DIALOG_IS(TYPE_FILAMENT_HEAT_LOAD_COMPLETED))
    uiCfg.filament_heat_completed_load = true;
  else if (DIALOG_IS(TYPE_FILAMENT_HEAT_UNLOAD_COMPLETED))
    uiCfg.filament_heat_completed_unload = true;
  else if (DIALOG_IS(TYPE_FILAMENT_LOAD_COMPLETED, TYPE_FILAMENT_UNLOAD_COMPLETED)) {
    clear_cur_ui();
    draw_return_ui();
  }
  #if ENABLED(MKS_WIFI_MODULE)
    else if (DIALOG_IS(TYPE_UNBIND)) {
      cloud_unbind();
      clear_cur_ui();
      draw_return_ui();
    }
  #endif
  else {
    clear_cur_ui();
    draw_return_ui();
  }
  }
}

static void btn_cancel_event_cb(lv_obj_t *btn, lv_event_t event) {
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
          button_sound_end();
  if (DIALOG_IS(PAUSE_MESSAGE_OPTION)) {
    TERN_(ADVANCED_PAUSE_FEATURE, pause_menu_response = PAUSE_RESPONSE_RESUME_PRINT);
  }
  else if (DIALOG_IS(TYPE_FILAMENT_LOAD_HEAT, TYPE_FILAMENT_UNLOAD_HEAT, TYPE_FILAMENT_HEAT_LOAD_COMPLETED, TYPE_FILAMENT_HEAT_UNLOAD_COMPLETED)) {
    thermalManager.setTargetHotend(uiCfg.hotendTargetTempBak, uiCfg.extruderIndex);
    clear_cur_ui();
    draw_return_ui();
  }
  
  else if (DIALOG_IS(TYPE_FILAMENT_LOADING, TYPE_FILAMENT_UNLOADING)) {
    queue.enqueue_one_P(PSTR("M410"));
    uiCfg.filament_rate                = 0;
    uiCfg.filament_loading_completed   = false;
    uiCfg.filament_unloading_completed = false;
    uiCfg.filament_loading_time_flg    = false;
    uiCfg.filament_loading_time_cnt    = 0;
    uiCfg.filament_unloading_time_flg  = false;
    uiCfg.filament_unloading_time_cnt  = 0;
    thermalManager.setTargetHotend(uiCfg.hotendTargetTempBak, uiCfg.extruderIndex);
    clear_cur_ui();
    draw_return_ui();
  }
  else if(DIALOG_IS(NO_FILAMENT_TIPS))
  {
    if(uiCfg.print_state == IDLE)
    {
      clear_cur_ui();
      lv_draw_ready_print();
    }
    else
    {
      clear_cur_ui();
      if (gCfgItems.from_flash_pic)
          flash_preview_begin = true;
        else
          default_preview_flg = true;
      lv_draw_printing();
    }


    // wait_for_heatup = false;
    // stop_print_time();
    // lv_clear_dialog();
    // recoveryed_flag = false;

    // #if ENABLED(SDSUPPORT)
    // uiCfg.print_state = IDLE;
    // card.abortFilePrintSoon();
    // #endif
    // lv_draw_ready_print();
  }
  else if (DIALOG_IS(TYPE_FINISH_PRINT)) {
        #if HAS_GCODE_PREVIEW
        // char buff[20];
        // sprintf(buff,"n %s",list_file.long_name[sel_id]);
        // SERIAL_ECHO_MSG(buff,"\r\n");
        if(recoveryed_flag)
        preview_gcode_prehandle(list_file.long_name[sel_id]);
        else
        preview_gcode_prehandle(list_file.file_name[sel_id]);
      #endif
      // memset(public_buf_name,0,sizeof(public_buf_name));
      // cutFileName((char *)list_file.long_name[sel_id], 16, 16, (char *)public_buf_name); //???????
      reset_print_time();
      start_print_time();
      baby_step_set_data = true;

      uiCfg.print_state = WORKING;
      lv_clear_dialog();
      // z_offset_add = 0;
      // gCfgItems.babystep_data = z_offset_add;
      //   update_spi_flash();
      layer_stop_flag = LAYER_STOP_NO_TRIGGERED;
      if(Layout_stop_num.already_obtained_flag)
        Layout_stop_num.already_obtained_flag = false;
    if(Layout_stop_num.bottom_already_obtained_flag)
      Layout_stop_num.bottom_already_obtained_flag = false;
      
      save_disp_layer_stop_num = 0;
      save_layer_stop_num = 0;
      open_fan = false;
      more_open_fan = true;
      lv_draw_printing();
      #if ENABLED(SDSUPPORT)
        if (!gcode_preview_over) {
          char *cur_name;
          if(recoveryed_flag)
          cur_name = strrchr(list_file.long_name[sel_id], '/');
          else
          cur_name = strrchr(list_file.file_name[sel_id], '/');
          // SERIAL_ECHO_MSG("pre  ",(const char *)cur_name);
          SdFile file, *curDir;
          card.abortFilePrintNow();
          const char * const fname = card.diveToFile(false, curDir, cur_name);
          
          if (!fname) return;
          if (file.open(curDir, fname, O_READ)) {
            gCfgItems.curFilesize = file.fileSize();
            file.close();
            update_spi_flash();
          }
          // SERIAL_ECHO_MSG("dir  ",(const char *)curDir);
          card.openFileRead(cur_name);
          if (card.isFileOpen()) {
            feedrate_percentage = 100;
            planner.flow_percentage[0] = 100;
            planner.e_factor[0] = planner.flow_percentage[0] * 0.01f;
            #if HAS_MULTI_EXTRUDER
              planner.flow_percentage[1] = 100;
              planner.e_factor[1] = planner.flow_percentage[1] * 0.01f;
            #endif
            card.startOrResumeFilePrinting();
            TERN_(POWER_LOSS_RECOVERY, recovery.prepare());
            once_flag = false;
          }
        }
      #endif
      // clear_cur_ui();
      // lv_draw_ready_print();
    }
  else if(DIALOG_IS(TYPE_PRINT_FILE))
  {
    clear_cur_ui();
    lv_draw_print_file();
  }
  else if(DIALOG_IS(TYPE_RECOVERY_TIPS))    {
      wait_for_heatup = false;
      stop_print_time();
      lv_clear_dialog();   
      lv_draw_ready_print();
      recoveryed_flag = false;
      #if ENABLED(SDSUPPORT)
        uiCfg.print_state = IDLE;
        card.abortFilePrintSoon();
      #endif
    }
  else {
    clear_cur_ui();
    draw_return_ui();
  }
  }
}
void draw_rec_dialog()
{
  lv_obj_t *btnOk = nullptr, *btnCancel = nullptr;
  lv_obj_t *labelOk;
  lv_obj_t *labelCancel;
    lv_style_copy(&dialog_page.style_btn, &lv_style_scr);
  dialog_page.style_btn.body.main_color = LV_COLOR_MAKE(0xfe,0xa2,0x0f);
  dialog_page.style_btn.body.grad_color = LV_COLOR_MAKE(0xfe,0xa2,0x0f);
  dialog_page.style_btn.body.radius = 5;

  lv_style_copy(&dialog_page.style_btn1, &lv_style_scr);
  dialog_page.style_btn1.body.main_color = LV_COLOR_MAKE(0x18,0xa2,0x63);
  dialog_page.style_btn1.body.grad_color = LV_COLOR_MAKE(0x18,0xa2,0x63);
  dialog_page.style_btn1.body.radius = 5;

  lv_style_copy(&dialog_page.style_btn1, &lv_style_scr);
  dialog_page.style_btn1.body.main_color = LV_COLOR_MAKE(0x43,0x86,0xf7);
  dialog_page.style_btn1.body.grad_color = LV_COLOR_MAKE(0x43,0x86,0xf7);
  dialog_page.style_btn1.body.radius = 5;

   btnOk = lv_btn_create(mks_printer.user_src, btn_ok_event_cb, 0, &dialog_page.style_btn);
   btnCancel = lv_btn_create(mks_printer.user_src, btn_cancel_event_cb, 0, &dialog_page.style_btn1);
      // }
   lv_obj_set_size(btnOk, BTN_DIALOG_W, BTN_DIALOG_H);
  lv_obj_align(btnOk, mks_printer.user_src, LV_ALIGN_IN_TOP_LEFT, BTN_DIALOG_X, BTN_DIALOG_Y1);

   labelOk = lv_label_create_empty(btnOk);
   lv_label_set_style(labelOk,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);


    lv_obj_set_size(btnCancel, BTN_DIALOG_W, BTN_DIALOG_H);
    lv_obj_align(btnCancel, mks_printer.user_src, LV_ALIGN_IN_TOP_LEFT, BTN_DIALOG_X, BTN_DIALOG_Y2);

    labelCancel = lv_label_create_empty(btnCancel);
    lv_label_set_style(labelCancel,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);
    lv_label_set_text(labelOk, dialog_menu.recovery_continue_btn);       // Set the labels text
    lv_label_set_text(labelCancel, dialog_menu.recovery_back_btn);
}
  // lv_obj_t *btnOk = nullptr, *btnCancel = nullptr;
lv_obj_t *labelDialog;

void wifi_outtime_label()
{
    lv_label_set_text(labelDialog, wifi_menu.link_overtime_text);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
}
void lv_draw_dialog(uint8_t type) {
  lv_obj_t *btnOk = nullptr, *btnCancel = nullptr;
  uiCfg.dialogType = type;
  // mks_printer.user_src = lv_screen_create(DIALOG_UI);
  lv_screen_distribution(DIALOG_UI);
  
  if(DIALOG_IS(WIFI_ENABLE_TIPS))
  {
    mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(WIFI_UI),TITLE_LABEL_X);
    lv_imgbtn_align_create(mks_printer.user_src1, "F:/png_back.bin", LV_ALIGN_IN_LEFT_MID, 0, 0, btn_back_event_cb,1);
  }

  labelDialog = lv_label_create(mks_printer.user_src, "");
  lv_style_copy(&dialog_page.style_btn, &lv_style_scr);
  dialog_page.style_btn.body.main_color = LV_COLOR_MAKE(0xfe,0xa2,0x0f);
  dialog_page.style_btn.body.grad_color = LV_COLOR_MAKE(0xfe,0xa2,0x0f);
  dialog_page.style_btn.body.radius = 5;

  lv_style_copy(&dialog_page.style_btn1, &lv_style_scr);
  dialog_page.style_btn1.body.main_color = LV_COLOR_MAKE(0x18,0xa2,0x63);
  dialog_page.style_btn1.body.grad_color = LV_COLOR_MAKE(0x18,0xa2,0x63);
  dialog_page.style_btn1.body.radius = 5;

  lv_style_copy(&dialog_page.style_btn2, &lv_style_scr);
  dialog_page.style_btn2.body.main_color = LV_COLOR_MAKE(0x43,0x86,0xf7);
  dialog_page.style_btn2.body.grad_color = LV_COLOR_MAKE(0x43,0x86,0xf7);
  dialog_page.style_btn2.body.radius = 5;

  lv_style_copy(&dialog_page.style_btn3, &lv_style_scr);
  dialog_page.style_btn3.body.main_color = LV_COLOR_MAKE(0xff,0xa2,0x10);
  dialog_page.style_btn3.body.grad_color = LV_COLOR_MAKE(0xff,0xa2,0x10);
  dialog_page.style_btn3.body.radius = 5;

  lv_style_copy(&dialog_page.style_btn4, &lv_style_scr);
  dialog_page.style_btn4.body.main_color = LV_COLOR_MAKE(0x18,0xa2,0x63);
  dialog_page.style_btn4.body.grad_color = LV_COLOR_MAKE(0x18,0xa2,0x63);
  dialog_page.style_btn4.body.radius = 5;

  lv_label_set_style(labelDialog,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
  // if (DIALOG_IS(TYPE_FINISH_PRINT, PAUSE_MESSAGE_RESUME)||DIALOG_IS(WIFI_ENABLE_TIPS)) {
  //   // // btnOk = lv_button_btn_create(mks_printer.user_src, BTN_OK_X + 90, BTN_OK_Y, 100, 50, btn_ok_event_cb);
  //   // lv_obj_t *labelOk = lv_label_align_create_n(mks_printer.user_src,"",LV_ALIGN_CENTER, 172, 46,LV_ALIGN_CENTER,0,60,btn_ok_event_cb,0,&User_style.btn1_bg_style);
  //   // // lv_obj_t *labelOk = lv_label_create_empty(btnOk);           // Add a label to the button
  //   // lv_label_set_text(labelOk, print_file_dialog_menu.confirm); // Set the labels text
  // }
  if(DIALOG_IS(NO_FILAMENT_TIPS))
  {
    dialog_img = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_err_tips.bin", LV_ALIGN_IN_TOP_LEFT, 126, 69, nullptr, 0);
    lv_refr_now(lv_refr_get_disp_refreshing());
    lv_imgbtn_set_src_both(dialog_img,"F:/png_err_tips.bin");

  }
  else if(DIALOG_IS(TYPE_RECOVERY_TIPS)){
    dialog_img = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_pro_tips.bin", LV_ALIGN_IN_TOP_LEFT, 126, 69, nullptr, 0);
    // lv_refr_now(lv_refr_get_disp_refreshing());
    // lv_imgbtn_set_src_both(dialog_img,"F:/png_pro_tips.bin");
  }else
  {

  }

  if (DIALOG_IS(PAUSE_MESSAGE_WAITING, PAUSE_MESSAGE_INSERT, PAUSE_MESSAGE_HEAT)) {
    btnOk = lv_button_btn_create(mks_printer.user_src, BTN_DIALOG_X, BTN_DIALOG_Y1, BTN_DIALOG_W, BTN_DIALOG_H, btn_ok_event_cb);
    lv_obj_t *labelOk = lv_label_create_empty(btnOk);           // Add a label to the button
    lv_label_set_text(labelOk, print_file_dialog_menu.confirm); // Set the labels text
  }
  else if(DIALOG_IS(EMERGENCY_TIPS))
  {
    lv_obj_t *labelOk;
    btnOk = lv_btn_create(mks_printer.user_src, btn_ok_event_cb, 0, &dialog_page.style_btn);
    lv_obj_set_size(btnOk, BTN_DIALOG_W, BTN_DIALOG_H);
    lv_obj_align(btnOk, mks_printer.user_src, LV_ALIGN_IN_TOP_LEFT, BTN_DIALOG_X, BTN_DIALOG_Y1);

    labelOk = lv_label_create_empty(btnOk);
    lv_label_set_text(labelOk, print_file_dialog_menu.confirm); // Set the labels text
    lv_label_set_style(labelOk,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_PARKING, PAUSE_MESSAGE_CHANGING, PAUSE_MESSAGE_UNLOAD, PAUSE_MESSAGE_LOAD, PAUSE_MESSAGE_PURGE, PAUSE_MESSAGE_RESUME, PAUSE_MESSAGE_HEATING)) {
    // nothing to do
  }
  else if (DIALOG_IS(WIFI_ENABLE_TIPS)) {
    // btnCancel = lv_button_btn_create(mks_printer.user_src, BTN_OK_X + 90, BTN_OK_Y, 100, 50, btn_cancel_event_cb);
    // lv_obj_t *labelCancel = lv_label_create_empty(btnCancel);
    // lv_label_set_text(labelCancel, print_file_dialog_menu.cancel);
  }
  else if (DIALOG_IS(TRANSFER_NO_DEVICE)) {
    btnCancel = lv_button_btn_create(mks_printer.user_src, BTN_DIALOG_X, BTN_DIALOG_Y1, BTN_DIALOG_W, BTN_DIALOG_H, btn_cancel_event_cb);
    lv_obj_t *labelCancel = lv_label_create_empty(btnCancel);
    lv_label_set_text(labelCancel, print_file_dialog_menu.cancel);
  }
  #if ENABLED(MKS_WIFI_MODULE)
    else if (DIALOG_IS(TYPE_UPLOAD_FILE)) {
      if (upload_result == 2) {
        btnCancel = lv_button_btn_create(mks_printer.user_src, BTN_DIALOG_X, BTN_DIALOG_Y1, BTN_DIALOG_W, BTN_DIALOG_H, btn_cancel_event_cb);
        lv_obj_t *labelCancel = lv_label_create_empty(btnCancel);
        lv_label_set_text(labelCancel, print_file_dialog_menu.cancel);
      }
      else if (upload_result == 3) {
        btnOk = lv_button_btn_create(mks_printer.user_src, BTN_DIALOG_X, BTN_DIALOG_Y1, BTN_DIALOG_W, BTN_DIALOG_H, btn_ok_event_cb);
        lv_obj_t *labelOk = lv_label_create_empty(btnOk);
        lv_label_set_text(labelOk, print_file_dialog_menu.confirm);
      }
    }
    else if (DIALOG_IS(TYPE_UPDATE_ESP_FIRMWARE)) {
      // nothing to do
    }
  #endif
  else if (DIALOG_IS(TYPE_FILAMENT_LOAD_HEAT, TYPE_FILAMENT_UNLOAD_HEAT)) {
    btnCancel = lv_button_btn_create(mks_printer.user_src, BTN_OK_X + 90, BTN_OK_Y, 100, 50, btn_cancel_event_cb);
    lv_obj_t *labelCancel = lv_label_create_empty(btnCancel);
    lv_label_set_text(labelCancel, print_file_dialog_menu.cancel);

    tempText1 = lv_label_create_empty(mks_printer.user_src);
    filament_sprayer_temp();
  }
  else if (DIALOG_IS(TYPE_FILAMENT_LOAD_COMPLETED, TYPE_FILAMENT_UNLOAD_COMPLETED)) {
    btnOk = lv_button_btn_create(mks_printer.user_src, BTN_OK_X + 90, BTN_OK_Y, 100, 50, btn_ok_event_cb);
    lv_obj_t *labelOk = lv_label_create_empty(btnOk);
    lv_label_set_text(labelOk, print_file_dialog_menu.confirm);
  }
  else if (DIALOG_IS(TYPE_FILAMENT_LOADING, TYPE_FILAMENT_UNLOADING)) {
    btnCancel = lv_button_btn_create(mks_printer.user_src, BTN_OK_X + 90, BTN_OK_Y, 100, 50, btn_cancel_event_cb);
    lv_obj_t *labelCancel = lv_label_create_empty(btnCancel);
    lv_label_set_text(labelCancel, print_file_dialog_menu.cancel);

    filament_bar = lv_bar_create(mks_printer.user_src, nullptr);
    lv_obj_set_pos(filament_bar, (TFT_WIDTH - 400) / 2, ((TFT_HEIGHT - titleHeight) - 40) / 2);
    lv_obj_set_size(filament_bar, 400, 25);
    lv_bar_set_style(filament_bar, LV_BAR_STYLE_INDIC, &lv_bar_style_indic);
    lv_bar_set_anim_time(filament_bar, 1000);
    lv_bar_set_value(filament_bar, 0, LV_ANIM_ON);
  }
  else {
      lv_obj_t *labelOk;
      lv_obj_t *labelCancel;

      // if(DIALOG_IS(TYPE_RECOVERY_TIPS)||DIALOG_IS(TYPE_FINISH_PRINT))
      // {
      //   btnOk = lv_btn_create(mks_printer.user_src, btn_ok_event_cb, 0, &dialog_page.style_btn);
      //   btnCancel = lv_btn_create(mks_printer.user_src, btn_cancel_event_cb, 0, &dialog_page.style_btn1);
      // }
      // else{
        if (DIALOG_IS(TYPE_STOP)) {
          btnOk = lv_btn_create(mks_printer.user_src, btn_ok_event_cb, 0, &dialog_page.style_btn3);
          btnCancel = lv_btn_create(mks_printer.user_src, btn_cancel_event_cb, 0, &dialog_page.style_btn4);
        }
        else
        {
          btnOk = lv_btn_create(mks_printer.user_src, btn_ok_event_cb, 0, &dialog_page.style_btn4);
          btnCancel = lv_btn_create(mks_printer.user_src, btn_cancel_event_cb, 0, &dialog_page.style_btn3);
        }
      // }
      lv_obj_set_size(btnOk, BTN_DIALOG_W, BTN_DIALOG_H);
      lv_obj_align(btnOk, mks_printer.user_src, LV_ALIGN_IN_TOP_LEFT, BTN_DIALOG_X, BTN_DIALOG_Y1);

      labelOk = lv_label_create_empty(btnOk);
      lv_label_set_style(labelOk,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);


      lv_obj_set_size(btnCancel, BTN_DIALOG_W, BTN_DIALOG_H);
      lv_obj_align(btnCancel, mks_printer.user_src, LV_ALIGN_IN_TOP_LEFT, BTN_DIALOG_X, BTN_DIALOG_Y2);

      labelCancel = lv_label_create_empty(btnCancel);
      lv_label_set_style(labelCancel,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);

      // lv_label_set_style(label,LV_LABEL_STYLE_MAIN,lstyle);

      // labelOk = lv_label_align_create_n(mks_printer.user_src,"",LV_ALIGN_CENTER, 172, 46,LV_ALIGN_CENTER,0,60,btn_ok_event_cb,0,&User_style.btn1_bg_style);
      // labelCancel = lv_label_align_create_n(mks_printer.user_src,"",LV_ALIGN_CENTER, 172, 46,LV_ALIGN_CENTER,0,120,btn_cancel_event_cb,0,&User_style.btn2_bg_style);
      // lv_obj_align(label, btn, labelalign, -35, 0);

    // btnOk = lv_button_btn_create(mks_printer.user_src, BTN_OK_X, BTN_OK_Y, 100, 50, btn_ok_event_cb);
    // lv_obj_t *labelOk = lv_label_create_empty(btnOk);             // Add a label to the button

    // btnCancel = lv_button_btn_create(mks_printer.user_src, BTN_CANCEL_X, BTN_CANCEL_Y, 100, 50, btn_cancel_event_cb);
    // lv_obj_t *labelCancel = lv_label_create_empty(btnCancel);     // Add a label to the button

    if (DIALOG_IS(PAUSE_MESSAGE_OPTION)) {
      lv_label_set_text(labelOk, pause_msg_menu.purgeMore);       // Set the labels text
      lv_label_set_text(labelCancel, pause_msg_menu.continuePrint);
    }
    else if(DIALOG_IS(NO_FILAMENT_TIPS))
    {
      lv_label_set_text(labelOk, dialog_menu.add_filament);       // Set the labels text
      lv_label_set_text(labelCancel, common_menu.text_back);
    }
    else if(DIALOG_IS(TYPE_RECOVERY_TIPS))
    {
      // lv_obj_set_size(btnOk, 173, 58);
      // lv_obj_set_size(btnCancel, 173, 58);
      // lv_obj_set_pos(btnOk,34,168);
      // lv_obj_set_pos(btnCancel,34,247);
      lv_label_set_text(labelOk, dialog_menu.recovery_continue_btn);       // Set the labels text
      lv_label_set_text(labelCancel, dialog_menu.recovery_back_btn);
    }
    else if (DIALOG_IS(TYPE_FINISH_PRINT)) {
      // lv_obj_set_size(btnOk, 173, 58);
      // lv_obj_set_size(btnCancel, 173, 58);
      // lv_obj_set_pos(btnOk,34,159);
      // lv_obj_set_pos(btnCancel,34,240);
      lv_label_set_text(labelOk, dialog_menu.complete_btn);       // Set the labels text
      lv_label_set_text(labelCancel, dialog_menu.print_again);
    }
    else {
      lv_label_set_text(labelOk, print_file_dialog_menu.confirm); // Set the labels text
      lv_label_set_text(labelCancel, print_file_dialog_menu.cancel);
    }
  }
  if (DIALOG_IS(TYPE_PRINT_FILE)) {
    lv_label_set_text(labelDialog, print_file_dialog_menu.print_file);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -50);

    ZERO(public_buf_m);
    cutFileName((char *)list_file.long_name[sel_id], 20, 20, (char *)public_buf_m); 
    lv_obj_t *labelFile = lv_label_create(mks_printer.user_src, public_buf_m);
    if(gCfgItems.language == LANG_SIMPLE_CHINESE)
    lv_label_set_style(labelFile,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
    else
    lv_label_set_style(labelFile,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);

    lv_obj_align(labelFile, nullptr, LV_ALIGN_CENTER, 0, -65);
  }
  else if (DIALOG_IS(TYPE_STOP)) {
    thermalManager.set_fan_speed(1,0);
    lv_label_set_text(labelDialog, print_file_dialog_menu.cancel_print);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -35);
  }
  else if (DIALOG_IS(TYPE_FINISH_PRINT)) {
    thermalManager.set_fan_speed(1,0);
    // char buff[30];
    lv_label_set_text(labelDialog, print_file_dialog_menu.print_finish);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -60);

    lv_obj_t *labelDialog1 = lv_label_create(mks_printer.user_src, "");

    // if(list_file.long_name[sel_id][0] == '/')
    // {
    //   strcpy(buff,list_file.long_name[sel_id]);
    //   buff[0] = ' ';
    // }
    // else
    // {
    //   strcpy(buff,list_file.long_name[sel_id]);
    // }
    ZERO(public_buf_m);
    cutFileName((char *)list_file.long_name[sel_id], 20, 20, (char *)public_buf_m); 
    lv_label_set_text(labelDialog1, public_buf_m);
    

    lv_obj_t *labelDialog2 = lv_label_create(mks_printer.user_src, "");
    if(gCfgItems.language == LANG_SIMPLE_CHINESE)
    {

    lv_label_set_style(labelDialog1,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
    lv_label_set_style(labelDialog2,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
    lv_obj_align(labelDialog1, mks_printer.user_src, LV_ALIGN_CENTER, 0, -90);
    }
    else
    {
    lv_label_set_style(labelDialog1,LV_LABEL_STYLE_MAIN,&User_style.text22_black_style);
    lv_label_set_style(labelDialog2,LV_LABEL_STYLE_MAIN,&User_style.text22_black_style);
    lv_obj_align(labelDialog1, mks_printer.user_src, LV_ALIGN_CENTER, 0, -90);
    }

    sprintf_P(public_buf_l, PSTR("When %d%d:%d%d:%d%d"), print_time.hours / 10, print_time.hours % 10, print_time.minutes / 10, print_time.minutes % 10, print_time.seconds / 10, print_time.seconds % 10);
    lv_label_set_text(labelDialog2, public_buf_l);
    lv_obj_align(labelDialog2, nullptr, LV_ALIGN_CENTER, 0, -40);
    stop_print_time();

  }
  else if(DIALOG_IS(TYPE_RECOVERY_TIPS))
  {
    char buff[20];
    // lv_label_set_text(labelDialog, dialog_menu.recovery_text);
    lv_label_set_text(labelDialog, "Found a file has mot been\n              printed yet!");

    lv_obj_t *labelDialog1 = lv_label_create(mks_printer.user_src, "");

    // if(list_file.long_name[sel_id][0] == '/')
    // {
    //   strcpy(buff,list_file.long_name[sel_id]);
    //   buff[0] = ' ';
    // }
    // else
    // {
    //   strcpy(buff,list_file.long_name[sel_id]);
    // }
    ZERO(public_buf_m);
    cutFileName((char *)list_file.long_name[sel_id], 20, 20, (char *)public_buf_m); 
    lv_label_set_text(labelDialog1, public_buf_m);
    // lv_label_set_text(labelDialog1, buff);
    if(gCfgItems.language == LANG_SIMPLE_CHINESE)
    {
      lv_label_set_style(labelDialog,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
      lv_label_set_style(labelDialog1,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
      lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 10, -55);
      lv_obj_align(labelDialog1, nullptr, LV_ALIGN_CENTER, 0, 10);
    }
    else
    {
      lv_label_set_style(labelDialog,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
      lv_label_set_style(labelDialog1,LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
      lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -65);
      lv_obj_align(labelDialog1, labelDialog, LV_ALIGN_CENTER, 0, 65);
    }
  }
  else if(DIALOG_IS(EMERGENCY_TIPS))
  {
    lv_obj_t *labelDialog1 = lv_label_create(mks_printer.user_src, "");
    // lv_obj_t *labelDialog2 = lv_label_create(mks_printer.user_src, "");
    // lv_obj_t *labelDialog3 = lv_label_create(mks_printer.user_src, "");
    // lv_obj_t *labelDialog4 = lv_label_create(mks_printer.user_src, "");
    lv_label_set_text(labelDialog1, dialog_menu.emergency_text1);       
    lv_label_set_style(labelDialog1,LV_LABEL_STYLE_MAIN,&User_style.en_text_black_style);
    // lv_label_set_text(labelDialog2, dialog_menu.emergency_text2);
    // lv_label_set_text(labelDialog3, dialog_menu.emergency_text3);
    // lv_label_set_text(labelDialog4, dialog_menu.emergency_text4);
     if(gCfgItems.language == LANG_SIMPLE_CHINESE)
    {
      lv_obj_align(labelDialog1, nullptr, LV_ALIGN_CENTER, 0, -40);
      // lv_obj_align(labelDialog2, nullptr, LV_ALIGN_IN_TOP_LEFT, 115, 156);
      // lv_obj_align(labelDialog3, nullptr, LV_ALIGN_IN_TOP_LEFT, 115, 176);
      // lv_obj_align(labelDialog4, nullptr, LV_ALIGN_IN_TOP_LEFT, 115, 196); 
    }
    else
    {
      lv_obj_align(labelDialog1, nullptr, LV_ALIGN_CENTER, 0, -40);
      // lv_obj_align(labelDialog2, nullptr, LV_ALIGN_IN_TOP_LEFT, 65, 156);
      // lv_obj_align(labelDialog3, nullptr, LV_ALIGN_IN_TOP_LEFT, 65, 176);
      // lv_obj_align(labelDialog4, nullptr, LV_ALIGN_IN_TOP_LEFT, 85, 196); 
    } 
  }
  else if(DIALOG_IS(NO_FILAMENT_TIPS))
  {
    lv_obj_t *labelDialog1 = lv_label_create(mks_printer.user_src, "");
    lv_obj_t *labelDialog2 = lv_label_create(mks_printer.user_src, "");

    lv_label_set_text(labelDialog, dialog_menu.no_filament_text1);
    lv_label_set_text(labelDialog1, dialog_menu.no_filament_text2);
    lv_label_set_text(labelDialog2, dialog_menu.no_filament_text3);

    lv_label_set_style(labelDialog,LV_LABEL_STYLE_MAIN,&User_style.en_text_black_style);
    lv_label_set_style(labelDialog1,LV_LABEL_STYLE_MAIN,&User_style.en_text_black_style);
    lv_label_set_style(labelDialog2,LV_LABEL_STYLE_MAIN,&User_style.en_text_black_style);

    if(gCfgItems.language == LANG_SIMPLE_CHINESE)
    {
      lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -70);
      lv_obj_align(labelDialog1, nullptr, LV_ALIGN_CENTER, 0, -40);
      lv_obj_align(labelDialog2, nullptr, LV_ALIGN_CENTER, 0, -10);
    }
    else
    {
      lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -70);
      lv_obj_align(labelDialog1, nullptr, LV_ALIGN_CENTER, 0, -40);
      lv_obj_align(labelDialog2, nullptr, LV_ALIGN_CENTER, 0, -10);
    }

  }
  else if (DIALOG_IS(PAUSE_MESSAGE_PARKING)) {
    lv_label_set_text(labelDialog, pause_msg_menu.pausing);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_CHANGING)) {
    lv_label_set_text(labelDialog, pause_msg_menu.changing);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_UNLOAD)) {
    lv_label_set_text(labelDialog, pause_msg_menu.unload);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_WAITING)) {
    lv_label_set_text(labelDialog, pause_msg_menu.waiting);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_INSERT)) {
    lv_label_set_text(labelDialog, pause_msg_menu.insert);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_LOAD)) {
    lv_label_set_text(labelDialog, pause_msg_menu.load);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_PURGE)) {
    lv_label_set_text(labelDialog, pause_msg_menu.purge);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_RESUME)) {
    lv_label_set_text(labelDialog, pause_msg_menu.resume);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_HEAT)) {
    lv_label_set_text(labelDialog, pause_msg_menu.heat);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_HEATING)) {
    lv_label_set_text(labelDialog, pause_msg_menu.heating);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(PAUSE_MESSAGE_OPTION)) {
    lv_label_set_text(labelDialog, pause_msg_menu.option);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(STORE_EEPROM_TIPS)) {
    lv_label_set_text(labelDialog, eeprom_menu.storeTips);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(READ_EEPROM_TIPS)) {
    lv_label_set_text(labelDialog, eeprom_menu.readTips);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(REVERT_EEPROM_TIPS)) {
    lv_label_set_text(labelDialog, eeprom_menu.revertTips);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(WIFI_CONFIG_TIPS)) {
    lv_label_set_text(labelDialog, machine_menu.wifiConfigTips);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(WIFI_ENABLE_TIPS)) {
    lv_label_set_text(labelDialog, print_file_dialog_menu.wifi_enable_tips);
    lv_label_set_style(labelDialog,LV_LABEL_STYLE_MAIN,&User_style.small_text_black_style);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(TRANSFER_NO_DEVICE)) {
    lv_label_set_text(labelDialog, DIALOG_UPDATE_NO_DEVICE_EN);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  #if ENABLED(MKS_WIFI_MODULE)
    else if (DIALOG_IS(TYPE_UPLOAD_FILE)) {
      if (upload_result == 1) {
        lv_label_set_text(labelDialog, DIALOG_UPLOAD_ING_EN);
        lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
      }
      else if (upload_result == 2) {
        lv_label_set_text(labelDialog, DIALOG_UPLOAD_ERROR_EN);
        lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
      }
      else if (upload_result == 3) {
        char buf[200];
        int _index = 0;

        strcpy_P(buf, PSTR(DIALOG_UPLOAD_FINISH_EN));
        _index = strlen(buf);
        buf[_index++] = '\n';
        strcat_P(buf, PSTR(DIALOG_UPLOAD_SIZE_EN));

        _index = strlen(buf);
        buf[_index++] = ':';
        sprintf_P(&buf[_index], PSTR(" %d KBytes\n"), (int)(upload_size / 1024));

        strcat_P(buf, PSTR(DIALOG_UPLOAD_TIME_EN));
        _index = strlen(buf);
        buf[_index++] = ':';
        sprintf_P(&buf[_index], PSTR(" %d s\n"), (int)upload_time_sec);

        strcat_P(buf, PSTR(DIALOG_UPLOAD_SPEED_EN));
        _index = strlen(buf);
        buf[_index++] = ':';
        sprintf_P(&buf[_index], PSTR(" %d KBytes/s\n"), (int)(upload_size / upload_time_sec / 1024));

        lv_label_set_text(labelDialog, buf);
        lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
      }
    }
    else if (DIALOG_IS(TYPE_UPDATE_ESP_FIRMWARE)) {
      lv_label_set_text(labelDialog, DIALOG_UPDATE_WIFI_FIRMWARE_EN);
      lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
    }
  #endif // MKS_WIFI_MODULE
  else if (DIALOG_IS(TYPE_FILAMENT_LOAD_HEAT)) {
    lv_label_set_text(labelDialog, filament_menu.filament_dialog_load_heat);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(TYPE_FILAMENT_HEAT_LOAD_COMPLETED)) {
    lv_label_set_text(labelDialog, filament_menu.filament_dialog_load_heat_confirm);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(TYPE_FILAMENT_UNLOAD_HEAT)) {
    lv_label_set_text(labelDialog, filament_menu.filament_dialog_unload_heat);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(TYPE_FILAMENT_HEAT_UNLOAD_COMPLETED)) {
    lv_label_set_text(labelDialog, filament_menu.filament_dialog_unload_heat_confirm);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(TYPE_FILAMENT_LOAD_COMPLETED)) {
    lv_label_set_text(labelDialog, filament_menu.filament_dialog_load_completed);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(TYPE_FILAMENT_UNLOAD_COMPLETED)) {
    lv_label_set_text(labelDialog, filament_menu.filament_dialog_unload_completed);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -20);
  }
  else if (DIALOG_IS(TYPE_FILAMENT_LOADING)) {
    lv_label_set_text(labelDialog, filament_menu.filament_dialog_loading);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -70);
  }
  else if (DIALOG_IS(TYPE_FILAMENT_UNLOADING)) {
    lv_label_set_text(labelDialog, filament_menu.filament_dialog_unloading);
    lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -70);
  }
  #if ENABLED(MKS_WIFI_MODULE)
    else if (DIALOG_IS(TYPE_UNBIND)) {
      lv_label_set_text(labelDialog, common_menu.unbind_printer_tips);
      lv_obj_align(labelDialog, nullptr, LV_ALIGN_CENTER, 0, -70);
    }
  #endif
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) {
      if (btnOk) lv_group_add_obj(g, btnOk);
      if (btnCancel) lv_group_add_obj(g, btnCancel);
    }
  #endif
}


void filament_sprayer_temp() {
  char buf[20] = {0};
  sprintf(buf, preheat_menu.value_state, thermalManager.wholeDegHotend(uiCfg.extruderIndex), thermalManager.degTargetHotend(uiCfg.extruderIndex));

  strcpy(public_buf_l, uiCfg.extruderIndex < 1 ? extrude_menu.ext1 : extrude_menu.ext2);
  strcat_P(public_buf_l, PSTR(": "));
  strcat(public_buf_l, buf);
  lv_label_set_text(tempText1, public_buf_l);
  lv_obj_align(tempText1, nullptr, LV_ALIGN_CENTER, 0, -50);
}

void filament_dialog_handle() {
  if (temps_update_flag && (DIALOG_IS(TYPE_FILAMENT_LOAD_HEAT, TYPE_FILAMENT_UNLOAD_HEAT))) {
    filament_sprayer_temp();
    temps_update_flag = false;
  }
  if (uiCfg.filament_heat_completed_load) {
    uiCfg.filament_heat_completed_load = false;
    lv_clear_dialog();
    lv_draw_dialog(DIALOG_TYPE_FILAMENT_LOADING);
    planner.synchronize();
    uiCfg.filament_loading_time_flg = true;
    uiCfg.filament_loading_time_cnt = 0;
    sprintf_P(public_buf_m, PSTR("T%d\nG91\nG1 E%d F%d\nG90"), uiCfg.extruderIndex, gCfgItems.filamentchange_load_length, gCfgItems.filamentchange_load_speed);
    queue.inject(public_buf_m);
  }
  if (uiCfg.filament_heat_completed_unload) {
    uiCfg.filament_heat_completed_unload = false;
    lv_clear_dialog();
    lv_draw_dialog(DIALOG_TYPE_FILAMENT_UNLOADING);
    planner.synchronize();
    uiCfg.filament_unloading_time_flg = true;
    uiCfg.filament_unloading_time_cnt = 0;
    sprintf_P(public_buf_m, PSTR("T%d\nG91\nG1 E-%d F%d\nG90"), uiCfg.extruderIndex, gCfgItems.filamentchange_unload_length, gCfgItems.filamentchange_unload_speed);
    queue.inject(public_buf_m);
  }

  if (uiCfg.filament_load_heat_flg) {
    const celsius_t diff = thermalManager.wholeDegHotend(uiCfg.extruderIndex) - gCfgItems.filament_limit_temp;
    if (ABS(diff) < 2 || diff > 0) {
      uiCfg.filament_load_heat_flg = false;
      lv_clear_dialog();
      lv_draw_dialog(DIALOG_TYPE_FILAMENT_HEAT_LOAD_COMPLETED);
    }
  }

  if (uiCfg.filament_loading_completed) {
    uiCfg.filament_rate = 0;
    uiCfg.filament_loading_completed = false;
    lv_clear_dialog();
    lv_draw_dialog(DIALOG_TYPE_FILAMENT_LOAD_COMPLETED);
  }

  if (uiCfg.filament_unload_heat_flg) {
    const celsius_t diff = thermalManager.wholeDegHotend(uiCfg.extruderIndex) - gCfgItems.filament_limit_temp;
    if (ABS(diff) < 2 || diff > 0) {
      uiCfg.filament_unload_heat_flg = false;
      lv_clear_dialog();
      lv_draw_dialog(DIALOG_TYPE_FILAMENT_HEAT_UNLOAD_COMPLETED);
    }
  }

  if (uiCfg.filament_unloading_completed) {
    uiCfg.filament_rate = 0;
    uiCfg.filament_unloading_completed = false;
    lv_clear_dialog();
    lv_draw_dialog(DIALOG_TYPE_FILAMENT_UNLOAD_COMPLETED);
  }

  if (DIALOG_IS(TYPE_FILAMENT_LOADING, TYPE_FILAMENT_UNLOADING))
    lv_filament_setbar();
}

void lv_filament_setbar() {
  lv_bar_set_value(filament_bar, uiCfg.filament_rate, LV_ANIM_ON);
}

void lv_clear_dialog() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_clean(mks_printer.user_src);
}

#endif // HAS_TFT_LVGL_UI
