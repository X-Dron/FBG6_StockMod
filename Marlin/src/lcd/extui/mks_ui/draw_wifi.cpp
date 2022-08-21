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

extern lv_group_t *g;
// static lv_obj_t *src, *wifi_page.label_name, *wifi_page.label_number, *wifi_page.label_state, *wifi_page.label_ip;

typedef struct
{
  lv_obj_t *btn_reconnect;
  lv_obj_t *imgbtn_back;
  lv_obj_t *label_ip;
  lv_obj_t *label_name;
  lv_obj_t *label_number;
  lv_obj_t *label_state;
  lv_obj_t *label_text;
  lv_style_t style_blue_btn; //lan

}WIFI_UI_PAGE;
WIFI_UI_PAGE wifi_page;
enum {
  ID_W_RETURN = 1,
  ID_W_CLOUD,
  ID_W_RECONNECT
};
wifi_connect_data_type wifi_connect_data;
static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
    button_sound_end();
  clear_cur_ui();
  switch (obj->mks_obj_id) {
    case ID_W_RETURN:
      lv_draw_tool();
      break;
    case ID_W_CLOUD:
      lv_draw_cloud_bind();
      break;
    #if ENABLED(MKS_WIFI_MODULE)
      case ID_W_RECONNECT: {
        uint8_t cmd_wifi_list[] = { 0xA5, 0x07, 0x00, 0x00, 0xFC };
        raw_send_to_wifi(cmd_wifi_list, COUNT(cmd_wifi_list));
        lv_draw_wifi_list();
      } break;
    #endif
  }
  }
}

void lv_draw_wifi() {
  // mks_printer.user_src = lv_screen_create(WIFI_UI);User_style.text22_black_style
  static lv_style_t wifi_style;
  lv_style_copy(&wifi_style, &User_style.text22_black_style);
  wifi_style.text.font = &DengXian_16;

  lv_screen_distribution(WIFI_UI); 
  
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(WIFI_UI),10);

  lv_style_copy(&wifi_page.style_blue_btn, &lv_style_scr);
  wifi_page.style_blue_btn.body.main_color = LV_COLOR_MAKE(0x41,0x85,0xf7);
  wifi_page.style_blue_btn.body.grad_color = LV_COLOR_MAKE(0x41,0x85,0xf7);
  wifi_page.style_blue_btn.body.radius = 5;

  // lv_obj_t *buttonReconnect = nullptr, *label_Reconnect = nullptr;
  // lv_obj_t *buttonCloud = nullptr, *label_Cloud = nullptr;

  // const bool enc_ena = TERN0(HAS_ROTARY_ENCODER, gCfgItems.encoder_enable);

  if (gCfgItems.wifi_mode_sel == STA_MODEL) {

    // if (gCfgItems.cloud_enable)
    //   buttonCloud = lv_imgbtn_create(mks_printer.user_src, "F:/bmp_cloud.bin", BTN_X_PIXEL + INTERVAL_V * 2, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_W_CLOUD);

    // buttonReconnect = lv_imgbtn_create(mks_printer.user_src, "F:/png_back.bin", BTN_X_PIXEL * 2 + INTERVAL_V * 3, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_W_RECONNECT);

    #if HAS_ROTARY_ENCODER
      if (gCfgItems.cloud_enable) lv_group_add_obj(g, buttonCloud);
      if (enc_ena) lv_group_add_obj(g, buttonReconnect);
    #endif

    // label_Reconnect = lv_label_create_empty(buttonReconnect);
  wifi_page.btn_reconnect = lv_btn_create(mks_printer.user_src,NULL);//event_handler,ID_LAYER_STOP_SAVE
  lv_obj_set_event_cb_mks(wifi_page.btn_reconnect, event_handler, ID_W_RECONNECT, "", 0);
  lv_obj_set_size(wifi_page.btn_reconnect,156,80);
  lv_obj_set_pos(wifi_page.btn_reconnect,12,384);
  lv_btn_set_style_both(wifi_page.btn_reconnect,  &wifi_page.style_blue_btn);
  
  lv_label_align_create(wifi_page.btn_reconnect,wifi_menu.reconnect, 0, 0,LV_ALIGN_CENTER,   &User_style.text_white_style);

    // if (gCfgItems.cloud_enable) label_Cloud = lv_label_create_empty(buttonCloud);
  }
  wifi_page.label_text = lv_label_create_empty(mks_printer.user_src);
  if(gCfgItems.language == LANG_SIMPLE_CHINESE)
    lv_obj_set_style(wifi_page.label_text, &User_style.text_black_style);
  else
    lv_obj_set_style(wifi_page.label_text, &wifi_style);
  lv_label_set_text(wifi_page.label_text, wifi_menu.info_text);
  lv_obj_align(wifi_page.label_text, nullptr, LV_ALIGN_IN_TOP_LEFT, 15, 82);

  wifi_page.imgbtn_back = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_back5.bin", LV_ALIGN_IN_TOP_LEFT, 184, 384, event_handler, ID_W_RETURN);

  // Create an Image button
  // lv_obj_t *buttonBack = lv_imgbtn_create(mks_printer.user_src, "F:/bmp_return.bin", BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_W_RETURN);
  // if (enc_ena) lv_group_add_obj(g, buttonBack);
  // lv_obj_t *label_Back = lv_label_create_empty(buttonBack);

  // if (gCfgItems.multiple_language) {
  //   if (gCfgItems.wifi_mode_sel == STA_MODEL) {
  //     if (gCfgItems.cloud_enable) {
  //       lv_label_set_text(label_Cloud, wifi_menu.cloud);
  //       lv_obj_align(label_Cloud, buttonCloud, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
  //     }
  //     lv_label_set_text(label_Reconnect, wifi_menu.reconnect);
  //     lv_obj_align(label_Reconnect, buttonReconnect, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
  //   }
  //   lv_label_set_text(label_Back, common_menu.text_back);
  //   lv_obj_align(label_Back, buttonBack, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
  // }

  wifi_page.label_ip = lv_label_create_empty(mks_printer.user_src);
  lv_obj_set_style(wifi_page.label_ip, &tft_style_label_rel);
  wifi_page.label_name = lv_label_create_empty(mks_printer.user_src);
  lv_obj_set_style(wifi_page.label_name, &tft_style_label_rel);
  wifi_page.label_number = lv_label_create_empty(mks_printer.user_src);
  lv_obj_set_style(wifi_page.label_number, &tft_style_label_rel);
  wifi_page.label_state = lv_label_create_empty(mks_printer.user_src);
  lv_obj_set_style(wifi_page.label_state, &tft_style_label_rel);

  disp_wifi_state();
}

void disp_wifi_state() {
  strcpy(public_buf_m, wifi_menu.ip);
  strcat(public_buf_m, ipPara.ip_addr);
  lv_label_set_text(wifi_page.label_ip, public_buf_m);
  lv_obj_align(wifi_page.label_ip, nullptr, LV_ALIGN_IN_TOP_LEFT, 15, 228);

  strcpy(public_buf_m, wifi_menu.wifi);
  strcat(public_buf_m, wifiPara.ap_name);
  lv_label_set_text(wifi_page.label_name, public_buf_m);
  lv_obj_align(wifi_page.label_name, nullptr, LV_ALIGN_IN_TOP_LEFT, 15, 260);

  if (wifiPara.mode == AP_MODEL) {
    strcpy(public_buf_m, wifi_menu.key);
    strcat(public_buf_m, wifiPara.keyCode);
    lv_label_set_text(wifi_page.label_number, public_buf_m);
    lv_obj_align(wifi_page.label_number, nullptr, LV_ALIGN_IN_TOP_LEFT, 15, 294);

    strcpy(public_buf_m, wifi_menu.state_ap);
    if (wifi_link_state == WIFI_CONNECTED)
      strcat(public_buf_m, wifi_menu.connected);
    else if (wifi_link_state == WIFI_NOT_CONFIG)
      strcat(public_buf_m, wifi_menu.disconnected);
    else
      strcat(public_buf_m, wifi_menu.exception);
    lv_label_set_text(wifi_page.label_state, public_buf_m);
    lv_obj_align(wifi_page.label_state, nullptr, LV_ALIGN_IN_TOP_LEFT, 15, 326);
  }
  else {
    strcpy(public_buf_m, wifi_menu.state_sta);
    if (wifi_link_state == WIFI_CONNECTED)
      strcat(public_buf_m, wifi_menu.connected);
    else if (wifi_link_state == WIFI_NOT_CONFIG)
      strcat(public_buf_m, wifi_menu.disconnected);
    else
      strcat(public_buf_m, wifi_menu.exception);
    lv_label_set_text(wifi_page.label_state, public_buf_m);
    lv_obj_align(wifi_page.label_state, nullptr, LV_ALIGN_CENTER, 15, 326);

    lv_label_set_text(wifi_page.label_number, "");
    lv_obj_align(wifi_page.label_number, nullptr, LV_ALIGN_IN_TOP_LEFT, 15, 294);
  }
}

void lv_clear_wifi() {
  // if (TERN0(HAS_ROTARY_ENCODER, gCfgItems.encoder_enable))
  //   lv_group_remove_all_objs(g);
  lv_obj_clean(mks_printer.user_src);
}

#endif // MKS_WIFI_MODULE
#endif // HAS_TFT_LVGL_UI
