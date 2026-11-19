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

//
// PIC32MX795 CNC Pin Assignments
// 4 or 5 stepper motors + SD card SPI
//

// ---------------------------------------
// Stepper Motors (4 or 5 axes)
// ---------------------------------------
// X axis pins
#ifndef X_STEP_PIN
  #define X_STEP_PIN             22
#endif
#ifndef X_DIR_PIN
  #define X_DIR_PIN              23
#endif
#ifndef X_ENABLE_PIN
  #define X_ENABLE_PIN           24
#endif

// Y axis pins
#ifndef Y_STEP_PIN
  #define Y_STEP_PIN             25
#endif
#ifndef Y_DIR_PIN
  #define Y_DIR_PIN              26
#endif
#ifndef Y_ENABLE_PIN
  #define Y_ENABLE_PIN           27
#endif

// Z axis pins
#ifndef Z_STEP_PIN
  #define Z_STEP_PIN             28
#endif
#ifndef Z_DIR_PIN
  #define Z_DIR_PIN              29
#endif
#ifndef Z_ENABLE_PIN
  #define Z_ENABLE_PIN           30
#endif

// A/4th axis pins (optional 5th motor)
#ifndef A_STEP_PIN
  #define A_STEP_PIN             31
#endif
#ifndef A_DIR_PIN
  #define A_DIR_PIN              32
#endif
#ifndef A_ENABLE_PIN
  #define A_ENABLE_PIN           33
#endif

// ---------------------------------------
// Endstops
// ---------------------------------------
#ifndef X_MIN_PIN
  #define X_MIN_PIN              34
#endif
#ifndef X_MAX_PIN
  #define X_MAX_PIN              35
#endif
#ifndef Y_MIN_PIN
  #define Y_MIN_PIN              36
#endif
#ifndef Y_MAX_PIN
  #define Y_MAX_PIN              37
#endif
#ifndef Z_MIN_PIN
  #define Z_MIN_PIN              38
#endif
#ifndef Z_MAX_PIN
  #define Z_MAX_PIN              39
#endif

// ---------------------------------------
// Temperature sensing (thermistors)
// ---------------------------------------
#ifndef TEMP_0_PIN
  #define TEMP_0_PIN             3  // ADC0
#endif
#ifndef TEMP_BED_PIN
  #define TEMP_BED_PIN           4  // ADC1
#endif

// ---------------------------------------
// SPI for microSD card
// ---------------------------------------
#ifndef SD_DETECT_PIN
  #define SD_DETECT_PIN          -1  // No detect pin (optional)
#endif

#ifndef SD_SS_PIN
  #define SD_SS_PIN              10  // CS pin for SD card SPI
#endif

#ifndef SD_SCK_PIN
  #define SD_SCK_PIN             11  // SPI1 SCK
#endif

#ifndef SD_MOSI_PIN
  #define SD_MOSI_PIN            12  // SPI1 SDO
#endif

#ifndef SD_MISO_PIN
  #define SD_MISO_PIN            13  // SPI1 SDI
#endif

// ---------------------------------------
// Control pins
// ---------------------------------------
#ifndef FAN_PIN
  #define FAN_PIN                5
#endif

#ifndef COOLANT_MIST_PIN
  #define COOLANT_MIST_PIN       6
#endif

#ifndef SPINDLE_ENABLE_PIN
  #define SPINDLE_ENABLE_PIN     7
#endif

#ifndef SPINDLE_SPEED_PIN
  #define SPINDLE_SPEED_PIN      8
#endif

// ---------------------------------------
// Probe/Z probe
// ---------------------------------------
#ifndef PROBE_PIN
  #define PROBE_PIN              34  // Shared with X_MIN or separate
#endif

// ---------------------------------------
// LCD/Display
// ---------------------------------------
#ifndef LCD_PINS_RS
  #define LCD_PINS_RS            -1
#endif
#ifndef LCD_PINS_ENABLE
  #define LCD_PINS_ENABLE        -1
#endif
#ifndef LCD_PINS_D4
  #define LCD_PINS_D4            -1
#endif
#ifndef LCD_PINS_D5
  #define LCD_PINS_D5            -1
#endif
#ifndef LCD_PINS_D6
  #define LCD_PINS_D6            -1
#endif
#ifndef LCD_PINS_D7
  #define LCD_PINS_D7            -1
#endif

// ---------------------------------------
// AUXILIARY pins
// ---------------------------------------
#ifndef USER_BUTTON_PIN
  #define USER_BUTTON_PIN        37
#endif

#ifndef LED_PIN
  #define LED_PIN                13  // On-board LED
#endif