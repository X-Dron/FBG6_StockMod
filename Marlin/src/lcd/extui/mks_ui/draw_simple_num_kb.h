
#pragma once

#ifdef __cplusplus
  extern "C" { /* C-declarations for C++ */
#endif
typedef enum {
    GCODE,
    EX1_TEMP,
    BED_TEMP,
    FAN_SPEED,
    PRINT_SPEED,
    EX_SPEED,
    SET_PLA_BED_TEMP,
    SET_PLA_TEMP,
    SET_ABS_BED_TEMP,
    SET_ABS_TEMP,
    SET_LAYER_STOP,
    WIFI_PASSWORD, //password
} CHANGE_NUM;
void mks_clear_simple_num_kb();
void mks_draw_simple_num_kb(uint16_t option_ui,CHANGE_NUM chang);
#ifdef __cplusplus
  } /* C-declarations for C++ */
#endif

