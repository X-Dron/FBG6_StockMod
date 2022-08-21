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

#include "../../../inc/MarlinConfig.h"
#include <string.h>

#define language_btn_w                   156
#define language_btn_h                   74
#define language_first_y_offset          57
#define language_y_offset                4
#define language_first_x_offset          2
#define language_x_offset                4


typedef struct{
  
  lv_obj_t *lg_1_imgbtn;
  lv_obj_t *lg_2_imgbtn;
  lv_obj_t *lg_3_imgbtn;
  lv_obj_t *lg_4_imgbtn;
  lv_obj_t *lg_5_imgbtn;
  lv_obj_t *lg_6_imgbtn;
  lv_obj_t *lg_7_imgbtn;
  lv_obj_t *lg_8_imgbtn;

  lv_obj_t *up_imgbtn;
  lv_obj_t *png_next_imgbtn;

  lv_obj_t *png_back2_imgbtn;

}LANGUAGE_UI_PAGE;

LANGUAGE_UI_PAGE language_page;

enum {
  ID_CN = 1,
  ID_T_CN,
  ID_EN,
  ID_GE,
  ID_PE,
  ID_PO,
  ID_FR,
  ID_SP,
  ID_KO,
  ID_RU,
  // ID_IT,
  ID_L_RETURN
};

#define SELECTED    1
#define UNSELECTED  0

void disp_langurag(uint16_t num);

extern lv_group_t *g;
static lv_obj_t *scr;
static lv_obj_t *buttonCN, *buttonT_CN, *buttonEN, *buttonRU;
static lv_obj_t *labelCN, *labelT_CN, *labelEN, *labelRU;
static lv_obj_t *buttonES, *buttonFR, *buttonIT;
static lv_obj_t *imgset;

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
    button_sound_end();
    if (event != LV_EVENT_RELEASED) return;
    switch (obj->mks_obj_id) {
      case ID_CN:
        // disp_langurag(obj->mks_obj_id);
        // disp_language(gCfgItems.language, UNSELECTED);
        // lv_imgbtn_set_src_both(buttonCN, "F:/png_chinese_on.bin");
        // lv_obj_refresh_ext_draw_pad(buttonCN);
        gCfgItems.language = LANG_SIMPLE_CHINESE;
        disp_imgset_refre(gCfgItems.language);
        update_spi_flash();
        disp_language_init();
        break;
      case ID_EN:
        // disp_langurag(obj->mks_obj_id);

        // disp_language(gCfgItems.language, UNSELECTED);
        // lv_imgbtn_set_src_both(buttonEN, "F:/png_englist_on.bin");
        // lv_obj_refresh_ext_draw_pad(buttonEN);
        gCfgItems.language = LANG_ENGLISH;
        disp_imgset_refre(gCfgItems.language);
        update_spi_flash();
        disp_language_init();
        break;
      case ID_GE:
        disp_langurag(obj->mks_obj_id);

        // disp_language(gCfgItems.language, UNSELECTED);
        // lv_imgbtn_set_src_both(buttonRU, "F:/bmp_russian_sel.bin");
        // lv_obj_refresh_ext_draw_pad(buttonRU);
        gCfgItems.language = LANG_GERMAN;
        update_spi_flash();
        disp_language_init();
        break;
      case ID_PE:
        disp_langurag(obj->mks_obj_id);

        // disp_language(gCfgItems.language, UNSELECTED);
        // lv_imgbtn_set_src_both(buttonES, "F:/bmp_spanish_sel.bin");
        // lv_obj_refresh_ext_draw_pad(buttonES);
        gCfgItems.language = LANG_PORTUGUESE;
        update_spi_flash();
        disp_language_init();
        break;
      case ID_PO:
        disp_langurag(obj->mks_obj_id);

        // disp_language(gCfgItems.language, UNSELECTED);
        // lv_imgbtn_set_src_both(buttonES, "F:/bmp_spanish_sel.bin");
        // lv_obj_refresh_ext_draw_pad(buttonES);
        // gCfgItems.language = LANG_POLISH;
        update_spi_flash();
        disp_language_init();
        break;
      case ID_FR:
        disp_langurag(obj->mks_obj_id);

        // disp_language(gCfgItems.language, UNSELECTED);
        // lv_imgbtn_set_src_both(buttonFR, "F:/bmp_french_sel.bin");
        // lv_obj_refresh_ext_draw_pad(buttonFR);
        gCfgItems.language = LANG_FRENCH;
        update_spi_flash();
        disp_language_init();
        break;
      case ID_SP:
        disp_langurag(obj->mks_obj_id);

        // disp_language(gCfgItems.language, UNSELECTED);
        // lv_imgbtn_set_src_both(buttonES, "F:/bmp_spanish_sel.bin");
        // lv_obj_refresh_ext_draw_pad(buttonES);
        gCfgItems.language = LANG_SPANISH;
        update_spi_flash();
        disp_language_init();
        break;
      case ID_KO:
        disp_langurag(obj->mks_obj_id);

        // disp_language(gCfgItems.language, UNSELECTED);
        // lv_imgbtn_set_src_both(buttonES, "F:/bmp_spanish_sel.bin");
        // lv_obj_refresh_ext_draw_pad(buttonES);
        gCfgItems.language = LANG_KOREAN;
        update_spi_flash();
        disp_language_init();
        break;
      case ID_RU:
        gCfgItems.language = LANG_RUSSIAN;
        disp_imgset_refre(gCfgItems.language);
        update_spi_flash();
        disp_language_init();
        break;
      
      case ID_L_RETURN:
        // buttonCN   = nullptr;
        // buttonT_CN = nullptr;
        // buttonEN   = nullptr;
        // buttonRU   = nullptr;
        // buttonES   = nullptr;
        // buttonFR   = nullptr;
        // buttonFR   = nullptr;
        // buttonIT   = nullptr;
        lv_clear_language();
        // lv_draw_set();
        lv_draw_tool();
        break;
    }
  }
}
void disp_langurag(uint16_t num)
{
  switch (num) {
      case ID_CN:
       lv_imgbtn_set_src_both(language_page.lg_1_imgbtn,"F:/png_englist_off.bin");
       lv_imgbtn_set_src_both(language_page.lg_2_imgbtn,"F:/png_chinese_on.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_3_imgbtn,"F:/german.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_4_imgbtn,"F:/Portuguese.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_5_imgbtn,"F:/polish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_6_imgbtn,"F:/french.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_7_imgbtn,"F:/Spanish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_8_imgbtn,"F:/korean.bin");
        break;
      case ID_EN:
       lv_imgbtn_set_src_both(language_page.lg_1_imgbtn,"F:/png_englist_on.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_2_imgbtn,"F:/png_chinese_off.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_3_imgbtn,"F:/german.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_4_imgbtn,"F:/Portuguese.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_5_imgbtn,"F:/polish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_6_imgbtn,"F:/french.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_7_imgbtn,"F:/Spanish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_8_imgbtn,"F:/korean.bin");
        break;
      case ID_GE:
       lv_imgbtn_set_src_both(language_page.lg_1_imgbtn,"F:/png_englist_off.bin");
       lv_imgbtn_set_src_both(language_page.lg_2_imgbtn,"F:/png_chinese_off.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_3_imgbtn,"F:/german_on.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_4_imgbtn,"F:/Portuguese.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_5_imgbtn,"F:/polish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_6_imgbtn,"F:/french.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_7_imgbtn,"F:/Spanish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_8_imgbtn,"F:/korean.bin");
        break;
      case ID_PE:
       lv_imgbtn_set_src_both(language_page.lg_1_imgbtn,"F:/png_englist_off.bin");
       lv_imgbtn_set_src_both(language_page.lg_2_imgbtn,"F:/png_chinese_off.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_3_imgbtn,"F:/german.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_4_imgbtn,"F:/Portuguese_on.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_5_imgbtn,"F:/polish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_6_imgbtn,"F:/french.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_7_imgbtn,"F:/Spanish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_8_imgbtn,"F:/korean.bin");
        break;
      case ID_PO:
       lv_imgbtn_set_src_both(language_page.lg_1_imgbtn,"F:/png_englist_off.bin");
       lv_imgbtn_set_src_both(language_page.lg_2_imgbtn,"F:/png_chinese_off.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_3_imgbtn,"F:/german.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_4_imgbtn,"F:/Portuguese.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_5_imgbtn,"F:/polish_on.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_6_imgbtn,"F:/french.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_7_imgbtn,"F:/Spanish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_8_imgbtn,"F:/korean.bin");
        break;
      case ID_FR:
       lv_imgbtn_set_src_both(language_page.lg_1_imgbtn,"F:/png_englist_off.bin");
       lv_imgbtn_set_src_both(language_page.lg_2_imgbtn,"F:/png_chinese_off.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_3_imgbtn,"F:/german.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_4_imgbtn,"F:/Portuguese.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_5_imgbtn,"F:/polish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_6_imgbtn,"F:/french_on.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_7_imgbtn,"F:/Spanish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_8_imgbtn,"F:/korean.bin");
        break;
      case ID_SP:
       lv_imgbtn_set_src_both(language_page.lg_1_imgbtn,"F:/png_englist_off.bin");
       lv_imgbtn_set_src_both(language_page.lg_2_imgbtn,"F:/png_chinese_off.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_3_imgbtn,"F:/german.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_4_imgbtn,"F:/Portuguese.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_5_imgbtn,"F:/polish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_6_imgbtn,"F:/french.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_7_imgbtn,"F:/Spanish_on.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_8_imgbtn,"F:/korean.bin");
        break;
      case ID_KO:
       lv_imgbtn_set_src_both(language_page.lg_1_imgbtn,"F:/png_englist_off.bin");
       lv_imgbtn_set_src_both(language_page.lg_2_imgbtn,"F:/png_chinese_off.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_3_imgbtn,"F:/german.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_4_imgbtn,"F:/Portuguese.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_5_imgbtn,"F:/polish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_6_imgbtn,"F:/french.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_7_imgbtn,"F:/Spanish.bin");
      //  lv_imgbtn_set_src_both(language_page.lg_8_imgbtn,"F:/korean_on.bin");
        break;
    }
}
// #define LANG_SIMPLE_CHINESE    1
// #define LANG_COMPLEX_CHINESE   2
// #define LANG_ENGLISH           3
// #define LANG_JAPAN             4
// #define LANG_GERMAN            5
// #define LANG_FRENCH            6
// #define LANG_RUSSIAN           7
// #define LANG_KOREAN            8
// #define LANG_TURKISH           9
// #define LANG_SPANISH          10
// #define LANG_GREEK            11
// #define LANG_ITALY            12
// #define LANG_PORTUGUESE       13
// static void disp_language(uint8_t language, uint8_t state) {
//   uint16_t id;
//   lv_obj_t *obj;

//   public_buf_l[0] = '\0';

//   switch (language) {
//     case LANG_SIMPLE_CHINESE:
//       id = ID_CN;
//       strcpy_P(public_buf_l, PSTR("F:/png_chinese_off"));
//       obj = buttonCN;
//       break;
//     case LANG_COMPLEX_CHINESE:
//       id = ID_T_CN;
//       strcpy_P(public_buf_l, PSTR("F:/bmp_traditional_cn"));
//       obj = buttonT_CN;
//       break;
//     case LANG_ENGLISH:
//       id = ID_EN;
//       strcpy_P(public_buf_l, PSTR("F:/png_englist_off"));
//       obj = buttonEN;
//       break;
//     case LANG_GERMAN:
//       id = ID_GE;
//       strcpy_P(public_buf_l, PSTR("F:/bmp_russian"));
//       obj = buttonRU;
//       break;
//     case LANG_PORTUGUESE:
//       id = ID_PE;
//       strcpy_P(public_buf_l, PSTR("F:/bmp_spanish"));
//       obj = buttonES;
//       break;
//     case LANG_RUSSIAN:
//       id = ID_PO;
//       strcpy_P(public_buf_l, PSTR("F:/bmp_french"));
//       obj = buttonFR;
//       break;
//     case LANG_FRENCH:
//       id = ID_FR;
//       strcpy_P(public_buf_l, PSTR("F:/bmp_italy"));
//       obj = buttonIT;
//       break;
//      case LANG_SPANISH:
//       id = ID_SP;
//       strcpy_P(public_buf_l, PSTR("F:/bmp_italy"));
//       obj = buttonIT;
//       break;
//      case LANG_KOREAN:
//       id = ID_KO;
//       strcpy_P(public_buf_l, PSTR("F:/bmp_italy"));
//       obj = buttonIT;
//       break;
//     default:
//       id = ID_CN;
//       strcpy_P(public_buf_l, PSTR("F:/png_chinese_off"));
//       obj = buttonCN;
//       break;
//   }

//   if (state == SELECTED) strcat_P(public_buf_l, PSTR("_sel"));

//   strcat_P(public_buf_l, PSTR(".bin"));

//   lv_obj_set_event_cb_mks(obj, event_handler, id, "", 0);
//   lv_imgbtn_set_src_both(obj, public_buf_l);

//   if (state == UNSELECTED) lv_obj_refresh_ext_draw_pad(obj);
// }

void lv_draw_language() {
  
  static lv_style_t btn_red, btn_blue, btn_yellow, btn_green;

  lv_style_copy(&btn_red, &style_para_value);
  btn_red.body.main_color   = LV_COLOR_MAKE(0xd9, 0x4d, 0x40);//LV_COLOR_RED
  btn_red.body.grad_color   = LV_COLOR_MAKE(0xd9, 0x4d, 0x40);
  btn_red.text.color = LV_COLOR_WHITE;
  btn_red.text.font = &DengXian_16;

  lv_style_copy(&btn_blue, &style_para_value);
  btn_blue.body.main_color   = LV_COLOR_MAKE(0x40, 0x84, 0xf0);//LV_COLOR_BLUE
  btn_blue.body.grad_color   = LV_COLOR_MAKE(0x40, 0x84, 0xf0);
  btn_blue.text.color = LV_COLOR_WHITE;
  btn_blue.text.font = &DengXian_16;

  lv_style_copy(&btn_yellow, &style_para_value);
  btn_yellow.body.main_color   = LV_COLOR_MAKE(0xf8, 0xa0, 0x10);//LV_COLOR_ORANGE
  btn_yellow.body.grad_color   = LV_COLOR_MAKE(0xf8, 0xa0, 0x10);
  btn_yellow.text.color = LV_COLOR_WHITE;
  btn_yellow.text.font = &DengXian_16;

  lv_style_copy(&btn_green, &style_para_value);
  btn_green.body.main_color   = LV_COLOR_MAKE(0x18, 0xa0, 0x60);//LV_COLOR_GREEN
  btn_green.body.grad_color   = LV_COLOR_MAKE(0x18, 0xa0, 0x60);
  btn_green.text.color = LV_COLOR_WHITE;
  btn_green.text.font = &DengXian_16;

  // lv_screen_distribution(LANGUAGE_UI);
  // mks_printer.user_src1 = lv_title_create(mks_printer.user_src,get_title_text(LANGUAGE_UI));
    lv_screen_distribution(LANGUAGE_UI); 
  
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,get_title_text(LANGUAGE_UI),10);
  lv_refr_now(lv_refr_get_disp_refreshing());

#if 0
  // mks_printer.user_src2 = lv_scr_create(mks_printer.user_src,0,TITLE_H,TITLE_W,TFT_HEIGHT-TITLE_H);
  // if(gCfgItems.language == LANG_SIMPLE_CHINESE)
  // {
  //   language_page.lg_1_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_englist_off.bin", LV_ALIGN_IN_TOP_LEFT, 0, language_first_y_offset, event_handler, ID_EN);
  //   language_page.lg_2_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_chinese_on.bin", LV_ALIGN_IN_TOP_LEFT, language_first_x_offset + language_x_offset + language_btn_w, language_first_y_offset,event_handler, ID_CN); 
  // }
  // else
  // {
    language_page.lg_1_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_englist_on.bin", LV_ALIGN_IN_TOP_LEFT, 0, language_first_y_offset, event_handler, ID_EN);
    // language_page.lg_2_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_chinese_off.bin", LV_ALIGN_IN_TOP_LEFT, language_first_x_offset + language_x_offset + language_btn_w, language_first_y_offset,event_handler, ID_CN); 

  // }
  // language_page.lg_5_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/polish.bin", LV_ALIGN_IN_TOP_LEFT,0, language_first_y_offset + language_y_offset + language_btn_h,  event_handler, ID_PO);
  // language_page.lg_3_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/german.bin", LV_ALIGN_IN_TOP_LEFT, 0, language_first_y_offset + language_y_offset*2 + language_btn_h*2, event_handler, ID_GE);
  // language_page.lg_4_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/Portuguese.bin", LV_ALIGN_IN_TOP_LEFT, 0, language_first_y_offset + language_y_offset*3 + language_btn_h*3, event_handler, ID_PE);
  
  // language_page.lg_6_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/french.bin", LV_ALIGN_IN_TOP_LEFT, language_x_offset + language_btn_w, language_first_y_offset + language_y_offset + language_btn_h, event_handler, ID_FR);
  // language_page.lg_7_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/Spanish.bin", LV_ALIGN_IN_TOP_LEFT, language_x_offset + language_btn_w, language_first_y_offset + language_y_offset*2 + language_btn_h*2, event_handler, ID_SP);
  // language_page.lg_8_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/korean.bin", LV_ALIGN_IN_TOP_LEFT, language_x_offset + language_btn_w, language_first_y_offset + language_y_offset*3 + language_btn_h*3, event_handler, ID_KO);
#else
  buttonEN = lv_label_btn_create(mks_printer.user_src, 2, 57, 156, 74, event_handler, ID_EN);
  lv_btn_set_style(buttonEN, LV_BTN_STYLE_REL, &btn_red);
  lv_btn_set_style(buttonEN, LV_BTN_STYLE_PR,  &btn_red);
  labelEN = lv_label_create_empty(mks_printer.user_src);
  lv_obj_set_style(labelEN, &btn_red);
  lv_label_set_text(labelEN, language_menu.english);
    lv_label_set_style(labelEN,LV_LABEL_STYLE_MAIN,&User_style.my_text_black_style);
  lv_obj_align(labelEN, buttonEN, LV_ALIGN_IN_TOP_MID, 0, 0);

  buttonCN = lv_label_btn_create(mks_printer.user_src, 162, 57, 156, 74, event_handler, ID_CN);
  lv_btn_set_style(buttonCN, LV_BTN_STYLE_REL, &btn_blue);
  lv_btn_set_style(buttonCN, LV_BTN_STYLE_PR,  &btn_blue);
  labelCN = lv_label_create_empty(mks_printer.user_src);
  lv_obj_set_style(labelCN, &btn_red);
    lv_label_set_style(labelCN,LV_LABEL_STYLE_MAIN,&User_style.my_text_black_style);
  lv_label_set_text(labelCN, language_menu.chinese_s);
  lv_obj_align(labelCN, buttonCN, LV_ALIGN_IN_TOP_MID, 0, 0);
  
  buttonRU = lv_label_btn_create(mks_printer.user_src, 2, 135, 156, 74, event_handler, ID_RU);
  lv_btn_set_style(buttonRU, LV_BTN_STYLE_REL, &btn_green);
  lv_btn_set_style(buttonRU, LV_BTN_STYLE_PR,  &btn_green);
  labelRU = lv_label_create_empty(mks_printer.user_src);
  lv_obj_set_style(labelRU, &btn_red);
    lv_label_set_style(labelRU,LV_LABEL_STYLE_MAIN,&User_style.my_text_black_style);
  lv_label_set_text(labelRU, language_menu.russian);
  lv_obj_align(labelRU, buttonRU, LV_ALIGN_IN_TOP_MID, 0, 0);
#endif
  language_page.up_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_pre.bin", LV_ALIGN_IN_BOTTOM_LEFT, 10, -10, event_handler, ID_L_RETURN);
  language_page.png_next_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_next.bin", LV_ALIGN_IN_BOTTOM_MID, 0, -10, event_handler, ID_L_RETURN);
  language_page.png_back2_imgbtn = lv_imgbtn_align_create(mks_printer.user_src, "F:/png_back2.bin", LV_ALIGN_IN_BOTTOM_RIGHT, -10, -10, event_handler, ID_L_RETURN);
  lv_refr_now(lv_refr_get_disp_refreshing());

  imgset = lv_img_create(mks_printer.user_src,nullptr);
  disp_imgset_refre(gCfgItems.language);
  // language_menu
  // scr = lv_screen_create(LANGUAGE_UI);
  // // Create image buttons
  // buttonCN = lv_big_button_create(scr, "F:/png_chinese_off.bin", language_menu.png_chinese_off_s, INTERVAL_V, titleHeight, event_handler, ID_CN);
  // lv_obj_clear_protect(buttonCN, LV_PROTECT_FOLLOW);
  // buttonT_CN = lv_big_button_create(scr, "F:/bmp_traditional_cn.bin", language_menu.png_chinese_off_t, BTN_X_PIXEL + INTERVAL_V * 2, titleHeight, event_handler, ID_T_CN);
  // buttonEN = lv_big_button_create(scr, "F:/png_englist_off.bin", language_menu.png_englist_off, BTN_X_PIXEL * 2 + INTERVAL_V * 3, titleHeight, event_handler, ID_EN);
  // buttonRU = lv_big_button_create(scr, "F:/bmp_russian.bin", language_menu.russian, BTN_X_PIXEL * 3 + INTERVAL_V * 4, titleHeight, event_handler, ID_RU);
  // buttonES = lv_big_button_create(scr, "F:/bmp_spanish.bin", language_menu.spanish, INTERVAL_V, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_ES);
  // buttonFR = lv_big_button_create(scr, "F:/bmp_french.bin", language_menu.french, BTN_X_PIXEL + INTERVAL_V * 2, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_FR);
  // buttonIT = lv_big_button_create(scr, "F:/bmp_italy.bin", language_menu.italy, BTN_X_PIXEL * 2 + INTERVAL_V * 3, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_IT);
  // lv_big_button_create(scr, "F:/bmp_return.bin", common_menu.text_png_back2, BTN_X_PIXEL * 3 + INTERVAL_V * 4, BTN_Y_PIXEL + INTERVAL_H + titleHeight, event_handler, ID_L_RETURN);
  // disp_language(gCfgItems.language, SELECTED);
}

void lv_clear_language() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  // lv_obj_del(scr);
  lv_obj_clean(mks_printer.user_src);
}

void disp_imgset_refre(uint8_t language)
{
  switch(language)
  {
    case LANG_SIMPLE_CHINESE:
      lv_img_set_src(imgset, "F:/bmp_language_set_b.bin");
      lv_obj_align(imgset,buttonCN, LV_ALIGN_IN_BOTTOM_MID, 0, -2);
      break;
    case LANG_ENGLISH:
      lv_img_set_src(imgset, "F:/bmp_language_set_r.bin");
      lv_obj_align(imgset,buttonEN, LV_ALIGN_IN_BOTTOM_MID, 0, -2);
      break;
    case LANG_RUSSIAN:
      lv_img_set_src(imgset, "F:/bmp_language_set_g.bin");
      lv_obj_align(imgset,buttonRU, LV_ALIGN_IN_BOTTOM_MID, 0, -2);
      break;
  }
}

#endif // HAS_TFT_LVGL_UI
