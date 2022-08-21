
#include "../../../inc/MarlinConfigPre.h"

#if HAS_TFT_LVGL_UI

#include "draw_ui.h"
#include <lv_conf.h>

#include "../../../module/temperature.h"
#include "../../../inc/MarlinConfig.h"
#include "../../../gcode/queue.h"
#include "../../../module/planner.h"        // for synchronize

typedef struct{
  lv_obj_t *title_ok_imgbtn;
  lv_obj_t *title_ok_label;
  lv_obj_t *title_back_imgbtn;

  lv_style_t simple_num_kb_bg;
  lv_style_t simple_num_kb_pr;
  lv_style_t simple_num_kb_rel;

  lv_style_t simple_ta_bg;
  
  lv_obj_t *simple_kb;
  lv_obj_t *simple_ta;


}SIMPLE_NUM_KB_UI_PAGE;

SIMPLE_NUM_KB_UI_PAGE simple_num_kb_page;

uint8_t temp[10];
uint8_t switch_kb = 1;  //1~{D,HO<|EL~}  0~{J}WV<rRW<|EL~} 2gcode~{=!EL~}
CHANGE_NUM cur_chang_num;
extern uint32_t wifi_node;
extern bool gb_kb_input_layout_num_flag;
extern int32_t gb_kb_input_layout_num; 
enum{
    ID_S_BACK = 1, 
    ID_S_OK
};


static void event_handler(lv_obj_t* obj, lv_event_t event) 
{
 if((event == LV_EVENT_PRESSED))
  {
    button_sound_start();
  }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST) ){
        button_sound_end();
        if (event != LV_EVENT_RELEASED) return;
        switch (obj->mks_obj_id) {
            case ID_S_BACK: 
            if(cur_chang_num == SET_LAYER_STOP)
              gb_kb_input_layout_num_flag = false; 
              mks_clear_simple_num_kb(); 
              draw_return_ui(); 
              break;
        }
        switch_kb = 1 ;
    }
}


static void event_handler_ok(lv_obj_t* obj, lv_event_t event) 
{
    uint16_t temp_num;
  if(event == LV_EVENT_PRESSED)
    {
      button_sound_start();
    }
	if ((event == LV_EVENT_RELEASED)||(event == LV_EVENT_PRESS_LOST)) {
        button_sound_end();

        // temp_num = atof(lv_ta_get_text(simple_num_kb_page.simple_ta));
        
        if(cur_chang_num == GCODE)
        {
            sprintf_P(public_buf_l, PSTR("%s"), lv_ta_get_text(simple_num_kb_page.simple_ta));
            queue.inject(public_buf_l);
        }
        else if(cur_chang_num == EX1_TEMP)
        {
            temp_num = atoi(lv_ta_get_text(simple_num_kb_page.simple_ta));
            if(atoi(lv_ta_get_text(simple_num_kb_page.simple_ta)) < 0)
            {
              temp_num = 0;
            }
            if(temp_num > 275)
            {
              temp_num = 275;
            }
            thermalManager.setTargetHotend(temp_num, 0);
        }
        else if(cur_chang_num == BED_TEMP)
        {
            temp_num = atoi(lv_ta_get_text(simple_num_kb_page.simple_ta));
             if(atoi(lv_ta_get_text(simple_num_kb_page.simple_ta)) < 0)
            {
              temp_num = 0;
            }
            if(temp_num > 140)
            {
              temp_num = 140;
            }
            thermalManager.setTargetBed(temp_num);
        }
        else if(cur_chang_num == SET_PLA_TEMP)
        {
            temp_num = atoi(lv_ta_get_text(simple_num_kb_page.simple_ta));
            if(atoi(lv_ta_get_text(simple_num_kb_page.simple_ta)) < 0)
            {
              temp_num = 0;
            }
            if(temp_num > 275)
            {
              temp_num = 275;
            }
            temp_seting.pla_ex1 = temp_num;
            // thermalManager.setTargetHotend(temp_num, 0);
        }
        else if(cur_chang_num == SET_PLA_BED_TEMP)
        {
            temp_num = atoi(lv_ta_get_text(simple_num_kb_page.simple_ta));
             if(atoi(lv_ta_get_text(simple_num_kb_page.simple_ta)) < 0)
            {
              temp_num = 0;
            }
            if(temp_num > 140)
            {
              temp_num = 140;
            }
            temp_seting.pla_bed = temp_num;
            // thermalManager.setTargetHotend(temp_num, 0);
        }
        else if(cur_chang_num == SET_ABS_TEMP)
        {
            temp_num = atoi(lv_ta_get_text(simple_num_kb_page.simple_ta));
            if(atoi(lv_ta_get_text(simple_num_kb_page.simple_ta)) < 0)
            {
              temp_num = 0;
            }
            if(temp_num > 275)
            {
              temp_num = 275;
            }
            temp_seting.abs_ex1 = temp_num;
            // thermalManager.setTargetHotend(temp_num, 0);
        }
        else if(cur_chang_num == SET_ABS_BED_TEMP)
        {
            temp_num = atoi(lv_ta_get_text(simple_num_kb_page.simple_ta));
            if(atoi(lv_ta_get_text(simple_num_kb_page.simple_ta)) < 0)
            {
              temp_num = 0;
            }
            if(temp_num > 140)
            {
              temp_num = 140;
            }
            temp_seting.abs_bed = temp_num;
            // thermalManager.setTargetHotend(temp_num, 0);
        }
        else if(cur_chang_num == FAN_SPEED)
        {
            temp_num = atoi(lv_ta_get_text(simple_num_kb_page.simple_ta));
            if(atoi(lv_ta_get_text(simple_num_kb_page.simple_ta)) < 0)
            {
              temp_num = 0;
            }
            if(temp_num > 100)
            {
              temp_num = 100;
            }
            thermalManager.set_fan_speed(0, map(temp_num, 0, 100, 0, 255));
        }
        else if(cur_chang_num == PRINT_SPEED)
        {
            temp_num = atoi(lv_ta_get_text(simple_num_kb_page.simple_ta));
            if(atoi(lv_ta_get_text(simple_num_kb_page.simple_ta)) < 20)
            {
              temp_num = 20;
            }
            if(temp_num > 200)
            {
              temp_num = 200;
            }
            feedrate_percentage = temp_num;
        }
        else if(cur_chang_num == EX_SPEED)
        {
            temp_num = atoi(lv_ta_get_text(simple_num_kb_page.simple_ta));
            if(atoi(lv_ta_get_text(simple_num_kb_page.simple_ta)) < 100)
            {
              temp_num = 100;
            }
            if(temp_num > 900)
            {
              temp_num = 900;
            }
            planner.flow_percentage[uiCfg.extruderIndex] = temp_num;

        }
        if(cur_chang_num == SET_LAYER_STOP)
        {
          gb_kb_input_layout_num_flag = true;
           
          gb_kb_input_layout_num = atoi(lv_ta_get_text(simple_num_kb_page.simple_ta));
          if(gb_kb_input_layout_num < 0)
          {
            gb_kb_input_layout_num = 0;
          }
        }
        mks_clear_simple_num_kb();
        draw_return_ui();
        
	}
}
static void event_handler_simple_kb_event(lv_obj_t* obj, lv_event_t event) {

        if(event == LV_EVENT_CONNECT) {
        lv_kb_ext_t *ext = (lv_kb_ext_t*)lv_obj_get_ext_attr(obj);
        const char * ret_ta_txt = lv_ta_get_text(ext->ta);
      
          ZERO(uiCfg.wifi_name);
          memcpy((void *)uiCfg.wifi_name, wifi_list.wifiName[wifi_list.nameIndex], 32);

          ZERO(uiCfg.wifi_key);
          memcpy((void *)uiCfg.wifi_key, ret_ta_txt, sizeof(uiCfg.wifi_key));

          gCfgItems.wifi_mode_sel = STA_MODEL;

          package_to_wifi(WIFI_PARA_SET, nullptr, 0);

          public_buf_l[0] = 0xA5;
          public_buf_l[1] = 0x09;
          public_buf_l[2] = 0x01;
          public_buf_l[3] = 0x00;
          public_buf_l[4] = 0x01;
          public_buf_l[5] = 0xFC;
          public_buf_l[6] = 0x00;
          raw_send_to_wifi((uint8_t*)public_buf_l, 6);

          last_disp_state = KEYBOARD_UI;
          mks_clear_simple_num_kb();
          wifi_tips_type = TIPS_TYPE_JOINING;
          lv_draw_wifi_tips();
    }
    lv_kb_def_event_cb(obj, event);
}
// static void event_handler_wifi_connnect(lv_obj_t* obj, lv_event_t event) {

    
// 	if((event == LV_EVENT_RELEASED) || (event == LV_EVENT_PRESS_LOST)) {
    
//         memset(mks_wifi.wifi_name_connect, 0, sizeof(mks_wifi.wifi_name_connect));
//         memset(wifi_send_username, 0, sizeof(wifi_send_username));
//         memset(wifi_send_password, 0, sizeof(wifi_send_password));

//         strcpy(mks_wifi.wifi_name_connect, mks_wifi.wifi_name_str[mks_wifi.wifi_choose]);

//         strcat(username_cmd, mks_wifi.wifi_name_str[wifi_src.wifi_send_num-1]);
//         strcat(password_cmd, lv_ta_get_text(wifi_src.wifi_tb));

//         strcat(username_cmd,enter_cmd);
//         strcat(password_cmd,enter_cmd);

//         strcpy(wifi_send_username, username_cmd);
//         strcpy(wifi_send_password, password_cmd);

//         mks_lv_clean_ui();
//         mks_draw_wifi_connecting();

//         wifi_src.wifi_kb_flag = wifi_kb_send_wifi_username;
// 	}
// }


// void mks_dis_num()
// {
//     char buff[10];
   
//     switch (sys_rt_f_override)
//     {
//     case TYPE_SPEED:
//         sprintf(buff, "%d", sys_rt_f_override);
//         // dtostrf(sys_rt_f_override, 1, 1, buff);
//         // itoa(sys_rt_f_override,buff,10);
//         // lv_ta_add_text(simple_num_kb_page.simple_ta,buff);
//         break;
//     case TYPE_FAN:
//         sprintf(buff, "%d", fan_num_value);
//         // dtostrf(fan_num_value, 1, 1, buff);
//         // lv_ta_add_text(simple_num_kb_page.simple_ta,buff);
//         break;
//     case TYPE_PWR:
//         sprintf(buff, "%d", sys_rt_s_override);
//         // dtostrf(sys_rt_s_override, 1, 1, buff);
//         break;
//     case TYPE_STEP_PR:
//         // sprintf(buff, "%d", sys_rt_s_override);
//         break;
//     default:
//         break;
//     }
//     lv_ta_set_text(simple_num_kb_page.simple_ta,buff);
// }

void mks_draw_simple_num_kb(uint16_t option_ui,CHANGE_NUM chang)
{
  lv_screen_distribution(SIMPLE_KB_UI);
  cur_chang_num = chang;
  if(chang == WIFI_PASSWORD)
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src,(char const *)wifi_list.wifiName[wifi_node],TITLE_LABEL_X);
  else
  mks_printer.user_src1 = lv_title_offset_n_create(mks_printer.user_src);

  lv_refr_now(lv_refr_get_disp_refreshing());
  simple_num_kb_page.title_back_imgbtn = lv_imgbtn_create(mks_printer.user_src,"F:/png_back.bin",TITLE_BACK_X,TITLE_BACK_Y, event_handler, ID_S_BACK);

  // simple_num_kb_page.title_back_imgbtn = lv_imgbtn_align_create(mks_printer.user_src1, "F:/title_return.bin", LV_ALIGN_IN_LEFT_MID, 10, 0, event_handler, ID_S_BACK);
  simple_num_kb_page.title_ok_imgbtn = lv_btn_create(mks_printer.user_src1, event_handler_ok, 0, NULL);
  lv_obj_set_size(simple_num_kb_page.title_ok_imgbtn, 30, 20);
  lv_obj_align(simple_num_kb_page.title_ok_imgbtn, mks_printer.user_src1, LV_ALIGN_IN_RIGHT_MID, -20, 0);
  lv_btn_set_style_both(simple_num_kb_page.title_ok_imgbtn,&User_style.title_bg_style);
  simple_num_kb_page.title_ok_label = lv_label_create_empty(mks_printer.user_src1);
  lv_label_set_text(simple_num_kb_page.title_ok_label,"√OK");
  lv_label_set_style(simple_num_kb_page.title_ok_label,LV_LABEL_STYLE_MAIN,&User_style.my_text_white_style);
  lv_obj_align(simple_num_kb_page.title_ok_label, simple_num_kb_page.title_ok_imgbtn, LV_ALIGN_CENTER, 0, 0);
  if(option_ui == 0)
    {
        lv_style_copy(&simple_num_kb_page.simple_num_kb_rel, &lv_style_btn_rel);
        simple_num_kb_page.simple_num_kb_rel.body.radius = 0;
        simple_num_kb_page.simple_num_kb_rel.body.main_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
        simple_num_kb_page.simple_num_kb_rel.body.grad_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
        // simple_num_kb_page.simple_num_kb_rel.body.border.width = 3;
        simple_num_kb_page.simple_num_kb_rel.body.border.color = LV_COLOR_GRAY;
        simple_num_kb_page.simple_num_kb_rel.body.border.part = LV_BORDER_NONE;
        simple_num_kb_page.simple_num_kb_rel.text.color = LV_COLOR_BLACK;
        // simple_num_kb_page.simple_num_kb_rel.body.padding.top = 10;
        lv_style_copy(&simple_num_kb_page.simple_num_kb_pr, &lv_style_btn_pr);
        simple_num_kb_page.simple_num_kb_pr.body.radius = 0;
        // simple_num_kb_page.simple_num_kb_pr.body.border.width = 3;
        simple_num_kb_page.simple_num_kb_rel.body.border.color = LV_COLOR_GRAY;
        simple_num_kb_page.simple_num_kb_pr.body.main_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
        simple_num_kb_page.simple_num_kb_pr.body.grad_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
        simple_num_kb_page.simple_num_kb_pr.body.border.part = LV_BORDER_NONE;
        simple_num_kb_page.simple_num_kb_pr.text.color = LV_COLOR_BLACK;
        // simple_num_kb_page.simple_num_kb_pr.body.padding.top = 10;
    }
    else
    {
       lv_style_copy(&simple_num_kb_page.simple_num_kb_rel, &lv_style_btn_rel);
        simple_num_kb_page.simple_num_kb_rel.body.radius = 0;
        simple_num_kb_page.simple_num_kb_rel.body.main_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
        simple_num_kb_page.simple_num_kb_rel.body.grad_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
        simple_num_kb_page.simple_num_kb_rel.body.border.width = 0;
        // simple_num_kb_page.simple_num_kb_rel.body.border.color = LV_COLOR_GRAY;
        // simple_num_kb_page.simple_num_kb_rel.body.border.part = LV_BORDER_FULL;
        simple_num_kb_page.simple_num_kb_rel.text.color = LV_COLOR_BLACK;
        // simple_num_kb_page.simple_num_kb_rel.body.padding.top = 10;
        lv_style_copy(&simple_num_kb_page.simple_num_kb_pr, &lv_style_btn_pr);
        simple_num_kb_page.simple_num_kb_pr.body.radius = 0;
        simple_num_kb_page.simple_num_kb_pr.body.border.width = 0;
        // simple_num_kb_page.simple_num_kb_rel.body.border.color = LV_COLOR_GRAY;
        simple_num_kb_page.simple_num_kb_pr.body.main_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
        simple_num_kb_page.simple_num_kb_pr.body.grad_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
        // simple_num_kb_page.simple_num_kb_pr.body.border.part = LV_BORDER_FULL;
        simple_num_kb_page.simple_num_kb_pr.text.color = LV_COLOR_BLACK;
        // simple_num_kb_page.simple_num_kb_pr.body.padding.top = 10; 
    }

    lv_style_copy(&simple_num_kb_page.simple_num_kb_bg, &lv_style_pretty);
    simple_num_kb_page.simple_num_kb_bg.body.main_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
    simple_num_kb_page.simple_num_kb_bg.body.grad_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
    simple_num_kb_page.simple_num_kb_bg.body.border.width = 0;
    simple_num_kb_page.simple_num_kb_bg.body.radius = 0;

    if(option_ui == 0)
        switch_kb = 0;
    else 
        switch_kb = 2;


  simple_num_kb_page.simple_kb = lv_kb_create(mks_printer.user_src, NULL);
  lv_kb_set_cursor_manage(simple_num_kb_page.simple_kb, true);
  lv_kb_set_style(simple_num_kb_page.simple_kb, LV_KB_STYLE_BG, &simple_num_kb_page.simple_num_kb_bg);
  lv_kb_set_style(simple_num_kb_page.simple_kb, LV_KB_STYLE_BTN_REL, &simple_num_kb_page.simple_num_kb_rel);
  lv_kb_set_style(simple_num_kb_page.simple_kb, LV_KB_STYLE_BTN_PR, &simple_num_kb_page.simple_num_kb_pr);
  lv_obj_set_size(simple_num_kb_page.simple_kb,308,334);
  lv_obj_set_pos(simple_num_kb_page.simple_kb,6,123);
  lv_obj_set_event_cb(simple_num_kb_page.simple_kb, event_handler_simple_kb_event);

  
  lv_style_copy(&simple_num_kb_page.simple_ta_bg, &lv_style_pretty);
  simple_num_kb_page.simple_ta_bg.body.main_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
  simple_num_kb_page.simple_ta_bg.body.grad_color = LV_COLOR_WHITE;//lv_color_make(58, 98, 151);
  simple_num_kb_page.simple_ta_bg.body.border.width = 0;
  simple_num_kb_page.simple_ta_bg.body.radius = 0;
  simple_num_kb_page.simple_ta_bg.text.color = LV_COLOR_BLACK;

  simple_num_kb_page.simple_ta = lv_ta_create(mks_printer.user_src, NULL); 
  lv_ta_set_style(simple_num_kb_page.simple_ta,LV_TA_STYLE_BG,&simple_num_kb_page.simple_ta_bg);
  lv_obj_set_pos(simple_num_kb_page.simple_ta, 0, 51);
  lv_obj_set_size(simple_num_kb_page.simple_ta, 320, 70);
  lv_ta_set_text(simple_num_kb_page.simple_ta, "");
  lv_kb_set_ta(simple_num_kb_page.simple_kb, simple_num_kb_page.simple_ta);
//   mks_printer.user_src = lv_scr_create(mks_printer.user_src,0,TITLE_H+20,TITLE_W,TFT_HEIGHT-TITLE_H-20);
 
  // lv_imgbtn_set_src_both(simple_num_kb_page.title_back_imgbtn,"F:/title_return.bin");
  // lv_imgbtn_set_src_both(simple_num_kb_page.title_ok_imgbtn,"F:/title_ok.bin");


//   lv_kb_set_style(simple_num_kb_page.simple_kb, LV_KB_STYLE_BG, & simple_num_kb_page.simple_num_kb_bg);
//   lv_kb_set_style(simple_num_kb_page.simple_kb, LV_KB_STYLE_BTN_REL, & simple_num_kb_page.simple_num_kb_rel);
//   lv_kb_set_style(simple_num_kb_page.simple_kb, LV_KB_STYLE_BTN_PR, &simple_num_kb_page.simple_num_kb_pre);


}

void mks_clear_simple_num_kb(void)
{
    lv_obj_clean(mks_printer.user_src);
}
#endif