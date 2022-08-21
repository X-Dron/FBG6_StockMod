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
#include "../../../module/motion.h"
#include "../../../inc/MarlinConfig.h"
#include "../../../gcode/gcode.h"
#include "../../../module/stepper.h"
#include "../../../module/endstops.h"

typedef struct{
  lv_obj_t *x_label;
  lv_obj_t *y_label;
  lv_obj_t *z_label;
  lv_obj_t *step_label;

  lv_obj_t *y_add_imgbtn;
  lv_obj_t *y_dec_imgbtn;  

  lv_obj_t *x_add_imgbtn;
  lv_obj_t *x_dec_imgbtn;

  lv_obj_t *z_add_imgbtn;
  lv_obj_t *z_dec_imgbtn;

  lv_obj_t *step_imgbtn;
  lv_obj_t *moto_imgbtn;
  lv_obj_t *back_imgbtn;

  lv_obj_t *home_imgbtn;


}MOVE_UI_PAGE;

MOVE_UI_PAGE move_page;

extern lv_group_t *g;
bool g_emergency_endstop_flag = false;
bool gb_emergency_stop = false;
bool g_emergency_stop_flag = false;
// bool stop_home = false;
// bool return_home = false;
// bool stoping_home = false;
#ifndef USE_NEW_LVGL_CONF
static lv_obj_t *scr;
#endif

extern bool g_emergency_endstop;
extern bool g_emergency_home;

static lv_obj_t *labelV, *buttonV, *labelP;
static lv_task_t *updatePosTask;
static char cur_label = 'Z';
static float cur_pos = 0;
static bool home_flag = false;
enum {
  ID_M_X_P = 1,
  ID_M_X_N,
  ID_M_Y_P,
  ID_M_Y_N,
  ID_M_Z_P,
  ID_M_Z_N,
  ID_M_ENABLE,
  ID_M_STEP,
  ID_M_HOME,
  ID_M_RETURN
};

void disp_cur_pos() {
  char str_1[16];
  sprintf_P(public_buf_l, PSTR("%c:%s mm"), cur_label, dtostrf(cur_pos, 1, 1, str_1));
  if (labelP) lv_label_set_text(labelP, public_buf_l);
}

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  char str_1[16];
 if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
      button_sound_end();
    if (!queue.ring_buffer.full(3)) {
      bool do_inject = true;
      float dist = uiCfg.move_dist;
      switch (obj->mks_obj_id) {
        case ID_M_X_N: dist *= -1; case ID_M_X_P: cur_label = 'X'; break;
        case ID_M_Y_N: dist *= -1; case ID_M_Y_P: cur_label = 'Y'; break;
        case ID_M_Z_N: dist *= -1; case ID_M_Z_P: cur_label = 'Z'; break;
        default: do_inject = false;
      }
      if (do_inject) {
        sprintf_P(public_buf_l, PSTR("G91\nG1 %c%s F%d\nG90"), cur_label, dtostrf(dist, 1, 3, str_1), uiCfg.moveSpeed);
        queue.inject(public_buf_l);
      }
    }

    switch (obj->mks_obj_id) {
      case ID_M_HOME:
        // if(g_emergency_home)
        // {
          // g_emergency_home = false;
          g_emergency_endstop = false;
          g_emergency_stop_flag = false;
          home_flag = true;
          queue.inject_P(G28_STR);
        // }
      break;
      case ID_M_STEP:
        if (ABS(10 * (int)uiCfg.move_dist) == 100)
          uiCfg.move_dist = 0.1;
        else
          uiCfg.move_dist *= 10.0f;
        disp_move_dist();
        break;
      case ID_M_ENABLE:

        if(home_flag)
        {
          queue.clear();
          memset(planner.block_buffer, 0, sizeof(planner.block_buffer));
          memset(queue.injected_commands, 0, sizeof(queue.injected_commands));
          while(planner.has_blocks_queued())
          {
            stepper.discard_current_block();
          }
          planner.quick_stop();
          g_emergency_stop_flag = true;
          g_emergency_endstop = true;
          home_flag = false;
          gb_emergency_stop = true;
          TERN(HAS_SUICIDE, suicide(), gcode.process_subcommands_now(PSTR("M410")));
          gcode.process_subcommands_now(PSTR("M84"));

        }
        else
        {
          TERN(HAS_SUICIDE, suicide(), gcode.process_subcommands_now(PSTR("M410")));
          gcode.process_subcommands_now(PSTR("M84"));
        }
        break;
      case ID_M_RETURN:
        // endstops.enable(true);
        // if(g_emergency_home && g_emergency_endstop)
        // {
          g_emergency_endstop = false;
          g_emergency_stop_flag = false;
          clear_cur_ui();
          draw_return_ui();
        // }
        // if(!g_emergency_home)
        // {
        //   clear_cur_ui();
        //   draw_return_ui();
        // }
        return;
    }
  // disp_cur_pos();
  }
}
void refresh_pos(lv_task_t *) {
  switch (cur_label) {
    case 'X': cur_pos = current_position.x; break;
    case 'Y': cur_pos = current_position.y; break;
    case 'Z': cur_pos = current_position.z; break;
    default: return;
  }
  disp_cur_pos();
}
void move_data_refresh()
{
  char str_1[16];
  float pos_x = current_position.x - (X_MIN_POS);
  if(pos_x < 0)
  {
    pos_x = 0;
  }
  sprintf_P(public_buf_l, PSTR("X %smm"),dtostrf(pos_x, 1, 1, str_1));
  if ( move_page.x_label)
  {
    lv_label_set_text( move_page.x_label, public_buf_l);
  lv_obj_align(move_page.x_label, mks_printer.user_src1, LV_ALIGN_IN_LEFT_MID,  5, 0);
  } 
  memset(str_1,0,sizeof(str_1));
  memset(public_buf_l,0,sizeof(public_buf_l));
  float pos_y = current_position.y - (Y_MIN_POS);
  sprintf_P(public_buf_l, PSTR("Y %smm"),dtostrf(pos_y, 1, 1, str_1));
  if ( move_page.y_label){ 
  lv_label_set_text( move_page.y_label, public_buf_l);
  lv_obj_align(move_page.y_label, mks_printer.user_src1, LV_ALIGN_CENTER,  0, 0);
  }
  memset(str_1,0,sizeof(str_1));
  memset(public_buf_l,0,sizeof(public_buf_l));
  sprintf_P(public_buf_l, PSTR("Z %smm"), dtostrf(current_position.z, 1, 1, str_1));
  if ( move_page.z_label) {
  lv_label_set_text( move_page.z_label, public_buf_l);
  lv_obj_align(move_page.z_label, mks_printer.user_src1, LV_ALIGN_IN_RIGHT_MID,  -5, 0);
  }
}
void lv_draw_move_motor() {
  lv_screen_distribution(MOVE_MOTOR_UI);
  // mks_printer.user_src1 = lv_title_create(mks_printer.user_src);
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src);
  lv_refr_now(lv_refr_get_disp_refreshing());

  // mks_printer.user_src = lv_scr_create(mks_printer.user_src,0,TITLE_H,TITLE_W,TFT_HEIGHT-TITLE_H);
  move_page.x_label = lv_label_create_empty(mks_printer.user_src1);
  lv_label_set_text(move_page.x_label,"X 0.0mm");
  // lv_obj_align(move_page.x_label, mks_printer.user_src1, LV_ALIGN_IN_LEFT_MID,  5, 0);
  lv_label_set_style(move_page.x_label,LV_LABEL_STYLE_MAIN,&User_style.text16_white_style);

  move_page.y_label = lv_label_create_empty(mks_printer.user_src1);
  lv_label_set_text(move_page.y_label,"Y 0.0mm");
  // lv_obj_align(move_page.y_label, mks_printer.user_src1, LV_ALIGN_CENTER,  0, 0);
  lv_label_set_style(move_page.y_label,LV_LABEL_STYLE_MAIN,&User_style.text16_white_style);

  move_page.z_label = lv_label_create_empty(mks_printer.user_src1);
  lv_label_set_text(move_page.z_label,"Z 0.0mm");
  // lv_obj_align(move_page.z_label, mks_printer.user_src1, LV_ALIGN_IN_RIGHT_MID,  -5, 0);
  lv_label_set_style(move_page.z_label,LV_LABEL_STYLE_MAIN,&User_style.text16_white_style);
  lv_obj_align(move_page.x_label, mks_printer.user_src1, LV_ALIGN_IN_LEFT_MID,  5, 0);
  lv_obj_align(move_page.y_label, mks_printer.user_src1, LV_ALIGN_CENTER,  0, 0);
  lv_obj_align(move_page.z_label, mks_printer.user_src1, LV_ALIGN_IN_RIGHT_MID,  -5, 0);

  // move_page.x_label = lv_label_align_create(mks_printer.user_src1,"X:0.00mm", 5, 7,LV_ALIGN_IN_LEFT_MID,  &User_style.small_text_white_style);
  // move_page.y_label = lv_label_align_create(mks_printer.user_src1,"Y:0.00mm", 0, 7,LV_ALIGN_CENTER,  &User_style.small_text_white_style);
  // move_page.z_label = lv_label_align_create(mks_printer.user_src1,"Z:0.00mm", -5, 7,LV_ALIGN_IN_RIGHT_MID,  &User_style.small_text_white_style);


  move_page.y_add_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_y_add.bin", LV_ALIGN_IN_TOP_MID, 0, 70, event_handler, ID_M_Y_P);
  move_page.y_dec_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_y_dec.bin", LV_ALIGN_IN_BOTTOM_MID, 0, -120, event_handler, ID_M_Y_N);
  move_page.x_add_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_x_add.bin", LV_ALIGN_IN_LEFT_MID, 5, -80-15, event_handler, ID_M_X_P);
  move_page.x_dec_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_x_dec.bin", LV_ALIGN_IN_LEFT_MID, 5, 0+30, event_handler, ID_M_X_N);
  
  
  move_page.z_add_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_z_add.bin", LV_ALIGN_IN_RIGHT_MID, -5, -80-15, event_handler, ID_M_Z_P);
  move_page.z_dec_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_z_dec.bin", LV_ALIGN_IN_RIGHT_MID, -5, 0+30, event_handler, ID_M_Z_N);
  move_page.home_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_home.bin", LV_ALIGN_CENTER, 0, -40+15, event_handler, ID_M_HOME);
  move_page.step_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_mv_step.bin", LV_ALIGN_IN_BOTTOM_LEFT, 10, -10, event_handler, ID_M_STEP);
  move_page.moto_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_em_ctr.bin", LV_ALIGN_IN_BOTTOM_MID, 0, -10, event_handler, ID_M_ENABLE);
  move_page.back_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_back3.bin", LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10, event_handler, ID_M_RETURN);

  if ((int)(10 * uiCfg.move_dist) == 1)
    move_page.step_label = lv_label_create(move_page.step_imgbtn,"0.1mm");
  else if ((int)(10 * uiCfg.move_dist) == 10)
    move_page.step_label = lv_label_create(move_page.step_imgbtn,"1mm");
  else if ((int)(10 * uiCfg.move_dist) == 100)
    move_page.step_label = lv_label_create(move_page.step_imgbtn,"10mm");
  lv_label_set_style(move_page.step_label,LV_LABEL_STYLE_MAIN,&User_style.text_white_style);
  lv_obj_align(move_page.step_label,move_page.step_imgbtn,LV_ALIGN_IN_BOTTOM_MID,0,-10);
  disp_move_dist();
  // lv_refr_now(lv_refr_get_disp_refreshing());
  // lv_refr_now(lv_refr_get_disp_refreshing());
  // lv_imgbtn_set_src_both(move_page.y_dec_imgbtn,  "F:/png_y_dec.bin");
  // lv_imgbtn_set_src_both(move_page.x_add_imgbtn,  "F:/png_x_add.bin");
  // lv_imgbtn_set_src_both(move_page.x_dec_imgbtn,  "F:/png_x_dec.bin");
  // lv_imgbtn_set_src_both(move_page.z_add_imgbtn,  "F:/png_z_add.bin");
  // lv_imgbtn_set_src_both(move_page.z_dec_imgbtn,  "F:/png_z_dec.bin");
  // lv_imgbtn_set_src_both(move_page.home_imgbtn,  "F:/png_home.bin");
  // lv_imgbtn_set_src_both(move_page.step_imgbtn,  "F:/png_move_step.bin");
  // lv_imgbtn_set_src_both(move_page.moto_imgbtn,  "F:/move_moto_disable.bin");
  // lv_imgbtn_set_src_both(move_page.back_imgbtn,  "F:/back.bin");
  // lv_imgbtn_set_src_both(move_page.y_add_imgbtn,  "F:/png_y_add.bin");

  // scr = lv_screen_create(MOVE_MOTOR_UI);
  // lv_obj_t *buttonXI = lv_big_button_create(scr, "F:/bmp_xAdd.bin", move_menu.x_add, INTERVAL_V, titleHeight, event_handler, ID_M_X_P);
  // lv_obj_clear_protect(buttonXI, LV_PROTECT_FOLLOW);
  // lv_big_button_create(scr, "F:/bmp_xDec.bin", move_menu.x_dec, INTERVAL_V, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_M_X_N);
  // lv_big_button_create(scr, "F:/bmp_yAdd.bin", move_menu.y_add, BTN_X_PIXEL + INTERVAL_V * 2, titleHeight, event_handler, ID_M_Y_P);
  // lv_big_button_create(scr, "F:/bmp_yDec.bin", move_menu.y_dec, BTN_X_PIXEL + INTERVAL_V * 2, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_M_Y_N);
  // lv_big_button_create(scr, "F:/bmp_zAdd.bin", move_menu.z_add, BTN_X_PIXEL * 2 + INTERVAL_V * 3, titleHeight, event_handler, ID_M_Z_P);
  // lv_big_button_create(scr, "F:/bmp_zDec.bin", move_menu.z_dec, BTN_X_PIXEL * 2 + INTERVAL_V * 3, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_M_Z_N);

  // // button with image and label changed dynamically by disp_move_dist
  // buttonV = lv_imgbtn_create(scr, nullptr, BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_M_STEP);
  // labelV = lv_label_create_empty(buttonV);
  // #if HAS_ROTARY_ENCODER
  //   if (gCfgItems.encoder_enable) lv_group_add_obj(g, buttonV);
  // #endif

  // lv_big_button_create(scr, "F:/bmp_return.bin", common_menu.text_back, BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_M_RETURN);

  // // We need to patch the title to leave some space on the right for displaying the status
  // lv_obj_t * title = lv_obj_get_child_back(scr, nullptr);
  // if (title != nullptr) lv_obj_set_width(title, TFT_WIDTH - 101);
  // labelP = lv_label_create(scr, TFT_WIDTH - 100, TITLE_YPOS, "Z:0.0mm");
  // if (labelP != nullptr)
  //   updatePosTask = lv_task_create(refresh_pos, 300, LV_TASK_PRIO_LOWEST, 0);

  // disp_move_dist();
  // disp_cur_pos();
}

void disp_move_dist() {
  if ((int)(10 * uiCfg.move_dist) == 1)
  lv_label_set_text(move_page.step_label,"0.1mm");
  else if ((int)(10 * uiCfg.move_dist) == 10)
  lv_label_set_text(move_page.step_label,"1mm");
  else if ((int)(10 * uiCfg.move_dist) == 100)
  lv_label_set_text(move_page.step_label,"10mm");
  lv_obj_align(move_page.step_label,move_page.step_imgbtn,LV_ALIGN_IN_BOTTOM_MID,0,-10);

  // if (gCfgItems.multiple_language) {
  //   if ((int)(10 * uiCfg.move_dist) == 1) {
  //     lv_label_set_text(move_page.step_imgbtn, move_menu.step_01mm);
  //     lv_obj_align(move_page.step_imgbtn, buttonV, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
  //   }
  //   else if ((int)(10 * uiCfg.move_dist) == 10) {
  //     lv_label_set_text(move_page.step_imgbtn, move_menu.step_1mm);
  //     lv_obj_align(move_page.step_imgbtn, buttonV, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
  //   }
  //   else if ((int)(10 * uiCfg.move_dist) == 100) {
  //     lv_label_set_text(move_page.step_imgbtn, move_menu.step_10mm);
  //     lv_obj_align(move_page.step_imgbtn, buttonV, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
  //   }
  // }
}

void lv_clear_move_motor() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  // lv_task_del(updatePosTask);
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);
}

#endif // HAS_TFT_LVGL_UI
