#ifndef HAL_H
#define HAL_H

// Types and defines for PIC32MX795 direct register access

// CPU frequency
#ifndef F_CPU
#define F_CPU 80000000UL
#endif

// Types
typedef uint8_t uint8_t;
typedef uint16_t uint16_t;
typedef uint32_t uint32_t;

// HAL data types
typedef struct {
    uint8_t mode;
    uint8_t configured;
} hal_spi_config_t;

// Function prototypes
void halInit(void);
void halTask(void);
void spiBegin(void);
uint8_t spiSend(uint8_t b);
uint8_t spiRec();
void spiRead(uint8_t *buf, uint16_t nbyte);
void spiSendBlock(uint8_t token, const uint8_t *buf);
void spiBeginTransaction(uint32_t spiClock, uint8_t bitOrder, uint8_t dataMode);

// GPIO functions
void gpioPinMode(uint8_t pin, uint8_t mode);
void gpioWrite(uint8_t pin, uint8_t value);
uint8_t gpioRead(uint8_t pin);

// Register Direct Access Definitions (PIC32MX795)

// SPI1 registers
#define SPI1CON *(volatile uint16_t *)0xBF801000
#define SPI1BUF *(volatile uint16_t *)0xBF801004
#define SPI1BRG *(volatile uint16_t *)0xBF801018

// Port B definitions
#define TRISB *(volatile uint32_t *)0xBF886000
#define PORTB *(volatile uint16_t *)0xBF802004
#define LATB *(volatile uint16_t *)0xBF802010

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

// Function Prototypes
void halInit(void);
void halTask(void);
void spiBegin(void);
uint8_t spiSend(uint8_t b);
uint8_t spiRec();
void spiRead(uint8_t *buf, uint16_t nbyte);
void spiSendBlock(uint8_t token, const uint8_t *buf);
void spiBeginTransaction(uint32_t spiClock, uint8_t bitOrder, uint8_t dataMode);

void gpioPinMode(uint8_t pin, uint8_t mode);
void gpioWrite(uint8_t pin, uint8_t value);
uint8_t gpioRead(uint8_t pin);

#endif // HAL_H
