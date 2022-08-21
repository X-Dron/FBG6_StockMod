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

#include "string.h"
#include "draw_ui.h"
#include "pic_manager.h"
#include "draw_ready_print.h"
#include "mks_hardware.h"
#include "SPIFlashStorage.h"
#include "../../../libs/W25Qxx.h"
#include "../../../sd/cardreader.h"
#include "../../../MarlinCore.h"

extern uint16_t DeviceCode;

#if ENABLED(SDSUPPORT)
  extern char *createFilename(char * const buffer, const dir_t &p);
#endif
// #define add_

static const char assets[][LONG_FILENAME_LENGTH] = {

  // ready screen
  // "png_preheat.bin",
  // "png_r_preheat.bin",
  // "png_r_filament.bin",
  "png_emergency.bin",
  // "png_filament.bin",
  "png_print.bin",
  "png_ready_bed.bin",
  "png_ready_ex.bin",
  "png_tool.bin",
  "png_move.bin",


  "png_level_four.bin",
  "png_level_three.bin",
  "png_level_one.bin",
  "png_level_two.bin",
  "png_level_z.bin",
  "png_back1.bin",
  "png_level_zoffset.bin",


  // Tool screen
  "png_about.bin",
  "png_advances.bin",
  "png_back.bin",
  "png_endstop.bin",
  "png_language.bin",
  "png_leveling.bin",
  "png_tempsetting.bin",
  "png_voice.bin",
  "png_wifi.bin",


  "png_en.bin",
  "png_dis.bin",
  "png_off.bin",
  "png_on.bin",

  "png_back2.bin",
  "png_chinese_off.bin",
  "png_chinese_on.bin",
  "png_englist_off.bin",
  "png_englist_on.bin",
  "png_next.bin",
  "png_pre.bin",
  "bmp_language_set_r.bin",
  "bmp_language_set_b.bin",
  "bmp_language_set_g.bin",


  "png_back3.bin",
  "png_em_ctr.bin",
  "png_home.bin",
  "png_mv_step.bin",
  "png_englist_on.bin",
  "png_x_add.bin",
  "png_x_dec.bin",
  "png_y_add.bin",
  "png_y_dec.bin",
  "png_z_add.bin",
  "png_z_dec.bin",


  "png_abs.bin",
  "png_cool.bin",
  "png_pla.bin",
  "png_temp_add.bin",
  "png_temp_b1.bin",
  "png_temp_b2.bin",
  "png_temp_dec.bin",
  "png_temp_e1.bin",
  "png_temp_e2.bin",

  
  "png_filament_img.bin",
  "png_loop_off.bin",
  "png_loop_on.bin",
  "png_squeeze.bin",
  "png_squeeze_in.bin",

  "png_back4.bin",
  "png_file.bin",
  "png_folder.bin",
  "png_next_page.bin",
  "png_pre_page.bin",

  "png_bed_temp.bin",
  "png_continue.bin",
  "png_e_temp.bin",
  "png_ex_factor.bin",
  "png_fan_temp.bin",
  "png_layer.bin",
  "png_speed_factor.bin",
  "png_speed_mm.bin",
  "png_stop.bin",
  "png_suspend.bin",
  "png_time.bin",
  "png_z_offset.bin",
  "png_baby_z_add.bin",
  "png_baby_z_dec.bin",

  "png_cur_for_preheat.bin",
  "png_cor_filament.bin",
  "png_err_tips.bin",
  "png_pro_tips.bin",
  "png_back5.bin",
  "png_more.bin",
  "png_fan_5010.bin", 
  "png_fan_5010_on.bin",
  "png_fan_5010_off.bin",
  #if HAS_MULTI_EXTRUDER
    "bmp_extru2.bin",
  #endif
 





  #if HAS_MULTI_EXTRUDER
    "bmp_ext2_state.bin",
  #endif


  // Manual Level screen (only if auto level is disabled)
  #if DISABLED(AUTO_BED_LEVELING_BILINEAR)
    "bmp_leveling1.bin",
    "bmp_leveling2.bin",
    "bmp_leveling3.bin",
    "bmp_leveling4.bin",
    "bmp_leveling5.bin",
  #endif

  // Language Select screen
  #if HAS_LANG_SELECT_SCREEN
    // "bmp_language.bin",
    // "bmp_simplified_cn.bin",
    // "bmp_simplified_cn_sel.bin",
    // "bmp_traditional_cn.bin",
    // "bmp_traditional_cn_sel.bin",
    // "bmp_english.bin",
    // "bmp_english_sel.bin",
    // "bmp_russian.bin",
    // "bmp_russian_sel.bin",
    // "bmp_spanish.bin",
    // "bmp_spanish_sel.bin",
    // "bmp_french.bin",
    // "bmp_french_sel.bin",
    // "bmp_italy.bin",
    // "bmp_italy_sel.bin",
  #endif // HAS_LANG_SELECT_SCREEN

  // G-code preview
  #if HAS_GCODE_DEFAULT_VIEW_IN_FLASH
    "bmp_preview.bin",
  #endif

  #if HAS_LOGO_IN_FLASH
    "bmp_logo.bin"
  #endif

  // // Settings screen
  // "bmp_about.bin",
  // "bmp_eeprom_settings.bin",
  // "bmp_machine_para.bin",
  // "bmp_function1.bin",


  // Base icons
  // "bmp_arrow.bin",
  // "bmp_back70x40.bin",
  // "bmp_value_blank.bin",
  // "bmp_blank_sel.bin",
  // "bmp_disable.bin",
  // "bmp_enable.bin",
  // "bmp_return.bin",

  #if ENABLED(MKS_WIFI_MODULE)
    // Wifi screen
    // "bmp_wifi.bin",
    // "bmp_cloud.bin",
  #endif

  #if ENABLED(MULTI_VOLUME)
    "bmp_usb_disk.bin",
    // "bmp_usb_disk_sel.bin",
    "bmp_sd.bin",
    // "bmp_sd_sel.bin",
  #endif

  // // Babystep screen
  // // "bmp_baby_move0_01.bin",
  // // "bmp_baby_move0_05.bin",
  // // "bmp_baby_move0_1.bin",
  // "bmp_init_state.bin",
  // // More screen
  // "bmp_custom1.bin",
  // "bmp_custom2.bin",
  // "bmp_custom3.bin",
  // "bmp_custom4.bin",
  // "bmp_custom5.bin",
  // "bmp_custom6.bin",
  // "bmp_custom7.bin"
};

#if HAS_SPI_FLASH_FONT
  static char fonts[][LONG_FILENAME_LENGTH] = { "FontUNIGBK.bin" };
  static char fonts_12[][LONG_FILENAME_LENGTH] = { "DengXian_8.bin" };
  static char fonts_16[][LONG_FILENAME_LENGTH] = { "DengXian_12.bin" };
  static char fonts_22[][LONG_FILENAME_LENGTH] = { "DengXian_16.bin" };
  static char fonts_28[][LONG_FILENAME_LENGTH] = { "DengXian_20.bin" };
#endif

uint8_t currentFlashPage = 0;

uint32_t lv_get_pic_addr(uint8_t *Pname) {
  uint8_t Pic_cnt;
  uint8_t i, j;
  PIC_MSG PIC;
  uint32_t tmp_cnt = 0;
  uint32_t addr = 0;

  currentFlashPage = 0;

  #if ENABLED(MARLIN_DEV_MODE)
    SERIAL_ECHOLNPGM("Getting picture SPI Flash Address: ", (const char*)Pname);
  #endif

  W25QXX.init(SPI_HALF_SPEED);

  W25QXX.SPI_FLASH_BufferRead(&Pic_cnt, PIC_COUNTER_ADDR, 1);
  if (Pic_cnt == 0xFF) Pic_cnt = 0;
  for (i = 0; i < Pic_cnt; i++) {
    j = 0;
    do {
      W25QXX.SPI_FLASH_BufferRead(&PIC.name[j], PIC_NAME_ADDR + tmp_cnt, 1);
      tmp_cnt++;
    } while (PIC.name[j++] != '\0');

    if ((strcasecmp((char*)Pname, (char*)PIC.name)) == 0) {
      if (DeviceCode == 0x9488 || DeviceCode == 0x5761)
        addr = PIC_DATA_ADDR_TFT35 + i * PER_PIC_MAX_SPACE_TFT35;
      else
        addr = PIC_DATA_ADDR_TFT32 + i * PER_PIC_MAX_SPACE_TFT32;
      return addr;
    }
  }
  return addr;
}

const char *assetsPath = "assets";
const char *bakPath = "_assets";

void spiFlashErase_PIC() {
  volatile uint32_t pic_sectorcnt = 0;
  W25QXX.init(SPI_HALF_SPEED);
  // erase 0x001000 -64K
  for (pic_sectorcnt = 0; pic_sectorcnt < (64 - 4) / 4; pic_sectorcnt++) {
    watchdog_refresh();
    W25QXX.SPI_FLASH_SectorErase(PICINFOADDR + pic_sectorcnt * 4 * 1024);
  }
  // erase 64K -- 6M
  for (pic_sectorcnt = 0; pic_sectorcnt < (PIC_SIZE_xM * 1024 / 64 - 1); pic_sectorcnt++) {
    watchdog_refresh();
    W25QXX.SPI_FLASH_BlockErase((pic_sectorcnt + 1) * 64 * 1024);
  }
}

#if HAS_SPI_FLASH_FONT
  void spiFlashErase_FONT() {
    volatile uint32_t Font_sectorcnt = 0;
    W25QXX.init(SPI_HALF_SPEED);
    for (Font_sectorcnt = 0; Font_sectorcnt < 32 - 1; Font_sectorcnt++) {
      watchdog_refresh();
      W25QXX.SPI_FLASH_BlockErase(FONTINFOADDR + Font_sectorcnt * 64 * 1024);
    }
    for (Font_sectorcnt = 0; Font_sectorcnt < 32 - 1; Font_sectorcnt++) {  // 32
      watchdog_refresh();
      W25QXX.SPI_FLASH_BlockErase(FONTINFOADDR_22 + Font_sectorcnt * 64 * 1024);
    }
    for (Font_sectorcnt = 0; Font_sectorcnt < 32 - 1; Font_sectorcnt++) {  // 32
      watchdog_refresh();
      W25QXX.SPI_FLASH_BlockErase(FONTINFOADDR_16 + Font_sectorcnt * 64 * 1024);
    }
    for (Font_sectorcnt = 0; Font_sectorcnt < 32 - 1; Font_sectorcnt++) {  // 32
      watchdog_refresh();
      W25QXX.SPI_FLASH_BlockErase(FONTINFOADDR_12 + Font_sectorcnt * 64 * 1024);
    }
    for (Font_sectorcnt = 0; Font_sectorcnt < 32 - 1; Font_sectorcnt++) {  // 32
      watchdog_refresh();
      W25QXX.SPI_FLASH_BlockErase(FONTINFOADDR_28 + Font_sectorcnt * 64 * 1024);
    }
  }
#endif

uint32_t LogoWrite_Addroffset = 0;

uint8_t Pic_Logo_Write(uint8_t *LogoName, uint8_t *Logo_Wbuff, uint32_t LogoWriteSize) {
  if (LogoWriteSize <= 0) return 0;

  W25QXX.SPI_FLASH_BufferWrite(Logo_Wbuff, PIC_LOGO_ADDR + LogoWrite_Addroffset, LogoWriteSize);

  for (uint32_t i = 0; i < LogoWriteSize; i++) {
    uint8_t temp1;
    W25QXX.SPI_FLASH_BufferRead(&temp1, PIC_LOGO_ADDR + LogoWrite_Addroffset + i, 1);
    if (*(Logo_Wbuff + i) != temp1) return 0;
  }
  LogoWrite_Addroffset += LogoWriteSize;
  const uint32_t logo_maxsize = DeviceCode == 0x9488 || DeviceCode == 0x5761 ? LOGO_MAX_SIZE_TFT35 : LOGO_MAX_SIZE_TFT32;
  if (LogoWrite_Addroffset >= logo_maxsize) LogoWrite_Addroffset = 0;
  return 1;
}

uint32_t TitleLogoWrite_Addroffset = 0;
uint8_t Pic_TitleLogo_Write(uint8_t *TitleLogoName, uint8_t *TitleLogo_Wbuff, uint32_t TitleLogoWriteSize) {
  if (TitleLogoWriteSize <= 0)
    return 0;
  if ((DeviceCode == 0x9488) || (DeviceCode == 0x5761))
    W25QXX.SPI_FLASH_BufferWrite(TitleLogo_Wbuff, PIC_ICON_LOGO_ADDR_TFT35 + TitleLogoWrite_Addroffset, TitleLogoWriteSize);
  else
    W25QXX.SPI_FLASH_BufferWrite(TitleLogo_Wbuff, PIC_ICON_LOGO_ADDR_TFT32 + TitleLogoWrite_Addroffset, TitleLogoWriteSize);
  TitleLogoWrite_Addroffset += TitleLogoWriteSize;
  if (TitleLogoWrite_Addroffset >= TITLELOGO_MAX_SIZE)
    TitleLogoWrite_Addroffset = 0;
  return 1;
}

uint32_t default_view_addroffset_r = 0;
void default_view_Write(uint8_t *default_view__Rbuff, uint32_t default_view_Writesize) {
  W25QXX.SPI_FLASH_BufferWrite(default_view__Rbuff, DEFAULT_VIEW_ADDR_TFT35 + default_view_addroffset_r, default_view_Writesize);
  default_view_addroffset_r += default_view_Writesize;
  if (default_view_addroffset_r >= DEFAULT_VIEW_MAX_SIZE)
    default_view_addroffset_r = 0;
}

uint32_t Pic_Info_Write(uint8_t *P_name, uint32_t P_size) {
  uint8_t pic_counter = 0;
  uint32_t Pic_SaveAddr;
  uint32_t Pic_SizeSaveAddr;
  uint32_t Pic_NameSaveAddr;
  uint8_t Pname_temp;
  uint32_t i, j;
  uint32_t name_len = 0;
  uint32_t SaveName_len = 0;
  union union32 size_tmp;

  W25QXX.SPI_FLASH_BufferRead(&pic_counter, PIC_COUNTER_ADDR, 1);

  if (pic_counter == 0xFF)
    pic_counter = 0;

  if ((DeviceCode == 0x9488) || (DeviceCode == 0x5761))
    Pic_SaveAddr = PIC_DATA_ADDR_TFT35 + pic_counter * PER_PIC_MAX_SPACE_TFT35;
  else
    Pic_SaveAddr = PIC_DATA_ADDR_TFT32 + pic_counter * PER_PIC_MAX_SPACE_TFT32;

  for (j = 0; j < pic_counter; j++) {
    do {
      W25QXX.SPI_FLASH_BufferRead(&Pname_temp, PIC_NAME_ADDR + SaveName_len, 1);
      SaveName_len++;
    } while (Pname_temp != '\0');
  }
  i = 0;
  while ((*(P_name + i) != '\0')) {
    i++;
    name_len++;
  }

  Pic_NameSaveAddr = PIC_NAME_ADDR + SaveName_len;
  W25QXX.SPI_FLASH_BufferWrite(P_name, Pic_NameSaveAddr, name_len + 1);
  Pic_SizeSaveAddr = PIC_SIZE_ADDR + 4 * pic_counter;
  size_tmp.dwords = P_size;
  W25QXX.SPI_FLASH_BufferWrite(size_tmp.bytes, Pic_SizeSaveAddr, 4);

  pic_counter++;
  W25QXX.SPI_FLASH_SectorErase(PIC_COUNTER_ADDR);
  W25QXX.SPI_FLASH_BufferWrite(&pic_counter, PIC_COUNTER_ADDR, 1);

  return Pic_SaveAddr;
}

#if ENABLED(SDSUPPORT)

  static void dosName2LongName(const char dosName[11], char *longName) {
    uint8_t j = 0;
    LOOP_L_N(i, 11) {
      if (i == 8) longName[j++] = '.';
      if (dosName[i] == '\0' || dosName[i] == ' ') continue;
      longName[j++] = dosName[i];
    }
    longName[j] = '\0';
  }

  static int8_t arrayFindStr(const char arr[][LONG_FILENAME_LENGTH], uint8_t arraySize, const char *str) {
    for (uint8_t a = 0; a < arraySize; a++) {
      if (strcasecmp(arr[a], str) == 0)
        return a;
    }
    return -1;
  }

  #if ENABLED(MARLIN_DEV_MODE)
    static uint32_t totalSizes = 0, totalCompressed = 0;
  #endif

  #define ASSET_TYPE_ICON       0
  #define ASSET_TYPE_LOGO       1
  #define ASSET_TYPE_TITLE_LOGO 2
  #define ASSET_TYPE_G_PREVIEW  3
  #define ASSET_TYPE_FONT       4
  #define ASSET_TYPE_FONT_22    5
  #define ASSET_TYPE_FONT_16    6
  #define ASSET_TYPE_FONT_12    7
  #define ASSET_TYPE_FONT_28    8
  static void loadAsset(SdFile &dir, dir_t& entry, const char *fn, int8_t assetType) {
    SdFile file;
    char dosFilename[FILENAME_LENGTH];
    createFilename(dosFilename, entry);
    if (!file.open(&dir, dosFilename, O_READ)) {
      #if ENABLED(MARLIN_DEV_MODE)
        SERIAL_ECHOLNPGM("Error opening Asset: ", fn);
      #endif
      return;
    }

    watchdog_refresh();
    disp_assets_update_progress(fn);

    W25QXX.init(SPI_HALF_SPEED);

    uint16_t pbr;
    uint32_t pfileSize;
    uint32_t totalSizeLoaded = 0;
    uint32_t Pic_Write_Addr;
    pfileSize = file.fileSize();
    totalSizeLoaded += pfileSize;
    if (assetType == ASSET_TYPE_LOGO) {
      do {
        watchdog_refresh();
        pbr = file.read(public_buf, BMP_WRITE_BUF_LEN);
        Pic_Logo_Write((uint8_t *)fn, public_buf, pbr);
      } while (pbr >= BMP_WRITE_BUF_LEN);
    }
    else if (assetType == ASSET_TYPE_TITLE_LOGO) {
      do {
        watchdog_refresh();
        pbr = file.read(public_buf, BMP_WRITE_BUF_LEN);
        Pic_TitleLogo_Write((uint8_t *)fn, public_buf, pbr);
      } while (pbr >= BMP_WRITE_BUF_LEN);
    }
    else if (assetType == ASSET_TYPE_G_PREVIEW) {
      do {
        watchdog_refresh();
        pbr = file.read(public_buf, BMP_WRITE_BUF_LEN);
        default_view_Write(public_buf, pbr);
      } while (pbr >= BMP_WRITE_BUF_LEN);
    }
    else if (assetType == ASSET_TYPE_ICON) {
      Pic_Write_Addr = Pic_Info_Write((uint8_t *)fn, pfileSize);
      SPIFlash.beginWrite(Pic_Write_Addr);
      #if HAS_SPI_FLASH_COMPRESSION
        do {
          watchdog_refresh();
          pbr = file.read(public_buf, SPI_FLASH_PageSize);
          TERN_(MARLIN_DEV_MODE, totalSizes += pbr);
          SPIFlash.writeData(public_buf, SPI_FLASH_PageSize);
        } while (pbr >= SPI_FLASH_PageSize);
      #else
        do {
          pbr = file.read(public_buf, BMP_WRITE_BUF_LEN);
          W25QXX.SPI_FLASH_BufferWrite(public_buf, Pic_Write_Addr, pbr);
          Pic_Write_Addr += pbr;
        } while (pbr >= BMP_WRITE_BUF_LEN);
      #endif
      #if ENABLED(MARLIN_DEV_MODE)
        SERIAL_ECHOLNPGM("Space used: ", fn, " - ", (SPIFlash.getCurrentPage() + 1) * SPI_FLASH_PageSize / 1024, "KB");
        totalCompressed += (SPIFlash.getCurrentPage() + 1) * SPI_FLASH_PageSize;
      #endif
      SPIFlash.endWrite();
    }
    else if (assetType == ASSET_TYPE_FONT) {
      Pic_Write_Addr = UNIGBK_FLASH_ADDR;
      do {
        watchdog_refresh();
        pbr = file.read(public_buf, BMP_WRITE_BUF_LEN);
        W25QXX.SPI_FLASH_BufferWrite(public_buf, Pic_Write_Addr, pbr);
        Pic_Write_Addr += pbr;
      } while (pbr >= BMP_WRITE_BUF_LEN);
    }
    else if(assetType == ASSET_TYPE_FONT_22)
    {
      Pic_Write_Addr = UNIGBK_FLASH_ADDR_22;
      do {
        watchdog_refresh();
        pbr = file.read(public_buf, BMP_WRITE_BUF_LEN);
        W25QXX.SPI_FLASH_BufferWrite(public_buf, Pic_Write_Addr, pbr);
        Pic_Write_Addr += pbr;
      } while (pbr >= BMP_WRITE_BUF_LEN);
      // W25QXX.SPI_FLASH_BufferRead(public_buf, UNIGBK_FLASH_ADDR_22, BMP_WRITE_BUF_LEN);
    }
    else if(assetType == ASSET_TYPE_FONT_16)
    {
      Pic_Write_Addr = UNIGBK_FLASH_ADDR_16;
      do {
        watchdog_refresh();
        pbr = file.read(public_buf, BMP_WRITE_BUF_LEN);
        W25QXX.SPI_FLASH_BufferWrite(public_buf, Pic_Write_Addr, pbr);
        Pic_Write_Addr += pbr;
      } while (pbr >= BMP_WRITE_BUF_LEN);
      // W25QXX.SPI_FLASH_BufferRead(public_buf, UNIGBK_FLASH_ADDR_16, BMP_WRITE_BUF_LEN);
    }
    else if(assetType == ASSET_TYPE_FONT_12)
    {
      Pic_Write_Addr = UNIGBK_FLASH_ADDR_12;
      do {
        watchdog_refresh();
        pbr = file.read(public_buf, BMP_WRITE_BUF_LEN);
        W25QXX.SPI_FLASH_BufferWrite(public_buf, Pic_Write_Addr, pbr);
        Pic_Write_Addr += pbr;
      } while (pbr >= BMP_WRITE_BUF_LEN);
      // W25QXX.SPI_FLASH_BufferRead(public_buf, UNIGBK_FLASH_ADDR_16, BMP_WRITE_BUF_LEN);
    }
    else if(assetType == ASSET_TYPE_FONT_28)
    {
      Pic_Write_Addr = UNIGBK_FLASH_ADDR_28;
      do {
        watchdog_refresh();
        pbr = file.read(public_buf, BMP_WRITE_BUF_LEN);
        W25QXX.SPI_FLASH_BufferWrite(public_buf, Pic_Write_Addr, pbr);
        Pic_Write_Addr += pbr;
      } while (pbr >= BMP_WRITE_BUF_LEN);
      // W25QXX.SPI_FLASH_BufferRead(public_buf, UNIGBK_FLASH_ADDR_16, BMP_WRITE_BUF_LEN);
    }

    file.close();

    #if ENABLED(MARLIN_DEV_MODE)
      SERIAL_ECHOLNPGM("Asset added: ", fn);
    #endif
  }

  void UpdateAssets() {
    if (!card.isMounted()) return;
    SdFile dir, root = card.getroot();
    if (dir.open(&root, assetsPath, O_RDONLY)) {

      disp_assets_update();
      disp_assets_update_progress("Erasing pics...");
      watchdog_refresh();
      spiFlashErase_PIC();
      #if HAS_SPI_FLASH_FONT
        disp_assets_update_progress("Erasing fonts...");
        watchdog_refresh();
        spiFlashErase_FONT();
      #endif
      // spi_flash_read_test();

      disp_assets_update_progress("Reading files...");
      dir_t d;
      while (dir.readDir(&d, card.longFilename) > 0) {
        // If we don't get a long name, but gets a short one, try it
        if (card.longFilename[0] == 0 && d.name[0] != 0)
          dosName2LongName((const char*)d.name, card.longFilename);
        if (card.longFilename[0] == 0) continue;
        if (card.longFilename[0] == '.') continue;

        int8_t a = arrayFindStr(assets, COUNT(assets), card.longFilename);
        if (a >= 0 && a < (int8_t)COUNT(assets)) {
          uint8_t assetType = ASSET_TYPE_ICON;
          if (strstr(assets[a], "_logo"))
            assetType = ASSET_TYPE_LOGO;
          else if (strstr(assets[a], "_titlelogo"))
            assetType = ASSET_TYPE_TITLE_LOGO;
          else if (strstr(assets[a], "_preview"))
            assetType = ASSET_TYPE_G_PREVIEW;

          loadAsset(dir, d, assets[a], assetType);

          continue;
        }

        #if HAS_SPI_FLASH_FONT
          a = arrayFindStr(fonts, COUNT(fonts), card.longFilename);
          if (a >= 0 && a < (int8_t)COUNT(fonts))
            loadAsset(dir, d, fonts[a], ASSET_TYPE_FONT);
            
          a = arrayFindStr(fonts_22, COUNT(fonts_22), card.longFilename);
          if (a >= 0 && a < (int8_t)COUNT(fonts_22))
            loadAsset(dir, d, fonts_22[a], ASSET_TYPE_FONT_22);
            
          a = arrayFindStr(fonts_16, COUNT(fonts_16), card.longFilename);
          if (a >= 0 && a < (int8_t)COUNT(fonts_16))
            loadAsset(dir, d, fonts_16[a], ASSET_TYPE_FONT_16);
          
          a = arrayFindStr(fonts_12, COUNT(fonts_12), card.longFilename);
          if (a >= 0 && a < (int8_t)COUNT(fonts_12))
            loadAsset(dir, d, fonts_12[a], ASSET_TYPE_FONT_12);
            
          a = arrayFindStr(fonts_28, COUNT(fonts_28), card.longFilename);
          if (a >= 0 && a < (int8_t)COUNT(fonts_28))
            loadAsset(dir, d, fonts_28[a], ASSET_TYPE_FONT_28);
        #endif
      }
      dir.rename(&root, bakPath);
    }
    dir.close();

    #if ENABLED(MARLIN_DEV_MODE)
      uint8_t pic_counter = 0;
      W25QXX.SPI_FLASH_BufferRead(&pic_counter, PIC_COUNTER_ADDR, 1);
      SERIAL_ECHOLNPGM("Total assets loaded: ", pic_counter);
      SERIAL_ECHOLNPGM("Total Uncompressed: ", totalSizes, ", Compressed: ", totalCompressed);
    #endif
  }

  #if HAS_SPI_FLASH_FONT
    void spi_flash_read_test() 
    { 
      W25QXX.SPI_FLASH_BufferRead(public_buf, UNIGBK_FLASH_ADDR_16, BMP_WRITE_BUF_LEN);
      printf("%s", public_buf); 
    }
  #endif

#endif // SDSUPPORT

void Pic_Read(uint8_t *Pname, uint8_t *P_Rbuff) {
  uint8_t i, j;
  uint8_t Pic_cnt;
  uint32_t tmp_cnt = 0;
  PIC_MSG PIC;

  W25QXX.SPI_FLASH_BufferRead(&Pic_cnt, PIC_COUNTER_ADDR, 1);
  if (Pic_cnt == 0xFF)
    Pic_cnt = 0;

  for (i = 0; i < Pic_cnt; i++) {
    j = 0;
    do {
      W25QXX.SPI_FLASH_BufferRead(&PIC.name[j], PIC_NAME_ADDR + tmp_cnt, 1);
      tmp_cnt++;
    } while (PIC.name[j++] != '\0' && j <= 50);  // fix-1025
    // pic size
    W25QXX.SPI_FLASH_BufferRead(PIC.size.bytes, PIC_SIZE_ADDR + i * 4, 4);

    if ((strcmp((char*)Pname, (char*)PIC.name)) == 0) {
      W25QXX.SPI_FLASH_BufferRead((uint8_t *)P_Rbuff, PIC_DATA_ADDR_TFT35 + i * PER_PIC_MAX_SPACE_TFT35, PIC.size.dwords);
      break;
    }
  }
}

void lv_pic_test(uint8_t *P_Rbuff, uint32_t addr, uint32_t size) {
  #if HAS_SPI_FLASH_COMPRESSION
    if (currentFlashPage == 0)
      SPIFlash.beginRead(addr);
    SPIFlash.readData(P_Rbuff, size);
    currentFlashPage++;
  #else
    W25QXX.init(SPI_HALF_SPEED);
    W25QXX.SPI_FLASH_BufferRead((uint8_t *)P_Rbuff, addr, size);
  #endif
}

#if HAS_SPI_FLASH_FONT
  void get_spi_flash_data(const char *rec_buf, int addr, int size) {
    W25QXX.init(SPI_HALF_SPEED);
    W25QXX.SPI_FLASH_BufferRead((uint8_t *)rec_buf, UNIGBK_FLASH_ADDR + addr, size);
  }

  void get_spi_flash_data_12(const char *rec_buf, int addr, int size) {
    W25QXX.init(SPI_HALF_SPEED);
    W25QXX.SPI_FLASH_BufferRead((uint8_t *)rec_buf, UNIGBK_FLASH_ADDR_12 + addr, size);
  }

  void get_spi_flash_data_16(const char *rec_buf, int addr, int size) {
    W25QXX.init(SPI_HALF_SPEED);
    W25QXX.SPI_FLASH_BufferRead((uint8_t *)rec_buf, UNIGBK_FLASH_ADDR_16 + addr, size);
  }

  void get_spi_flash_data_22(const char *rec_buf, int addr, int size) {
    W25QXX.init(SPI_HALF_SPEED);
    W25QXX.SPI_FLASH_BufferRead((uint8_t *)rec_buf, UNIGBK_FLASH_ADDR_22 + addr, size);
  }

  void get_spi_flash_data_28(const char *rec_buf, int addr, int size) {
    W25QXX.init(SPI_HALF_SPEED);
    W25QXX.SPI_FLASH_BufferRead((uint8_t *)rec_buf, UNIGBK_FLASH_ADDR_28 + addr, size);
  }
#endif

uint32_t logo_addroffset = 0;
void Pic_Logo_Read(uint8_t *LogoName, uint8_t *Logo_Rbuff, uint32_t LogoReadsize) {
  W25QXX.init(SPI_HALF_SPEED);
  W25QXX.SPI_FLASH_BufferRead(Logo_Rbuff, PIC_LOGO_ADDR + logo_addroffset, LogoReadsize);
  logo_addroffset += LogoReadsize;
  if (logo_addroffset >= LOGO_MAX_SIZE_TFT35)
    logo_addroffset = 0;
}

uint32_t default_view_addroffset = 0;
void default_view_Read(uint8_t *default_view_Rbuff, uint32_t default_view_Readsize) {
  W25QXX.init(SPI_HALF_SPEED);
  W25QXX.SPI_FLASH_BufferRead(default_view_Rbuff, DEFAULT_VIEW_ADDR_TFT35 + default_view_addroffset, default_view_Readsize);
  default_view_addroffset += default_view_Readsize;
  if (default_view_addroffset >= DEFAULT_VIEW_MAX_SIZE)
    default_view_addroffset = 0;
}

#if HAS_BAK_VIEW_IN_FLASH
  uint32_t flash_view_addroffset = 0;
  void flash_view_Read(uint8_t *flash_view_Rbuff, uint32_t flash_view_Readsize) {
    W25QXX.init(SPI_HALF_SPEED);
    W25QXX.SPI_FLASH_BufferRead(flash_view_Rbuff, BAK_VIEW_ADDR_TFT35 + flash_view_addroffset, flash_view_Readsize);
    flash_view_addroffset += flash_view_Readsize;
    if (flash_view_addroffset >= FLASH_VIEW_MAX_SIZE)
      flash_view_addroffset = 0;
  }
#endif

#endif // HAS_TFT_LVGL_UI
