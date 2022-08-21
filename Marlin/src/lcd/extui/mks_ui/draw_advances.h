
#pragma once

#ifdef __cplusplus
  extern "C" { /* C-declarations for C++ */
#endif


typedef struct{
  
  bool light_flag = true;

  bool fan5051_flag = true;

  bool flame_flag = true;


}ADVANCES_UI_DATA;
extern ADVANCES_UI_DATA advances_data;

void lv_clear_advances();
void lv_draw_advances();
void flame_beep_sound_start();
void flame_beep_sound_end();
void lv_fan5051();
void lv_flame();
// void button_sound_start();
// void button_sound_end();
// void print_tone_sound_start();
// void print_tone_sound_end();
// void no_filament_sound_start();
// void no_filament_sound_end();
// void endstop_sound_start();
// void endstop_sound_end();
#ifdef __cplusplus
  } /* C-declarations for C++ */
#endif

