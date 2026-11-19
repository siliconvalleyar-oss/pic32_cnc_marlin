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
#include "HAL.h"

// ----------------------------------------------------------------------------
// Configuration
// ----------------------------------------------------------------------------

// Oscillator configuration - PIC32MX795 default often uses PLL to reach 40MHz or 80MHz
// F_CPU should match the actual system clock frequency

// ----------------------------------------------------------------------------
// SPI Implementation for PIC32
// ----------------------------------------------------------------------------

// PIC32 uses the SPI1 module (and SPI2 for some pins)
// The Arduino PIC32 core wraps SPI in SPI1 objects

#if !defined(SPI_HARDWARE)
  #define SPI_HARDWARE SPI
#endif

static SPISettings spiConfig;

void spiBegin() {
  // Initialize SPI module
  // On PIC32MX, SPI1 is typically used
  // The Arduino core handles SPI.begin() setup
  SPI.begin();
}

void spiInit(uint8_t spiRate) {
  // Configure SPI speed
  // PIC32 SPI clock = F_PB / (spiClkDiv * 2)
  // spiClkDiv: 2 -> F_PB/4 (fastest), 3 -> F_PB/6, 4 -> F_PB/8, etc.
  
  uint32_t targetClock;
  uint16_t spiClkDiv;
  
  switch (spiRate) {
    case SPI_FULL_SPEED:
      targetClock = 8000000L;  // 8 MHz
      spiClkDiv = 2;           // F_PB/4
      break;
    case SPI_HALF_SPEED:
      targetClock = 4000000L;  // 4 MHz
      spiClkDiv = 3;           // F_PB/6
      break;
    case SPI_QUARTER_SPEED:
      targetClock = 2000000L;  // 2 MHz
      spiClkDiv = 4;           // F_PB/8
      break;
    case SPI_EIGHTH_SPEED:
      targetClock = 1000000L;  // 1 MHz
      spiClkDiv = 6;           // F_PB/12
      break;
    case SPI_SIXTEENTH_SPEED:
      targetClock = 500000L;   // 500 kHz
      spiClkDiv = 8;           // F_PB/16
      break;
    case SPI_SPEED_5:
      targetClock = 250000L;   // 250 kHz
      spiClkDiv = 12;          // F_PB/24
      break;
    case SPI_SPEED_6:
      targetClock = 125000L;   // 125 kHz
      spiClkDiv = 16;          // F_PB/32
      break;
    default:
      targetClock = 1000000L;
      spiClkDiv = 4;
      break;
  }
  
  // Configure SPI clock divider
  // On PIC32, the register is SPI1BRG
  // SPI baud rate = F_PB / (2 * (SPI1BRG + 1))
  // So SPI1BRG = (F_PB / (2 * targetClock)) - 1
  
  if (spiClkDiv <= 8) {
    // Direct divider values
    SPI_HARDWARE.beginDivider(spiClkDiv);
  } else {
    // Calculate BRG register value
    uint32_t brg = (F_CPU / (2 * targetClock)) - 1;
    if (brg > 0xFFFF) brg = 0xFFFF;
    SPI_HARDWARE.setClockDivider(brg);
  }
  
  spiConfig = SPISettings(targetClock, MSBFIRST, SPI_MODE0);
}

void spiSend(uint8_t b) {
  SPI.transfer(b);
}

uint8_t spiRec() {
  return SPI.transfer(0xFF);
}

void spiRead(uint8_t *buf, uint16_t nbyte) {
  for (uint16_t i = 0; i < nbyte; i++) {
    buf[i] = SPI.transfer(0xFF);
  }
}

void spiSendBlock(uint8_t token, const uint8_t *buf) {
  // SD card write block token + data
  // First send the token, then the 512-byte data block with CRC
  spiSend(token);
  
  if (buf) {
    spiRead(buf, 512);
  }
  
  // Send 2 CRC bytes (required by SD protocol)
  spiSend(0xFF);
  spiSend(0xFF);
}

void spiBeginTransaction(uint32_t spiClock, uint8_t bitOrder, uint8_t dataMode) {
  // Set SPI transaction parameters
  // PIC32 SPI modes: 
  // Mode 0: CPOL=0, CPHA=0 (clock idle low, sample at leading edge)
  // Mode 1: CPOL=0, CPHA=1 (clock idle low, sample at trailing edge)
  // Mode 2: CPOL=1, CPHA=0 (clock idle high, sample at leading edge)
  // Mode 3: CPOL=1, CPHA=1 (clock idle high, sample at trailing edge)
  
  uint8_t mode;
  switch (dataMode) {
    case SPI_MODE0: mode = 0; break; // CPOL=0, CPHA=0
    case SPI_MODE1: mode = 1; break; // CPOL=0, CPHA=1
    case SPI_MODE2: mode = 2; break; // CPOL=1, CPHA=0
    case SPI_MODE3: mode = 3; break; // CPOL=1, CPHA=1
    default: mode = 0;
  }
  
  // Set bit order
  if (bitOrder == LSBFIRST) {
    SPI.setBitOrder(LSBFIRST);
  } else {
    SPI.setBitOrder(MSBFIRST);
  }
  
  // Set data mode
  SPI.setDataMode(mode);
  
  // Set clock speed
  spiInit(SPI_FULL_SPEED);  // Will be overridden by actual clock setting
  // Note: SPI.beginTransaction takes clock speed in Hz
  // We'll set the divider based on the requested clock
  spiClock = spiClock;  // Store for reference
}