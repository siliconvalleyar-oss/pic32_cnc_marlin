# PINOUT - PIC32MX795 CNC Pin Assignments

## Pin Assignment Table

### Stepper Motors (4-5 Axes)

| Axis | Signal | PIC32 Pin | Port | Pin # | Description |
|------|--------|-----------|------|-------|-------------|
| **X** | STEP | 22 | RB22 | 51 | Step pulse output |
| | DIR | 23 | RB23 | 52 | Direction output |
| | ENABLE | 24 | RB24 | 53 | Enable (active LOW) |
| **Y** | STEP | 25 | RB25 | 54 | Step pulse output |
| | DIR | 26 | RB26 | 55 | Direction output |
| | ENABLE | 27 | RB27 | 56 | Enable (active LOW) |
| **Z** | STEP | 28 | RB28 | 57 | Step pulse output |
| | DIR | 29 | RB29 | 58 | Direction output |
| | ENABLE | 30 | RB30 | 59 | Enable (active LOW) |
| **A** (4th) | STEP | 31 | RB31 | 60 | Step pulse output |
| | DIR | 32 | RC0 | 19 | Direction output |
| | ENABLE | 33 | RC1 | 20 | Enable (active LOW) |

### Endstops (6 Inputs)

| Axis | Signal | PIC32 Pin | Port | Pin # | Description |
|------|--------|-----------|------|-------|-------------|
| **X** | MIN | 34 | RC2 | 21 | X minimum endstop |
| | MAX | 35 | RC3 | 22 | X maximum endstop |
| **Y** | MIN | 36 | RC4 | 23 | Y minimum endstop |
| | MAX | 37 | RC5 | 24 | Y maximum endstop |
| **Z** | MIN | 38 | RC6 | 25 | Z minimum endstop |
| | MAX | 39 | RC7 | 26 | Z maximum endstop |

### Temperature Sensors (ADC Inputs)

| Signal | PIC32 Pin | Port | Pin # | ADC Channel | Description |
|--------|-----------|------|-------|-------------|-------------|
| TEMP_0 | 3 | AN0 | 2 | AN0 | Hotend/Spindle thermistor |
| TEMP_BED | 4 | AN1 | 3 | AN1 | Bed/Ambient thermistor |

### SD Card (SPI1)

| Signal | PIC32 Pin | Port | Pin # | Description |
|--------|-----------|------|-------|-------------|
| SD_SS | 10 | RB10 | 39 | Chip Select (CS) |
| SD_SCK | 11 | RB11 | 40 | Serial Clock (SCK) |
| SD_MOSI | 12 | RB12 | 41 | Master Out Slave In (SDO) |
| SD_MISO | 13 | RB13 | 42 | Master In Slave Out (SDI) |
| SD_DETECT | -1 | - | - | Card Detect (not connected) |

### Control Outputs

| Signal | PIC32 Pin | Port | Pin # | Description |
|--------|-----------|------|-------|-------------|
| FAN | 5 | RB5 | 14 | Flood coolant / Part cooling fan |
| COOLANT_MIST | 6 | RB6 | 15 | Mist coolant |
| SPINDLE_ENABLE | 7 | RB7 | 16 | Spindle enable (VFD) |
| SPINDLE_SPEED | 8 | RB8 | 17 | Spindle PWM (0-100%) |

### Probe / Auxiliary

| Signal | PIC32 Pin | Port | Pin # | Description |
|--------|-----------|------|-------|-------------|
| PROBE | 34 | RC2 | 21 | Touch probe (shared with X_MIN) |
| USER_BUTTON | 37 | RC5 | 24 | User button (shared with Y_MAX) |
| LED | 13 | RB13 | 42 | Status LED (shared with SD_MISO) |

### LCD/Display (Not Connected)

| Signal | PIC32 Pin | Description |
|--------|-----------|-------------|
| LCD_PINS_RS | -1 | Register Select |
| LCD_PINS_ENABLE | -1 | Enable |
| LCD_PINS_D4 | -1 | Data 4 |
| LCD_PINS_D5 | -1 | Data 5 |
| LCD_PINS_D6 | -1 | Data 6 |
| LCD_PINS_D7 | -1 | Data 7 |

---

## PIC32MX795F512L Pinout Diagram (100-pin TQFP)

```
       ┌────────────────────────────────────┐
       │          PIC32MX795F512L           │
       │            100-pin TQFP            │
       └────────────────────────────────────┘

  VDD ── 1 ── VSS
  PGED1/AN0/RB0 ── 2 ── 100 ── RB1/AN1/PGEC1
  RB2/AN2 ── 3 ── 99 ── RB3/AN3
  RB4/AN4 ── 4 ── 98 ── RB5/AN5      ← FAN (5)
  RB6/AN6 ── 5 ── 97 ── RB7/AN7      ← COOLANT_MIST (6)
  RB8/AN8 ── 6 ── 96 ── RB9/AN9      ← SPINDLE_ENABLE (7)
  RB10/AN10 ── 7 ── 95 ── RB11/AN11  ← SD_SS (10), SD_SCK (11)
  RB12/AN12 ── 8 ── 94 ── RB13/AN13  ← SD_MOSI (12), SD_MISO/LED (13)
  RB14/AN14 ── 9 ── 93 ── RB15/AN15  ← A_DIR (32)
  VSS ── 10 ── 92 ── VDD
  VCAP ── 11 ── 91 ── VDD
  VSS ── 12 ── 90 ── MCLR
  RB16 ── 13 ── 89 ── RB17           ← A_ENABLE (33)
  RB18 ── 14 ── 88 ── RB19
  RB20 ── 15 ── 87 ── RB21
  RB22 ── 16 ── 86 ── RB23           ← X_STEP (22), X_DIR (23)
  RB24 ── 17 ── 85 ── RB25           ← X_ENABLE (24), Y_STEP (25)
  RB26 ── 18 ── 84 ── RB27           ← Y_DIR (26), Y_ENABLE (27)
  RB28 ── 19 ── 83 ── RB29           ← Z_STEP (28), Z_DIR (29)
  RB30 ── 20 ── 82 ── RB31           ← Z_ENABLE (30), A_STEP (31)
  RC0 ── 21 ── 81 ── RC1
  RC2 ── 22 ── 80 ── RC3             ← X_MIN (34), X_MAX (35)
  RC4 ── 23 ── 79 ── RC5             ← Y_MIN (36), Y_MAX/PROBE/USER_BTN (37)
  RC6 ── 24 ── 78 ── RC7             ← Z_MIN (38), Z_MAX (39)
  RC8 ── 25 ── 77 ── RC9
  RC10 ── 26 ── 76 ── RC11
  VSS ── 27 ── 75 ── VDD
  VCAP ── 28 ── 74 ── VDD
  RD0 ── 29 ── 73 ── RD1
  RD2 ── 30 ── 72 ── RD3
  RD4 ── 31 ── 71 ── RD5
  RD6 ── 32 ── 70 ── RD7
  RD8 ── 33 ── 69 ── RD9
  RD10 ── 34 ── 68 ── RD11
  RD12 ── 35 ── 67 ── RD13
  RD14 ── 36 ── 66 ── RD15
  VSS ── 37 ── 65 ── VDD
  RE0 ── 38 ── 64 ── RE1
  RE2 ── 39 ── 63 ── RE3
  RE4 ── 40 ── 62 ── RE5
  RE6 ── 41 ── 61 ── RE7
  RF0 ── 42 ── 60 ── RF1
  RF2 ── 43 ── 59 ── RF3
  RF4 ── 44 ── 58 ── RF5
  RF6 ── 45 ── 57 ── RF7
  RG0 ── 46 ── 56 ── RG1
  RG2 ── 47 ── 55 ── RG3
  RG6 ── 48 ── 54 ── RG7
  RG8 ── 49 ── 53 ── RG9
  VSS ── 50 ── 52 ── VDD
                    51 ── MCLR (duplicate)
```

---

## Port Mapping Summary

| Port | Pins Used | Function |
|------|-----------|----------|
| **PORTB** | RB0-RB15, RB22-RB31 | Primary I/O (SPI, Steppers X/Y/Z, Control) |
| **PORTC** | RC0-RC7 | Stepper A, Endstops, Probe |
| **PORTD** | - | Available for expansion |
| **PORTE** | - | Available for expansion |
| **PORTF** | - | Available for expansion |
| **PORTG** | - | Available for expansion |

---

## Electrical Considerations

### Input Protection (Endstops, Probe)
```c
// Recommended external circuit for each endstop input:
// [Switch] --+-- [10kΩ] --+-- [PIC32 Pin]
//            |            |
//           [GND]       [3.3V]  (Pull-up)
//            |            |
//           [100nF]     [100nF]  (Debounce capacitor)
//            |            |
//           [GND]       [GND]

// For NO switches: Connect between pin and GND
// For NC switches: Connect between pin and 3.3V (with pull-down)
```

### Output Drive (Step/Dir/Enable)
```c
// Pololu A4988/DRV8825 interface:
// PIC32 (3.3V) ── [330Ω] ── STEP/DIR/ENABLE
//                          │
//                      [10kΩ] (pull-down on driver)
//                          │
//                         GND

// Note: 3.3V logic is compatible with Pololu drivers
// Minimum VIH = 2.0V, PIC32 VOH = 2.4V (at 3.3V VDD)
```

### Spindle PWM Output
```c
// SPINDLE_SPEED (RB8/OC4) → VFD/Spindle Controller
// 
// Option 1: Direct PWM (0-3.3V)
// PIC32 ── [1kΩ] ── VFD PWM Input
// 
// Option 2: Optocoupler isolation (recommended)
// PIC32 ── [330Ω] ── [Opto LED] ── [Opto Transistor] ── VFD
//                      │                    │
//                     GND                  VFD_GND/VFD_PWM
```

### Temperature Sensor Interface
```c
// Thermistor voltage divider:
// 3.3V ── [4.7kΩ] ──+── [Thermistor] ── GND
//                   │
//                [PIC32 ANx]
//                   │
//                [100nF] (noise filter)
//                   │
//                  GND

// ADC: 10-bit, 0-3.3V → 0-1023
// Typical: 100kΩ NTC, Beta=3950
```

---

## Pin Conflict Notes

| Pin | Conflict | Resolution |
|-----|----------|------------|
| RB13 | SD_MISO / LED | LED only active when SD not in use; or move LED |
| RC2 | X_MIN / PROBE | Probe uses X_MIN pin; configure one at a time |
| RC5 | Y_MAX / USER_BUTTON | Button on Y_MAX; use different pin for dedicated button |
| RB8 | SPINDLE_SPEED / AN8 | Analog input not used when PWM active |

---

## Expansion Possibilities (Unused Pins)

### Available for Additional Features:
- **PORTD** (RD0-RD15): 16 pins - Additional axes, I/O, LCD
- **PORTE** (RE0-RE7): 8 pins - Additional sensors, encoders
- **PORTF** (RF0-RF7): 8 pins - Additional SPI/I2C, PWM
- **PORTG** (RG0-RG9): 10 pins - Additional timers, UART

### Suggested Assignments:
| Feature | Suggested Pins |
|---------|----------------|
| 5th Axis (B) | RD0-RD2 (STEP/DIR/ENABLE) |
| 6th Axis (C) | RD3-RD5 (STEP/DIR/ENABLE) |
| MPG/Handwheel | RE0, RE1 (Quad encoder) |
| I2C Expansion | RF4 (SDA), RF5 (SCL) |
| Second SPI | RF0 (SCK), RF1 (SDO), RF2 (SDI) |
| UART1 (Debug) | RG8 (TX), RG7 (RX) |

---

## Pin Definition Macros (pins_pic32mx795.h)

```c
// Stepper Motors
#define X_STEP_PIN      22
#define X_DIR_PIN       23
#define X_ENABLE_PIN    24

#define Y_STEP_PIN      25
#define Y_DIR_PIN       26
#define Y_ENABLE_PIN    27

#define Z_STEP_PIN      28
#define Z_DIR_PIN       29
#define Z_ENABLE_PIN    30

#define A_STEP_PIN      31
#define A_DIR_PIN       32
#define A_ENABLE_PIN    33

// Endstops
#define X_MIN_PIN       34
#define X_MAX_PIN       35
#define Y_MIN_PIN       36
#define Y_MAX_PIN       37
#define Z_MIN_PIN       38
#define Z_MAX_PIN       39

// Temperature
#define TEMP_0_PIN       3   // AN0
#define TEMP_BED_PIN     4   // AN1

// SPI / SD Card
#define SD_SS_PIN       10
#define SD_SCK_PIN      11
#define SD_MOSI_PIN     12
#define SD_MISO_PIN     13
#define SD_DETECT_PIN   -1

// Control
#define FAN_PIN              5
#define COOLANT_MIST_PIN     6
#define SPINDLE_ENABLE_PIN   7
#define SPINDLE_SPEED_PIN    8

// Probe
#define PROBE_PIN            34  // Shared with X_MIN

// UI
#define USER_BUTTON_PIN      37
#define LED_PIN              13

// LCD (Not Connected)
#define LCD_PINS_RS         -1
#define LCD_PINS_ENABLE     -1
#define LCD_PINS_D4         -1
#define LCD_PINS_D5         -1
#define LCD_PINS_D6         -1
#define LCD_PINS_D7         -1
```

---

*Document Version: 1.0*
*Last Updated: 2026-11-19*
*MCU: PIC32MX795F512L (100-pin TQFP)*