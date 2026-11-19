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

#endif // HAL_H