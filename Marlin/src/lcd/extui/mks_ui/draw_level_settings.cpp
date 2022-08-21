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

extern lv_group_t *g;
static lv_obj_t *scr;

enum {
  ID_LEVEL_RETURN = 1,
  ID_LEVEL_POSITION,
  ID_LEVEL_COMMAND,
  ID_LEVEL_ZOFFSET
};

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_RELEASED) return;
  lv_clear_level_settings();
  switch (obj->mks_obj_id) {
    case ID_LEVEL_RETURN:
      draw_return_ui();
      break;
    case ID_LEVEL_POSITION:
      lv_draw_tramming_pos_settings();
      break;
    case ID_LEVEL_COMMAND:
      keyboard_value = autoLevelGcodeCommand;
      lv_draw_keyboard();
      break;
    #if HAS_BED_PROBE
      case ID_LEVEL_ZOFFSET:
        lv_draw_auto_level_offset_settings();
        break;
    #endif
  }
}

void lv_draw_level_settings() {
  scr = lv_screen_create(LEVELING_PARA_UI, machine_menu.LevelingParaConfTitle);
  lv_screen_menu_item(scr, machine_menu.TrammingPosConf, PARA_UI_POS_X, PARA_UI_POS_Y, event_handler, ID_LEVEL_POSITION, 0);
  lv_screen_menu_item(scr, machine_menu.LevelingAutoCommandConf, PARA_UI_POS_X, PARA_UI_POS_Y * 2, event_handler, ID_LEVEL_COMMAND, 1);
  #if HAS_BED_PROBE
    lv_screen_menu_item(scr, machine_menu.LevelingAutoZoffsetConf, PARA_UI_POS_X, PARA_UI_POS_Y * 3, event_handler, ID_LEVEL_ZOFFSET, 2);
  #endif
  lv_big_button_create(scr, "F:/bmp_back70x40.bin", common_menu.text_back, PARA_UI_BACL_POS_X + 10, PARA_UI_BACL_POS_Y, event_handler, ID_LEVEL_RETURN, true);
}//bmp_back70x40

void lv_clear_level_settings() {
  #if HAS_ROTARY_RUCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

#endif // HAS_TFT_LVGL_UI


void ttt()
{
    tips_menu.joining      = TEXT_WIFI_JOINING_EN;
    tips_menu.failedJoin   = TEXT_WIFI_FAILED_JOIN_EN;
    tips_menu.wifiConected = TEXT_WIFI_WIFI_CONECTED_EN;

    common_menu.dialog_confirm_title  = TITLE_DIALOG_CONFIRM_RU;
    common_menu.text_back             = BACK_TEXT_RU;
    common_menu.close_machine_tips    = DIALOG_CLOSE_MACHINE_RU;
    common_menu.unbind_printer_tips   = DIALOG_UNBIND_PRINTER_RU;
    common_menu.print_special_title   = PRINTING_OTHER_LANGUGE;
    common_menu.pause_special_title   = PRINTING_PAUSE_OTHER_LANGUGE;
    common_menu.operate_special_title = PRINTING_OPERATION_OTHER_LANGUGE;
    //
    main_menu.title        = TITLE_READYPRINT_RU;
    main_menu.preheat      = PREHEAT_TEXT_RU;
    main_menu.move         = MOVE_TEXT_RU;
    main_menu.home         = HOME_TEXT_RU;
    main_menu.print        = PRINT_TEXT_RU;
    main_menu.extrude      = EXTRUDE_TEXT_RU;
    main_menu.leveling     = LEVELING_TEXT_RU;
    main_menu.autoleveling = AUTO_LEVELING_TEXT_RU;
    main_menu.fan          = FAN_TEXT_RU;
    main_menu.set          = SET_TEXT_RU;
    main_menu.more         = MORE_TEXT_RU;
    main_menu.tool         = TOOL_TEXT_RU;
    main_menu.filament     = FILAMENT_TEXT_RU;
    main_menu.emergency     = EMERGENCY_TEXT_EN;
    // TOOL
    tool_menu.title        = TOOL_TEXT_RU;
    tool_menu.preheat      = TOOL_PREHEAT_RU;
    tool_menu.extrude      = TOOL_EXTRUDE_RU;
    tool_menu.move         = TOOL_MOVE_RU;
    tool_menu.home         = TOOL_HOME_RU;

    layout_menu.title      = LAYOUT_TEXT_EN;

    // tool_menu.leveling     = TOOL_LEVELING_RU;
    tool_menu.autoleveling = TOOL_AUTO_LEVELING_RU;
    tool_menu.filament     = TOOL_FILAMENT_RU;
    tool_menu.more         = TOOL_MORE_RU;
    tool_menu.leveling     = TOOL_LEVELING_RU;
    tool_menu.tempsetting  = TOOL_TEMPSETING_EN;
    tool_menu.wifi         = TOOL_WIFI_EN;
    tool_menu.advances     = TOOL_ADVANCES_EN;

    tool_menu.endstop      = TOOL_ENDSTOP_EN;
    tool_menu.voice        = TOOL_VOICE_EN;
    tool_menu.language     = TOOL_LANGUAGE_EN;
    tool_menu.about        = TOOL_ABOUT_EN;
    //
    preheat_menu.adjust_title = TITLE_ADJUST_RU;
    preheat_menu.title        = TITLE_PREHEAT_RU;
    preheat_menu.add          = ADD_TEXT_RU;
    preheat_menu.dec          = DEC_TEXT_RU;
    preheat_menu.ext1         = EXTRUDER_1_TEXT_RU;
    preheat_menu.ext2         = EXTRUDER_2_TEXT_RU;
    preheat_menu.hotbed       = HEATBED_TEXT_RU;
    preheat_menu.off          = CLOSE_TEXT_RU;

    endstop_menu.title = ENDSTOP_TITLE_EN;
    endstop_menu.open = ENDSTOP_OPEN_EN;
    endstop_menu.trigger = ENDSTOP_TRIGGER_EN;
    endstop_menu.x_endstop = ENDSTOP_X_EN;
    endstop_menu.y_endstop = ENDSTOP_Y_EN;
    endstop_menu.z_endstop = ENDSTOP_Z_EN;
    endstop_menu.det_break = ENDSTOP_DET_BREAK_EN;
    endstop_menu.flame_sensor = ENDSTOP_FLAME_EN;

    dialog_menu.recovery_text = DIALOG_TEXT_EN;
    dialog_menu.complete_btn = DIALOG_COMPLETE_BTN_EN;
    dialog_menu.print_again = DIALOG_PRINT_AGAIN_BTN_EN;
    dialog_menu.recovery_continue_btn = DIALONG_RECOVERY_COMTINUE_BTN_EN;
    dialog_menu.recovery_back_btn = DIALONG_RECOVERY_BACK_BTN_EN;
    dialog_menu.add_filament = ADD_FILAMENT_RU;


    advances_menu.title = ADVANCES_TITLE_EN;
    advances_menu.floodlight = ADVANCES_FLOODLIGHT_EN;
    advances_menu.fan5051 = ADVANCES_5051FAN_EN;
    advances_menu.flame_alarm = ADVANCES_FLAME_ALARM_EN;

    voice_menu.title = VOICE_TITLE_EN;
    voice_menu.btn_sound = VOICE_BTN_SOUND_EN;
    voice_menu.endstop_sound = VOICE_ENDSTOP_SOUND_EN;
    voice_menu.print_tone_sound = VOICE_PRINT_TONE_SOUND_EN;
    voice_menu.no_filament_tone_sound = VOICE_NO_FILAMENT_SOUND_EN;
      

    tempsetting_menu.title = TEMPSETTING_TITLE_EN;
    tempsetting_menu.pla = TEMPSETTING_PLA_EN;
    tempsetting_menu.abs = TEMPSETTING_ABS_EN;
    tempsetting_menu.ex1 = TEMPSETTING_EX1_EN;
    tempsetting_menu.bed = TEMPSETTING_BED_EN;

    //
    move_menu.title    = TITLE_MOVE_RU;
    move_menu.zoffset = TEXT_ZOFFSET_RU;
    move_menu.layout_stop         = TEXT_LAYER_STOP_EN;
    //
    home_menu.title    = TITLE_HOME_RU;
    home_menu.stopmove = HOME_STOPMOVE_RU;
    //
    file_menu.title             = TITLE_CHOOSEFILE_RU;
    file_menu.page_up           = PAGE_UP_TEXT_RU;
    file_menu.page_down         = PAGE_DOWN_TEXT_RU;
    file_menu.file_loading      = FILE_LOADING_RU;
    file_menu.no_file           = NO_FILE_RU;
    file_menu.no_file_and_check = NO_FILE_RU;
    //
    // endstop_menu.title = ENDSTOP_TITLE_RU;
    // endstop_menu.x_endstop = ENDSTOP_X_RU;
    // endstop_menu.y_endstop = ENDSTOP_Y_RU;
    // endstop_menu.z_endstop = ENDSTOP_Z_RU;
    // endstop_menu.det_break = ENDSTOP_DET_BREAK_RU;
    // endstop_menu.flame_sensor = ENDSTOP_FLAME_RU;
    //
    extrude_menu.title       = TITLE_EXTRUDE_RU;
    extrude_menu.in          = EXTRUDER_IN_TEXT_RU;
    extrude_menu.out         = EXTRUDER_OUT_TEXT_RU;
    extrude_menu.ext1        = EXTRUDER_1_TEXT_RU;
    extrude_menu.ext2        = EXTRUDER_2_TEXT_RU;
    extrude_menu.low         = EXTRUDE_LOW_SPEED_TEXT_RU;
    extrude_menu.normal      = EXTRUDE_MEDIUM_SPEED_TEXT_RU;
    extrude_menu.high        = EXTRUDE_HIGH_SPEED_TEXT_RU;
    extrude_menu.temper_text = EXTRUDER_TEMP_TEXT_RU;
    //
    leveling_menu.title     = TITLE_LEVELING_RU;
    leveling_menu.position1 = LEVELING_POINT1_TEXT_RU;
    leveling_menu.position2 = LEVELING_POINT2_TEXT_RU;
    leveling_menu.position3 = LEVELING_POINT3_TEXT_RU;
    leveling_menu.position4 = LEVELING_POINT4_TEXT_RU;
    leveling_menu.position5 = LEVELING_POINT5_TEXT_RU;
    //
    set_menu.title        = TITLE_SET_RU;
    set_menu.filesys      = FILESYS_TEXT_RU;
    set_menu.wifi         = WIFI_TEXT_RU;
    set_menu.about        = ABOUT_TEXT_RU;
    set_menu.fan          = FAN_TEXT_RU;
    set_menu.filament     = FILAMENT_TEXT_RU;
    set_menu.breakpoint   = BREAK_POINT_TEXT_RU;
    set_menu.motoroff     = MOTOR_OFF_TEXT_RU;
    set_menu.motoroffXY   = MOTOR_OFF_XY_TEXT_RU;
    set_menu.language     = LANGUAGE_TEXT_RU;
    set_menu.shutdown     = SHUTDOWN_TEXT_RU;
    set_menu.machine_para = MACHINE_PARA_RU;
    set_menu.eepromSet    = EEPROM_SETTINGS_RU;
    //
    more_menu.title       = TITLE_MORE_RU;
    more_menu.gcode       = MORE_GCODE_RU;
    more_menu.entergcode  = MORE_ENTER_GCODE_EN;
    #if HAS_USER_ITEM(1)
      more_menu.custom1 = MORE_CUSTOM1_TEXT_RU;
    #endif
    #if HAS_USER_ITEM(2)
      more_menu.custom2 = MORE_CUSTOM2_TEXT_RU;
    #endif
    #if HAS_USER_ITEM(3)
      more_menu.custom3 = MORE_CUSTOM3_TEXT_RU;
    #endif
    #if HAS_USER_ITEM(4)
      more_menu.custom4 = MORE_CUSTOM4_TEXT_RU;
    #endif
    #if HAS_USER_ITEM(5)
      more_menu.custom5 = MORE_CUSTOM5_TEXT_RU;
    #endif
    #if HAS_USER_ITEM(6)
      more_menu.custom6 = MORE_CUSTOM6_TEXT_RU;
    #endif

    //
    filesys_menu.title   = TITLE_FILESYS_RU;
    filesys_menu.sd_sys  = SD_CARD_TEXT_RU;
    filesys_menu.usb_sys = U_DISK_TEXT_RU;
    // WIFI
    wifi_menu.title     = WIFI_TEXT;
    wifi_menu.cloud     = CLOUD_TEXT_RU;
    wifi_menu.reconnect = WIFI_RECONNECT_TEXT_RU;
    wifi_menu.info_text = WIFI_INFO_TEXT_EN;
    wifi_menu.link_overtime_text = WIFI_LINK_OVERTIME_EN;

    cloud_menu.title        = TITLE_CLOUD_TEXT_RU;
    cloud_menu.bind         = CLOUD_BINDED_RU;
    cloud_menu.binded       = CLOUD_BINDED_RU;
    cloud_menu.unbind       = CLOUD_UNBIND_RU;
    cloud_menu.unbinding    = CLOUD_UNBINDED_RU;
    cloud_menu.disconnected = CLOUD_DISCONNECTED_RU;
    cloud_menu.unbinded     = CLOUD_UNBINDED_RU;
    cloud_menu.disable      = CLOUD_DISABLE_RU;
    //
    // about_menu.title   = TITLE_ABOUT_RU;
    // about_menu.type    = ABOUT_TYPE_TEXT_RU;
    // about_menu.version = ABOUT_VERSION_TEXT_RU;
    // about_menu.wifi    = ABOUT_WIFI_TEXT_RU;
      about_menu.title   = ABOUT_TITLE_RU;
      about_menu.size    = ABOUT_SZIE_RU;
      about_menu.model   = ABOUT_MODEL_RU;
      about_menu.diameter=   ABOUT_DIAMETER_RU;
      about_menu.voltage =   ABOUT_VOLTAGE_EN;  
      about_menu.version =   ABOUT_VERSIZON_RU; 
      about_menu.support =   ABOUT_SUPPORT_RU; 
      about_menu.sales   =   ABOUT_SALES_RU; 
    
    //
    fan_menu.title = TITLE_FAN_RU;
    fan_menu.add   = FAN_ADD_TEXT_RU;
    fan_menu.dec   = FAN_DEC_TEXT_RU;
    fan_menu.state = FAN_TIPS1_TEXT_RU;
    //
    filament_menu.title                               = TITLE_FILAMENT_RU;
    filament_menu.in                                  = FILAMENT_IN_TEXT_RU;
    filament_menu.out                                 = FILAMENT_OUT_TEXT_RU;
    filament_menu.ext1                                = FILAMENT_EXT0_TEXT_RU;
    filament_menu.ext2                                = FILAMENT_EXT1_TEXT_RU;
    filament_menu.ready_replace                       = FILAMENT_CHANGE_TEXT_RU;
    filament_menu.filament_dialog_load_heat           = FILAMENT_DIALOG_LOAD_HEAT_TIPS_RU;
    filament_menu.filament_dialog_load_heat_confirm   = FILAMENT_DIALOG_LOAD_CONFIRM1_TIPS_RU;
    filament_menu.filament_dialog_loading             = FILAMENT_DIALOG_LOADING_TIPS_RU;
    filament_menu.filament_dialog_load_completed      = FILAMENT_DIALOG_LOAD_COMPLETE_TIPS_RU;
    filament_menu.filament_dialog_unload_heat         = FILAMENT_DIALOG_UNLOAD_HEAT_TIPS_RU;
    filament_menu.filament_dialog_unload_heat_confirm = FILAMENT_DIALOG_UNLOAD_CONFIRM_TIPS_RU;
    filament_menu.filament_dialog_unloading           = FILAMENT_DIALOG_UNLOADING_TIPS_RU;
    filament_menu.filament_dialog_unload_completed    = FILAMENT_DIALOG_UNLOAD_COMPLETE_TIPS_RU;
    filament_menu.heating_nozzle                      = HEATING_NOZZLE_RU;
    filament_menu.stop                                = FILAMENT_STOP_TEXT_RU;

    //
    language_menu.title = TITLE_LANGUAGE_RU;
    language_menu.next  = PAGE_DOWN_TEXT_RU;
    language_menu.up    = PAGE_UP_TEXT_RU;
    //
    printing_menu.title  = TITLE_PRINTING_RU;
    printing_menu.option = PRINTING_OPERATION_RU;
    printing_menu.stop   = PRINTING_STOP_RU;
    printing_menu.pause  = PRINTING_PAUSE_RU;
    printing_menu.resume = PRINTING_RESUME_RU;

    //
    operation_menu.title      = TITLE_OPERATION_RU;
    operation_menu.pause      = PRINTING_PAUSE_RU;
    operation_menu.stop       = PRINTING_STOP_RU;
    operation_menu.temp       = PRINTING_TEMP_RU;
    operation_menu.fan        = FAN_TEXT_RU;
    operation_menu.extr       = PRINTING_EXTRUDER_RU;
    operation_menu.speed      = PRINTING_CHANGESPEED_RU;
    operation_menu.filament   = FILAMENT_TEXT_RU;
    operation_menu.more       = PRINTING_MORE_RU;
    operation_menu.move       = PRINTING_MOVE_RU;
    operation_menu.auto_off   = AUTO_SHUTDOWN_RU;
    operation_menu.manual_off = MANUAL_SHUTDOWN_RU;
    //
    pause_menu.title    = TITLE_PAUSE_RU;
    pause_menu.resume   = PRINTING_RESUME_RU;
    pause_menu.stop     = PRINTING_STOP_RU;
    pause_menu.extrude  = PRINTING_EXTRUDER_RU;
    pause_menu.move     = PRINTING_MOVE_RU;
    pause_menu.filament = FILAMENT_TEXT_RU;
    pause_menu.more     = PRINTING_MORE_RU;

    //
    speed_menu.title         = TITLE_CHANGESPEED_RU;
    speed_menu.add           = ADD_TEXT_RU;
    speed_menu.dec           = DEC_TEXT_RU;
    speed_menu.move          = MOVE_SPEED_RU;
    speed_menu.extrude       = EXTRUDER_SPEED_RU;
    speed_menu.extrude_speed = EXTRUDER_SPEED_STATE_RU;
    speed_menu.move_speed    = MOVE_SPEED_STATE_RU;
    //
    printing_more_menu.title      = TITLE_MORE_RU;
    printing_more_menu.fan        = FAN_TEXT_RU;
    printing_more_menu.auto_close = AUTO_SHUTDOWN_RU;
    printing_more_menu.manual     = MANUAL_SHUTDOWN_RU;
    printing_more_menu.speed      = PRINTING_CHANGESPEED_RU;
    printing_more_menu.temp       = PRINTING_TEMP_RU;

    print_file_dialog_menu.confirm               = DIALOG_CONFIRM_RU;
    print_file_dialog_menu.cancel                = DIALOG_CANCLE_RU;
    print_file_dialog_menu.print_file            = DIALOG_PRINT_MODEL_RU;
    print_file_dialog_menu.cancel_print          = DIALOG_CANCEL_PRINT_RU;
    print_file_dialog_menu.retry                 = DIALOG_RETRY_RU;
    print_file_dialog_menu.stop                  = DIALOG_STOP_RU;
    print_file_dialog_menu.no_file_print_tips    = DIALOG_ERROR_TIPS1_RU;
    print_file_dialog_menu.print_from_breakpoint = DIALOG_REPRINT_FROM_BREAKPOINT_RU;
    print_file_dialog_menu.close_machine_error   = DIALOG_ERROR_TIPS2_RU;
    print_file_dialog_menu.filament_no_press     = DIALOG_FILAMENT_NO_PRESS_RU;
    print_file_dialog_menu.print_finish          = DIALOG_PRINT_FINISH_RU;
    print_file_dialog_menu.print_time            = DIALOG_PRINT_TIME_RU;
    print_file_dialog_menu.reprint               = DIALOG_REPRINT_RU;
    print_file_dialog_menu.wifi_enable_tips      = DIALOG_WIFI_ENABLE_TIPS_EN;
#ifdef MKS_DEBUG_HUANG
    print_file_dialog_menu.autolevelingTips      = DIALOG_AUTO_LEVELING_TIPS_RU;
#endif      

    dialog_menu.emergency_text1 = DIALOG_EMERGENCY_TEXT1_EN;
    dialog_menu.emergency_text2 = DIALOG_EMERGENCY_TEXT2_EN;
    dialog_menu.emergency_text3 = DIALOG_EMERGENCY_TEXT3_EN;
    dialog_menu.emergency_text4 = DIALOG_EMERGENCY_TEXT4_EN;
    dialog_menu.no_filament_text1 = DIALOG_NO_FILAMENT_TEXT1_EN;
    dialog_menu.no_filament_text2 = DIALOG_NO_FILAMENT_TEXT2_EN;
    dialog_menu.no_filament_text3 = DIALOG_NO_FILAMENT_TEXT3_EN;

    pause_msg_menu.pausing       = MESSAGE_PAUSING_RU;
    pause_msg_menu.changing      = MESSAGE_CHANGING_RU;
    pause_msg_menu.unload        = MESSAGE_UNLOAD_RU;
    pause_msg_menu.waiting       = MESSAGE_WAITING_RU;
    pause_msg_menu.insert        = MESSAGE_INSERT_RU;
    pause_msg_menu.load          = MESSAGE_LOAD_RU;
    pause_msg_menu.purge         = MESSAGE_PURGE_RU;
    pause_msg_menu.resume        = MESSAGE_RESUME_RU;
    pause_msg_menu.heat          = MESSAGE_HEAT_RU;
    pause_msg_menu.heating       = MESSAGE_HEATING_RU;
    pause_msg_menu.option        = MESSAGE_OPTION_RU;
    pause_msg_menu.purgeMore     = MESSAGE_PURGE_MORE_RU;
    pause_msg_menu.continuePrint = MESSAGE_CONTINUE_PRINT_RU;
    eeprom_menu.title            = EEPROM_SETTINGS_TITLE_RU;
    eeprom_menu.store            = EEPROM_SETTINGS_STORE_RU;
    eeprom_menu.read             = EEPROM_SETTINGS_READ_RU;
    eeprom_menu.revert           = EEPROM_SETTINGS_REVERT_RU;
    eeprom_menu.storeTips        = EEPROM_STORE_TIPS_RU;
    eeprom_menu.readTips         = EEPROM_READ_TIPS_RU;
    eeprom_menu.revertTips       = EEPROM_REVERT_TIPS_RU;
}
