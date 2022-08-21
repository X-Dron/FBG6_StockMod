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
#pragma once

#ifdef __cplusplus
  extern "C" { /* C-declarations for C++ */
#endif
typedef enum{
  EX_LOW_SPEED = 1,
  EX_MID_SPEED = 5,
  EX_HIGH_SPEED = 10,
}EX_SPEED_ENUM;

typedef enum{
  LOW_SPEED,
  HIGH_SPEED,
}FIlAMENT_SPEED;
typedef struct{
  
  FIlAMENT_SPEED cur_speed_step = LOW_SPEED;
  uint16_t cur_speed = 1000;//gCfgItems.filamentchange_load_speed
  bool contimued = false;
  bool contimued_dir = false;
  bool contimued_open = false;
  bool e_dir_odr;
}FILAMNT_DATA;
extern FILAMNT_DATA fila_data;

void lv_draw_filament_change();
void lv_clear_filament_change();
void disp_filament_type();
void disp_filament_temp();

#ifdef __cplusplus
  } /* C-declarations for C++ */
#endif
