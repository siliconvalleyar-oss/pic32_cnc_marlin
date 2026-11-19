# API Reference - PIC32 CNC HAL

## Overview
This document describes the Hardware Abstraction Layer (HAL) API for the PIC32MX795-based CNC firmware. The HAL provides a unified interface for GPIO, SPI, timers, ADC, and other hardware peripherals.

---

## GPIO Functions

### `gpioPinMode(uint8_t pin, uint8_t mode)`
Configure a GPIO pin as input or output.

**Parameters:**
- `pin` - Pin number (0-31 for PORTB)
- `mode` - 0 = OUTPUT, 1 = INPUT

**Implementation:** Direct register access to TRISB register.

```c
void gpioPinMode(uint8_t pin, uint8_t mode) {
    if (pin < 32) {
        uint32_t *tris = (uint32_t *)TRISB;
        uint32_t bit = 1 << pin;
        if (mode == 0) {
            *tris &= ~bit;  // Set as output
        } else {
            *tris |= bit;   // Set as input
        }
    }
}
```

### `gpioWrite(uint8_t pin, uint8_t value)`
Write digital value to a GPIO pin.

**Parameters:**
- `pin` - Pin number (0-31)
- `value` - 1 = HIGH, 0 = LOW

**Implementation:** Direct register access to LATB register.

```c
void gpioWrite(uint8_t pin, uint8_t value) {
    if (pin < 32) {
        if (value) {
            LATB |= (1 << pin);
        } else {
            LATB &= ~(1 << pin);
        }
    }
}
```

### `gpioRead(uint8_t pin)`
Read digital value from a GPIO pin.

**Parameters:**
- `pin` - Pin number (0-31)

**Returns:** 1 = HIGH, 0 = LOW

**Implementation:** Direct register access to PORTB register.

```c
uint8_t gpioRead(uint8_t pin) {
    if (pin < 32) {
        return (PORTB & (1 << pin)) ? 1 : 0;
    }
    return 0;
}
```

---

## SPI Functions

### `spiBegin(void)`
Initialize SPI module for SD card communication.

**Configuration:**
- Master mode enabled
- Clock divider set for ~10 MHz (SPI1BRG = 4)
- Mode 0 (CPOL=0, CPHA=0)
- MSB first

```c
void spiBegin(void) {
    gpioPinMode(SD_CS_PIN, 0);   // CS as output
    gpioPinMode(SD_SCK_PIN, 0);  // SCK as output
    gpioPinMode(SD_MOSI_PIN, 0); // MOSI as output
    gpioPinMode(SD_MISO_PIN, 1); // MISO as input
    
    SPI1CON = 0x8000;    // Enable SPI, master mode
    SPI1CONCLR = 0x3000; // Clear CKE/CKP bits (Mode 0)
    SPI1BRG = 4;         // Baud rate generator
}
```

### `spiSendReceive(uint8_t b)`
Send and receive a single byte via SPI.

**Parameters:**
- `b` - Byte to transmit

**Returns:** Received byte

```c
uint8_t spiSendReceive(uint8_t b) {
    SPI1BUF = b;
    while (!(SPI1STAT & 0x01));  // Wait for RXBF (Receive Buffer Full)
    return SPI1BUF;
}
```

### `spiSend(uint8_t b)`
Send a byte via SPI (discards received byte).

### `spiRec(void)`
Receive a byte via SPI (sends 0xFF dummy byte).

### `spiSendBlock(uint8_t token, const uint8_t *buf)`
Send a 512-byte block with token and CRC (SD card protocol).

**Parameters:**
- `token` - Start token (e.g., 0xFE for single block write)
- `buf` - Pointer to 512-byte data buffer

### `spiBeginTransaction(uint32_t spiClock, uint8_t bitOrder, uint8_t dataMode)`
Configure SPI transaction parameters.

**Parameters:**
- `spiClock` - Target clock frequency in Hz
- `bitOrder` - MSBFIRST or LSBFIRST
- `dataMode` - SPI_MODE0 through SPI_MODE3

**Calculates BRG:** `brg = (F_CPU / 2 / spiClock) - 1`

---

## Timer Functions

### `stepperTimerInit(void)`
Initialize stepper timer (placeholder - to be implemented).

### `delayMs(uint32_t ms)`
Blocking delay using CPU cycle counter.

```c
void delayMs(uint32_t ms) {
    uint32_t start = _CP0_GET_COUNT();
    uint32_t target = start + (F_CPU / 1000 * ms);
    while (_CP0_GET_COUNT() < target) {}
}
```

---

## ADC Functions

### `adcRead(uint8_t channel)`
Read analog value from ADC channel (stub implementation).

**Returns:** 16-bit ADC value (currently returns 0)

---

## HAL Lifecycle

### `halInit(void)`
Initialize all HAL subsystems.

```c
void halInit(void) {
    spiBegin();  // Initialize SPI for SD card
}
```

### `halTask(void)`
Main HAL task loop (currently empty - placeholder for background tasks).

---

## Register Definitions

### SPI1 Registers (PIC32MX795)
| Register | Address | Description |
|----------|---------|-------------|
| SPI1CON | 0xBF801000 | SPI1 Control Register |
| SPI1BUF | 0xBF801004 | SPI1 Buffer Register |
| SPI1BRG | 0xBF801018 | SPI1 Baud Rate Generator |
| SPI1STAT | 0xBF80100C | SPI1 Status Register |

### PORTB Registers
| Register | Address | Description |
|----------|---------|-------------|
| TRISB | 0xBF886004 | PORTB Direction Control |
| PORTB | 0xBF802004 | PORTB Input Data |
| LATB | 0xBF802010 | PORTB Output Latch |

---

## Pin Definitions (Default)

| Pin | Function | Port |
|-----|----------|------|
| 0 | SD_CS | RB0 |
| 1 | SD_SCK | RB1 |
| 2 | SD_MOSI | RB2 |
| 3 | SD_MISO | RB3 |

---

## SPI Mode Constants

| Mode | CPOL | CPHA | Description |
|------|------|------|-------------|
| SPI_MODE0 | 0 | 0 | Clock idle low, sample on rising edge |
| SPI_MODE1 | 0 | 1 | Clock idle low, sample on falling edge |
| SPI_MODE2 | 1 | 0 | Clock idle high, sample on rising edge |
| SPI_MODE3 | 1 | 1 | Clock idle high, sample on falling edge |

---

## Usage Example

```c
#include "HAL.h"

int main(void) {
    halInit();
    
    // Configure stepper pins
    gpioPinMode(X_STEP_PIN, 0);  // Output
    gpioPinMode(X_DIR_PIN, 0);   // Output
    gpioPinMode(X_ENABLE_PIN, 0); // Output
    
    // Enable X axis
    gpioWrite(X_ENABLE_PIN, 0);  // Active low
    
    // Step pulse
    gpioWrite(X_STEP_PIN, 1);
    delayMs(1);
    gpioWrite(X_STEP_PIN, 0);
    
    for (;;) {
        halTask();
    }
    return 0;
}
```

---

## Notes

1. **Direct Register Access**: This HAL uses direct register manipulation for maximum performance, bypassing Arduino-style abstractions.

2. **PIC32MX795 Specific**: Register addresses are specific to the PIC32MX795F512L microcontroller.

3. **SPI Speed**: Default SPI1BRG = 4 gives approximately 10 MHz at 80 MHz F_CPU.

4. **EXTENDING**: Additional peripherals (UART, I2C, PWM, ADC) need implementation in HAL.cpp/HAL.h.

5. **INTERRUPTS**: Current implementation is polling-based. For production CNC, timer interrupts for stepper control should be added.