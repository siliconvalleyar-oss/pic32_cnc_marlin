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

#include "Marduino.h"

//
// PIC32-specific types and defines
//

// Configuration bits and clock defines
#define _GET_CONFIG1(reg)      (*(unsigned far int *)(0xF80000C0))
#define _GET_CONFIG2(reg)      (*(unsigned far int *)(0xF80000C2))

// Oscillator defaults (adjust for your specific oscillator configuration)
#ifndef F_CPU
  #define F_CPU 40000000UL  // Default 40MHz for PIC32MX795
#endif

// ----------------------------------------------------------------------------
// SPI
// ----------------------------------------------------------------------------

// SPI speed definitions (approximate)
// On PIC32, SPI clock = F_PB / (spiClkDiv * 2)
// spiClkDiv values: 2=F_PB/4, 3=F_PB/6, 4=F_PB/8, etc.

#define SPI_FULL_SPEED      0
#define SPI_HALF_SPEED      1
#define SPI_QUARTER_SPEED   2
#define SPI_EIGHTH_SPEED    3
#define SPI_SIXTEENTH_SPEED 4
#define SPI_SPEED_5         5
#define SPI_SPEED_6         6

// SPI function declarations (will be implemented in HAL_SPI.cpp)
void spiBegin();
void spiInit(uint8_t spiRate);
void spiSend(uint8_t b);
uint8_t spiRec();
void spiRead(uint8_t *buf, uint16_t nbyte);
void spiSendBlock(uint8_t token, const uint8_t *buf);
void spiBeginTransaction(uint32_t spiClock, uint8_t bitOrder, uint8_t dataMode);

// ----------------------------------------------------------------------------
// Timer system for stepper motors
// ----------------------------------------------------------------------------

// PIC32 timer defines
#define TIMER_STEPPER TIMER_1  // Use Timer 1 for stepper timing
#define TIMER_SERIAL  TIMER_2  // Use Timer 2 for serial timing (if needed)
#define TIMER_SERVO   TIMER_3  // Use Timer 3 for servo timing

// Stepper timer constants (will be calibrated based on F_PB)
#define STEPPER_TIMER_PRESCALER  8
#define STEPPER_TIMER_PERIOD      10  // Adjust for desired step rate

// ----------------------------------------------------------------------------
// ADC for temperature sensing
// ----------------------------------------------------------------------------

#define HAL_ADC_RESOLUTION 10  // PIC10/12-bit ADC on PIC32MX

// ----------------------------------------------------------------------------
// Fast GPIO operations
// ----------------------------------------------------------------------------

// PIC32 port/pin macros - these will be defined per-board in pins/pic32/
// The Arduino core provides digitalWrite/digitalRead, but we need FastIO-style
// direct register access for stepper ISR speed.

// ----------------------------------------------------------------------------
// UART/Serial
// ----------------------------------------------------------------------------

// PIC32 UART modules - USART2/3/4/5/6 available on MX795
// Serial baud rate support will use the Arduino core's Serial class

// ----------------------------------------------------------------------------
// EEPROM
// ----------------------------------------------------------------------------

// PIC32 MX795 has 512KB Flash, EEPROM emulation may use flash pages
// or external SPI EEPROM. Default to SRAM-based for now.

// ----------------------------------------------------------------------------
// Include the family-specific implementation
// ----------------------------------------------------------------------------

#include HAL_PATH(.., HAL/PIC32/HAL.cpp)

extern MarlinHAL hal;

#endif // PIC32_HAL_H