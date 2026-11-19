#include <xc.h>
#include <stdint.h>

// PIC32MX795 Configuration
#ifndef F_CPU
#define F_CPU 80000000UL
#endif

// Pin definitions
#define SD_CS_PIN 0
#define SD_SCK_PIN 1
#define SD_MOSI_PIN 2
#define SD_MISO_PIN 3

// Pin masks
#define PIN0_mask (1 << 0)
#define PIN1_mask (1 << 1)
#define PIN2_mask (1 << 2)
#define PIN3_mask (1 << 3)

// HAL Implementation - Direct Register Access

void gpioPinMode(uint8_t pin, uint8_t mode) {
    if (pin < 32) {
        uint32_t *tris = (uint32_t *)TRISB;
        uint32_t bit = 1 << pin;
        if (mode == 0) {
            *tris &= ~bit;
        } else {
            *tris |= bit;
        }
    }
}

void gpioWrite(uint8_t pin, uint8_t value) {
    if (pin < 32) {
        if (value) {
            LATB |= (1 << pin);
        } else {
            LATB &= ~(1 << pin);
        }
    }
}

uint8_t gpioRead(uint8_t pin) {
    if (pin < 32) {
        return (PORTB & (1 << pin)) ? 1 : 0;
    }
    return 0;
}

// SPI Functions

void spiBegin(void) {
    gpioPinMode(SD_CS_PIN, 0);
    gpioPinMode(SD_SCK_PIN, 0);
    gpioPinMode(SD_MOSI_PIN, 0);
    gpioPinMode(SD_MISO_PIN, 1);
    
    SPI1CON = 0x8000;
    SPI1CONCLR = 0x3000;
    SPI1BRG = 4;
}

uint8_t spiSendReceive(uint8_t b) {
    SPI1BUF = b;
    while (!(SPI1STAT & 0x01));
    return SPI1BUF;
}

void spiSend(uint8_t b) {
    spiSendReceive(b);
}

uint8_t spiRec() {
    return spiSendReceive(0xFF);
}

void spiSendBlock(uint8_t token, const uint8_t *buf) {
    spiSend(token);
    if (buf) {
        for (uint16_t i = 0; i < 512; i++) {
            spiSend(buf[i]);
        }
    }
    spiSend(0xFF);
    spiSend(0xFF);
}

void spiBeginTransaction(uint32_t spiClock, uint8_t bitOrder, uint8_t dataMode) {
    SPI1CON = 0x8000;
    SPI1CONCLR = 0x3000;
    uint16_t brg = (F_CPU / 2 / spiClock) - 1;
    if (brg > 0xFFFF) brg = 0xFFFF;
    SPI1BRG = brg;
}

void delayMs(uint32_t ms) {
    uint32_t start = _CP0_GET_COUNT();
    uint32_t target = start + (F_CPU / 1000 * ms);
    while (_CP0_GET_COUNT() < target) {}
}

void stepperTimerInit(void) {}

uint16_t adcRead(uint8_t channel) {
    return 0;
}

void halInit(void) {
    spiBegin();
}

void halTask(void) {}