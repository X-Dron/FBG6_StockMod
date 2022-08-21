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

#include <lv_conf.h>
#include "tft_lvgl_configuration.h"

#if ENABLED(MKS_WIFI_MODULE)

#include "draw_ui.h"

#define NAME_BTN_X 278
#define NAME_BTN_Y 63
#define NAME_BTN_W 60

#define MARK_BTN_X 0
#define MARK_BTN_Y 68

#define OFFSET_X 21
#define OFFSET_FIRST_Y 52

WIFI_LIST wifi_list;
list_menu_def list_menu;
uint32_t wifi_node;
extern lv_group_t *g;
// static lv_obj_t *mks_printer.user_src;
static lv_obj_t *buttonWifiN[NUMBER_OF_PAGE];
static lv_obj_t *labelWifiText[NUMBER_OF_PAGE];
static lv_obj_t *labelPageText;

static lv_point_t wifi_line_points[][2] = { {{21, 115}, {21+279, 115}},
                                            {{21, 177}, {21+279, 177}},
                                            {{21, 239}, {21+279, 239}},
                                            {{21, 302}, {21+279, 302}},
                                            {{21, 362}, {21+279, 362}} };
static lv_style_t style_wifi_line;
static lv_obj_t * wifi_line[NUMBER_OF_PAGE];


#define ID_WL_RETURN 11
#define ID_WL_DOWN   12
#define ID_WL_UP     13

uint16_t wifi_name_node[NUMBER_OF_PAGE] = {0,0,0,0,0};

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
    button_sound_end();

  if (obj->mks_obj_id == ID_WL_RETURN) {
    // wifi_del_line();
    clear_cur_ui();
    lv_draw_tool();
  }
  else if (obj->mks_obj_id == ID_WL_UP)
  { 
    if (wifi_list.getNameNum > 0) {
      if (wifi_list.nameIndex >= NUMBER_OF_PAGE) {
        wifi_list.nameIndex -= NUMBER_OF_PAGE;
        wifi_list.currentWifipage--;
      }
      // wifi_del_line();
      disp_wifi_list();
    }
  }
  else if (obj->mks_obj_id == ID_WL_DOWN) {
    if (wifi_list.getNameNum > 0) {
      if ((wifi_list.nameIndex + NUMBER_OF_PAGE) <= wifi_list.getNameNum) {
        wifi_list.nameIndex += NUMBER_OF_PAGE;
        wifi_list.currentWifipage++;
      }
      // wifi_del_line();
      disp_wifi_list();
    }
  }
  else {
    for (uint8_t i = 0; i < NUMBER_OF_PAGE; i++) {
      if (obj->mks_obj_id == i + 1) {
        if (wifi_list.getNameNum != 0) {
          const bool do_wifi = wifi_link_state == WIFI_CONNECTED && strcmp((const char *)wifi_list.wifiConnectedName, (const char *)wifi_list.wifiName[wifi_list.nameIndex + i]) == 0;
          wifi_list.nameIndex += i;
          last_disp_state = WIFI_LIST_UI;
          // wifi_del_line();
          lv_clear_wifi_list();
          if (do_wifi)
            lv_draw_wifi();
          else {
            wifi_node = wifi_name_node[i];
            keyboard_value = wifiConfig;
            mks_draw_simple_num_kb(1,WIFI_PASSWORD);
            // lv_draw_keyboard();
          }
        }
      }
    }
  }
  }
}
lv_style_t style_wifi_btn;
lv_style_t style_wifi_btn_pre;

void lv_draw_wifi_list() {
  // mks_printer.user_src = lv_screen_create(WIFI_LIST_UI);
  lv_refr_now(lv_refr_get_disp_refreshing());
  lv_screen_distribution(WIFI_LIST_UI); 
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(WIFI_LIST_UI),10);
  lv_style_copy(&style_wifi_line, &lv_style_plain);
  style_wifi_line.line.color = LV_COLOR_GRAY;
  style_wifi_line.line.width = 1;
  style_wifi_line.line.rounded = 1;
  lv_obj_t *buttonUp = lv_imgbtn_create(mks_printer.user_src, "F:/png_pre.bin", 12, 387, event_handler, ID_WL_UP);
  lv_obj_t *buttonDown = lv_imgbtn_create(mks_printer.user_src, "F:/png_next.bin", 121, 387, event_handler, ID_WL_DOWN);
  lv_obj_t *buttonBack = lv_imgbtn_create(mks_printer.user_src, "F:/png_back4.bin", 229, 387, event_handler, ID_WL_RETURN);
  lv_style_copy(&style_wifi_btn,&tft_style_label_rel);

  // style_wifi_btn.body.main_color = LV_COLOR_BLACK;
  // style_wifi_btn.body.grad_color = LV_COLOR_BLACK;
  // style_wifi_btn.body.opa = 15;
  lv_style_copy(&style_wifi_btn_pre,&style_sel_text);


  for (uint8_t i = 0; i < NUMBER_OF_PAGE; i++) {
    buttonWifiN[i] = lv_label_btn_create(mks_printer.user_src, 21, NAME_BTN_Y * i + 1 + 52, NAME_BTN_X, NAME_BTN_W, event_handler, i + 1);
    labelWifiText[i] = lv_label_create_empty(buttonWifiN[i]);
    wifi_line[i] = lv_line_create(mks_printer.user_src, NULL);

    #if HAS_ROTARY_ENCODER
      uint8_t j = 0;
      if (gCfgItems.encoder_enable) {
        j = wifi_list.nameIndex + i;
        if (j < wifi_list.getNameNum) lv_group_add_obj(g, buttonWifiN[i]);
      }
    #endif

  }

  // labelPageText = lv_label_create_empty(mks_printer.user_src);
  // lv_obj_set_style(labelPageText, &tft_style_label_rel);

  wifi_list.nameIndex = 0;
  wifi_list.currentWifipage = 1;

  if (wifi_link_state == WIFI_CONNECTED && wifiPara.mode == STA_MODEL) {
    ZERO(wifi_list.wifiConnectedName);
    memcpy(wifi_list.wifiConnectedName, wifiPara.ap_name, sizeof(wifi_list.wifiConnectedName));
  }

  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) {
      lv_group_add_obj(g, buttonDown);
      lv_group_add_obj(g, buttonBack);
    }
  #else
    UNUSED(buttonDown);
    UNUSED(buttonBack);
  #endif

  disp_wifi_list();

  lv_refr_now(lv_refr_get_disp_refreshing());
  lv_imgbtn_set_src_both(buttonUp,  "F:/png_pre.bin");
  lv_imgbtn_set_src_both(buttonDown,  "F:/png_next.bin");
  lv_imgbtn_set_src_both(buttonBack,  "F:/png_back4.bin");
}

void disp_wifi_list() {
  int8_t tmpStr[WIFI_NAME_BUFFER_SIZE] = { 0 };
  uint8_t i, j;

  // sprintf((char *)tmpStr, list_menu.file_pages, wifi_list.currentWifipage, wifi_list.getPage);
  // lv_label_set_text(labelPageText, (const char *)tmpStr);
  // lv_obj_align(labelPageText, nullptr, LV_ALIGN_CENTER, 50, -100);

  for (i = 0; i < NUMBER_OF_PAGE; i++) {
    ZERO(tmpStr);
    
    j = wifi_list.nameIndex + i;
    wifi_name_node[i] = j;
    if (j >= wifi_list.getNameNum) {
      lv_label_set_text(labelWifiText[i], (const char *)tmpStr);
      lv_obj_align(labelWifiText[i], buttonWifiN[i], LV_ALIGN_IN_LEFT_MID, 20, 0);
      lv_label_set_style(labelWifiText[i],LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
      lv_btn_set_style(buttonWifiN[i], LV_BTN_STYLE_REL, &style_wifi_btn);

      // lv_refr_now(lv_refr_get_disp_refreshing());
    }
    else {
      lv_label_set_text(labelWifiText[i], (char const *)wifi_list.wifiName[j]);
      lv_obj_align(labelWifiText[i], buttonWifiN[i], LV_ALIGN_IN_LEFT_MID, 20, 0);

      const bool btext = (wifi_link_state == WIFI_CONNECTED && strcmp((const char *)wifi_list.wifiConnectedName, (const char *)wifi_list.wifiName[j]) == 0);
      if(btext)
      {
        lv_btn_set_style(buttonWifiN[i], LV_BTN_STYLE_REL, &style_wifi_btn_pre);
      }
      else
      {
        lv_btn_set_style(buttonWifiN[i], LV_BTN_STYLE_REL, &style_wifi_btn);

      }
      // lv_btn_set_style(buttonWifiN[i], LV_BTN_STYLE_REL, btext ? &style_sel_text : &tft_style_label_rel);
      // lv_label_set_style(labelWifiText[i],LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
      // lv_refr_now(lv_refr_get_disp_refreshing());
    }
    // if(i == 1)
    // {
    //   lv_label_set_style(labelWifiText[i],LV_LABEL_STYLE_MAIN,&User_style.text_black_style);
    // }
      lv_line_set_points(wifi_line[i], wifi_line_points[i], 2);     /*Set the points*/
      lv_line_set_style(wifi_line[i], LV_LINE_STYLE_MAIN, &style_wifi_line);
      lv_obj_align(wifi_line[i], mks_printer.user_src, LV_ALIGN_IN_TOP_LEFT, 0, 0);

  }
}
void wifi_del_line()
{
  lv_obj_del(wifi_line[0]);
  lv_obj_del(wifi_line[1]);
  lv_obj_del(wifi_line[2]);
  lv_obj_del(wifi_line[3]);
  lv_obj_del(wifi_line[4]);
}
void wifi_scan_handle() {
  if (!DIALOG_IS(WIFI_ENABLE_TIPS) || !uiCfg.command_send) 
  {
    if(wifi_connect_data.time >= 30000)
    {
      wifi_connect_data.time = 0;
      wifi_connect_data.start = false;
      wifi_connect_data.outtimeflag = true;
    }
    if(wifi_connect_data.outtimeflag)
    {
      wifi_outtime_label();
      wifi_connect_data.outtimeflag = false;
    }
    return;
  }
  wifi_connect_data.start = false;
  wifi_connect_data.outtimeflag = false;
  last_disp_state = DIALOG_UI;
  lv_clear_dialog();
  if (wifi_link_state == WIFI_CONNECTED && wifiPara.mode != AP_MODEL)
    lv_draw_wifi();
  else
    lv_draw_wifi_list();
}

void lv_clear_wifi_list() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_clean(mks_printer.user_src);
}

#endif // MKS_WIFI_MODULE
#endif // HAS_TFT_LVGL_UI
