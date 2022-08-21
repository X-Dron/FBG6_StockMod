
#pragma once

#ifdef __cplusplus
  extern "C" { /* C-declarations for C++ */
#endif

void lv_clear_voice();
void lv_draw_voice();
void button_sound_start();
void button_sound_end();
void print_tone_sound_start();
void print_tone_sound_end();
void no_filament_sound_start();
void no_filament_sound_end();
void endstop_sound_start();
void endstop_sound_end();
#ifdef __cplusplus
  } /* C-declarations for C++ */
#endif

