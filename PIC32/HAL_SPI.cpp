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
#include "../platforms.h"

#if defined(ARDUINO_ARCH_PIC32)

#include "../inc/MarlinConfig.h"

#include <SPI.h>

// ------------------------
// Public Variables
// ------------------------

// SPI configuration state
static uint32_t spiClock = 0;
static uint8_t spiBitOrder = MSBFIRST;
static uint8_t spiDataMode = SPI_MODE0;

// ------------------------
// Public functions
// ------------------------

void spiBegin(void) {
  // Initialize SPI bus - Arduino PIC32 core handles this
  // SD card chip select is typically handled separately
  SPI.begin();
}

void spiInit(uint8_t spiRate) {
  // Configure SPI speed for SD card or other SPI devices
  // PIC32 MX795: F_PB is typically F_CPU/2 = 20MHz at 40MHz system clock
  
  // Map Marlin SPI rates to PIC32 BRG values
  // SPI baud rate = F_PB / (2 * (BRG + 1))
  // BRG = (F_PB / (2 * target_baud)) - 1
  
  uint32_t targetBaud;
  uint16_t brg;
  
  switch (spiRate) {
    case SPI_FULL_SPEED:
      targetBaud = 8000000L;  // 8 MHz
      brg = ((F_CPU / 2) / targetBaud) - 1;
      break;
    case SPI_HALF_SPEED:
      targetBaud = 4000000L;  // 4 MHz
      brg = ((F_CPU / 2) / targetBaud) - 1;
      break;
    case SPI_QUARTER_SPEED:
      targetBaud = 2000000L;  // 2 MHz
      brg = ((F_CPU / 2) / targetBaud) - 1;
      break;
    case SPI_EIGHTH_SPEED:
      targetBaud = 1000000L;  // 1 MHz
      brg = ((F_CPU / 2) / targetBaud) - 1;
      break;
    case SPI_SIXTEENTH_SPEED:
      targetBaud = 500000L;   // 500 kHz
      brg = ((F_CPU / 2) / targetBaud) - 1;
      break;
    case SPI_SPEED_5:
      targetBaud = 250000L;   // 250 kHz
      brg = ((F_CPU / 2) / targetBaud) - 1;
      break;
    case SPI_SPEED_6:
      targetBaud = 125000L;   // 125 kHz
      brg = ((F_CPU / 2) / targetBaud) - 1;
      break;
    default:
      targetBaud = 4000000L;
      brg = 4;  // F_PB/10 ~4MHz
      break;
  }
  
  // Configure SPI1 BRG register
  // SPI1BRG = (F_PB / (2 * targetBaud)) - 1
  SPI1BRG = brg;
  
  // Enable SPI
  SPI1CON = 0x8000;  // SPI enabled, master mode
  
  // Set clock polarity and phase based on mode
  // Default to Mode 0 (CKE=0, CKP=0)
  SPI1CON &= ~(1 << 15);  // Clear EOF bit
  SPI1CON &= ~(1 << 14);  // Clear SMP bit (sample at middle)
  
  // Set bit order
  // (handled in spiBeginTransaction)
}

void spiSend(uint8_t b) {
  // Write a byte to SPI
  // Wait for transmit buffer to be empty
  while (!(SPI1STAT & (1 << 1))) {  // TXBFF = transmit buffer full flag
    // wait
  }
  SPI1BUF = b;
  
  // Wait for transmission complete
  while (SPI1STAT & (1 << 1)) {  // wait for TXBFF to clear
    // wait
  }
}

uint8_t spiRec() {
  // Read a byte from SPI
  // Start transmission by writing a dummy byte
  SPI1BUF = 0xFF;
  
  // Wait for reception complete
  while (!(SPI1STAT & (1 << 0))) {  // RXBF = receive buffer full flag
    // wait
  }
  
  return SPI1BUF;
}

void spiRead(uint8_t *buf, uint16_t nbyte) {
  for (uint16_t i = 0; i < nbyte; i++) {
    buf[i] = spiRec();
  }
}

void spiSendBlock(uint8_t token, const uint8_t *buf) {
  // SD card write block: token + 512 bytes data + 2 bytes CRC
  spiSend(token);
  
  if (buf) {
    spiRead((uint8_t *)buf, 512);
  }
  
  // Send CRC-16 bytes (required by SD protocol)
  spiSend(0xFF);
  spiSend(0xFF);
}

void spiBeginTransaction(uint32_t spiClock, uint8_t bitOrder, uint8_t dataMode) {
  // Set SPI transaction parameters
  this->spiClock = spiClock;
  this->spiBitOrder = bitOrder;
  this->spiDataMode = dataMode;
  
  // Configure SPI1
  // Clear control register
  SPI1CON = 0;
  
  // Set bit order
  if (bitOrder == LSBFIRST) {
    SPI1CONSET = (1 << 11);  // LPCON = 1 for LSB first
  } else {
    SPI1CONCLR = (1 << 11);  // LPCON = 0 for MSB first
  }
  
  // Set data mode (CPOL/CPHA)
  // Mode 0: CKE=0, CKP=0 (clock idle low, data valid on rising edge)
  // Mode 1: CKE=1, CKP=0 (clock idle low, data valid on falling edge)
  // Mode 2: CKE=0, CKP=1 (clock idle high, data valid on rising edge)
  // Mode 3: CKE=1, CKP=1 (clock idle high, data valid on falling edge)
  
  switch (dataMode) {
    case 0: // Mode 0: CKE=0, CKP=0
      SPI1CONCLR = (1 << 10) | (1 << 15);  // Clear CKE, CKP
      break;
    case 1: // Mode 1: CKE=1, CKP=0
      SPI1CONSET = (1 << 10);  // Set CKE
      SPI1CONCLR = (1 << 15);  // Clear CKP
      break;
    case 2: // Mode 2: CKE=0, CKP=1
      SPI1CONCLR = (1 << 10);  // Clear CKE
      SPI1CONSET = (1 << 15);  // Set CKP
      break;
    case 3: // Mode 3: CKE=1, CKP=1
      SPI1CONSET = (1 << 10) | (1 << 15);  // Set CKE, CKP
      break;
  }
  
  // Set baud rate
  uint16_t brg = ((F_CPU / 2) / spiClock) - 1;
  if (brg > 0xFFFF) brg = 0xFFFF;
  SPI1BRG = brg;
  
  // Enable SPI module
  SPI1CONSET = (1 << 15);  // SPIEN = 1
  
  // Wait for SPI to stabilize
  delayMicroseconds(1);
}
#else
// Fallback for non-PIC32 builds
void spiBegin(void) {}
void spiInit(uint8_t spiRate) {}
void spiSend(uint8_t b) {}
uint8_t spiRec() { return 0; }
void spiRead(uint8_t *buf, uint16_t nbyte) {}
void spiSendBlock(uint8_t token, const uint8_t *buf) {}
void spiBeginTransaction(uint32_t spiClock, uint8_t bitOrder, uint8_t dataMode) {}
#endif