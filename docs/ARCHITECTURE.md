# Architecture Overview - PIC32 CNC Firmware

## System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                        APPLICATION LAYER                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐          │
│  │  G-code      │  │  Motion      │  │  Machine     │          │
│  │  Parser      │──│  Planner     │──│  Control     │          │
│  └──────────────┘  └──────────────┘  └──────────────┘          │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                      HAL (Hardware Abstraction)                  │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐           │
│  │  GPIO    │ │  SPI     │ │  Timer   │ │  ADC     │           │
│  │  Driver  │ │  Driver  │ │  Driver  │ │  Driver  │           │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘           │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐                         │
│  │  UART    │ │  PWM     │ │  Interrupt│                        │
│  │  Driver  │ │  Driver  │ │  Manager │                        │
│  └──────────┘ └──────────┘ └──────────┘                         │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                      HARDWARE LAYER (PIC32MX795)                │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐              │
│  │ PORT A-G│ │ SPI1/2  │ │ Timer1-5│ │ ADC     │              │
│  │  GPIO   │ │ Modules │ │ Modules │ │ Module  │              │
│  └─────────┘ └─────────┘ └─────────┘ └─────────┘              │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐                           │
│  │ UART1-6 │ │ OC1-5   │ │ INT0-4  │                           │
│  │ Modules │ │ Modules │ │ Vectors │                           │
│  └─────────┘ └─────────┘ └─────────┘                           │
└─────────────────────────────────────────────────────────────────┘
```

---

## Module Descriptions

### 1. Application Layer

#### G-code Parser (`gcode.h/cpp` - *to be implemented*)
- Lexical analysis of G-code commands
- Parameter extraction (X, Y, Z, F, S, etc.)
- Command validation and error reporting
- Supports: G0-G3, G10, G20/G21, G28, G30, G38, G40-G49, G53-G59, G80-G89, G90-G94, M0-M9, M30, M84, M100-M119

#### Motion Planner (`planner.h/cpp` - *to be implemented*)
- Converts G-code moves to stepper pulses
- Trapezoidal acceleration profiles
- Look-ahead junction speed calculation
- Manages move queue/buffer
- Coordinates multi-axis synchronization

#### Machine Control (`machine.h/cpp` - *to be implemented*)
- High-level state machine (IDLE, RUNNING, PAUSED, ALARM, HOMING)
- Coordinates subsystem initialization
- Manages safety interlocks
- Handles emergency stop

### 2. HAL Layer (Current Implementation)

#### GPIO Driver (`HAL.h/cpp`)
- **Direct register access** for maximum speed
- PORTB manipulation (32 pins)
- Functions: `gpioPinMode()`, `gpioWrite()`, `gpioRead()`
- Used for: Step/Dir/Enable, Endstops, Spindle, Coolant, LED

#### SPI Driver (`HAL.h/cpp`, `PIC32/HAL_SPI.cpp`)
- **SPI1 module** for SD card communication
- Master mode, configurable clock (125 kHz - 8 MHz)
- Supports all 4 SPI modes
- Block read/write for SD protocol

#### Timer Driver (*stub - `HAL.h/cpp`*)
- Timer1 designated for stepper ISR
- Timer2 for serial timing
- Timer3 for servo/PWM
- Needs ISR implementation

#### ADC Driver (*stub - `HAL.h/cpp`*)
- 10-bit ADC on PIC32MX
- Channels: TEMP_0 (AN0), TEMP_BED (AN1)
- Needs conversion trigger and interrupt handling

#### UART Driver (*not implemented*)
- USB CDC or UART2 for host communication
- Ring buffer for RX/TX
- Baud rate: 250000 (configurable)

#### PWM Driver (*not implemented*)
- Output Compare modules (OC1-OC5)
- Spindle speed control (0-100% duty cycle)
- Frequency: ~20 kHz (above audible range)

#### Interrupt Manager (*not implemented*)
- Priority-based interrupt handling
- Critical section macros
- ISR registration/dispatch

### 3. Hardware Layer (PIC32MX795F512L)

#### Key Peripherals Used
| Peripheral | Instance | Purpose |
|------------|----------|---------|
| GPIO | PORTB (primary), PORTA, PORTC-G | All digital I/O |
| SPI | SPI1 | SD Card |
| Timer | Timer1 | Stepper ISR (highest priority) |
| Timer | Timer2 | System tick / Serial |
| Timer | Timer3 | Servo / Spindle PWM |
| ADC | ADC1 | Temperature sensors |
| UART | UART2 / USB | Host communication |
| OC | OC1-OC4 | Spindle PWM, Servos |
| INT | INT0-INT4 | Endstop interrupts |
| DMA | DMA0-7 | *Future: SPI/ADC DMA* |

---

## Data Flow

### G-code Execution Flow
```
Host/USB/SD  →  Command Buffer  →  G-code Parser  →  Motion Planner
                                                              │
                                                              ▼
                                              ┌─────────────────────────┐
                                              │   Stepper ISR (Timer1) │
                                              │  - Pop move from queue │
                                              │  - Calculate steps     │
                                              │  - Output pulses       │
                                              │  - Update position     │
                                              └─────────────────────────┘
                                                              │
                                                              ▼
                                              Step/Dir Signals → Pololu Drivers → Motors
```

### Real-time Constraints
| Task | Frequency | Deadline | Priority |
|------|-----------|----------|----------|
| Stepper ISR | 1-100 kHz | < 5 µs | **HIGHEST** (IPL7) |
| Endstop ISR | Event | < 10 µs | HIGH (IPL6) |
| Serial RX | 250 kbps | < 100 µs | MEDIUM (IPL4) |
| Planner | ~1 kHz | < 1 ms | MEDIUM (IPL3) |
| ADC Sampling | 100 Hz | < 10 ms | LOW (IPL2) |
| UI/Status | 10 Hz | < 100 ms | LOW (IPL1) |

---

## Memory Map (PIC32MX795F512L)

```
Flash (512 KB)
├── 0x1D000000 - 0x1D07FFFF  Boot Flash (12 KB)
├── 0x1FC00000 - 0x1FC03FFF  Configuration Words
├── 0x1D000000 - 0x1D07FFFF  Application Flash (480 KB available)
│   ├── Vector Table (0x200 bytes)
│   ├── .text (code)
│   ├── .rodata (constants)
│   └── .progmem (PROGMEM data)
│
RAM (128 KB)
├── 0x00000000 - 0x0001FFFF  Data RAM (128 KB)
│   ├── .data (initialized globals)
│   ├── .bss (zero-initialized)
│   ├── Stack (grows down from 0x0001FFFF)
│   └── Heap (grows up from .bss end)
│
Peripheral SFRs (0xBF800000 - 0xBF8FFFFF)
├── 0xBF800000 - 0xBF800FFF  Core Timer, INTC, DMA
├── 0xBF801000 - 0xBF801FFF  SPI1
├── 0xBF802000 - 0xBF802FFF  PORTB
├── 0xBF803000 - 0xBF803FFF  PORTA
├── ... (other ports)
├── 0xBF80A000 - 0xBF80AFFF  Timer1-5
├── 0xBF80B000 - 0xBF80BFFF  UART1-6
├── 0xBF80C000 - 0xBF80CFFF  ADC
├── 0xBF80D000 - 0xBF80DFFF  OC1-OC5
└── 0xBF810000 - 0xBF810FFF  INT Controller
```

---

## Configuration System

### Compile-time Configuration
- `Configuration.h` - Basic machine settings
- `Configuration_adv.h` - Advanced tuning parameters
- `pins_pic32mx795.h` - Pin assignments
- `platforms.h` - Architecture detection

### Runtime Configuration
- EEPROM emulation (Flash or SRAM)
- M500/M501/M502/M503 commands
- LCD menu editing

---

## Build System

```
MPLAB X Project (pic32_cnc.X)
├── nbproject/              # MPLAB project metadata
├── main.cpp                # Entry point
├── HAL.h / HAL.cpp         # Generic HAL interface
├── PIC32/
│   ├── HAL.h / HAL.cpp     # PIC32-specific HAL (Arduino-style)
│   └── HAL_SPI.cpp         # SPI implementation
├── pic32/
│   └── pins_pic32mx795.h   # Pin definitions
├── Configuration.h         # Marlin basic config
├── Configuration_adv.h     # Marlin advanced config
├── Marduino.h              # Marlin-Arduino compatibility
├── Arduino.h               # Minimal Arduino.h
├── progmem.h               # PROGMEM macros
├── pins.h                  # Board selector
└── platforms.h             # HAL path resolution
```

---

## Threading Model

**Single-threaded cooperative with ISR preemption**

```
Main Loop (Priority 0 - Background)
├── halTask()           # Background HAL tasks
├── gcodeProcess()      # Parse/execute commands
├── plannerUpdate()     # Refill move buffer
├── machineUpdate()     # State machine
├── uiUpdate()          # LCD/LED updates
└── commsTask()         # Serial TX/RX processing

ISR Context (Preemptive)
├── T1_ISR (Stepper)    # IPL7 - Step pulse generation
├── INT0-4_ISR          # IPL6 - Endstop triggers
├── UART2_RX_ISR        # IPL4 - Serial receive
├── ADC_ISR             # IPL2 - Temperature conversion
└── T2_ISR (SysTick)    # IPL3 - Timebase
```

---

## Safety Architecture

```
┌────────────────────────────────────────────────────────────┐
│                    EMERGENCY STOP CHAIN                     │
│                                                             │
│  Hardware E-Stop  →  MCU NMI/Reset  →  Immediate Stop      │
│       (NC)              (Watchdog)        (All outputs LOW) │
│                                                             │
│  Software E-Stop  →  Machine State  →  Controlled Stop     │
│     (M112/Btn)        = ALARM         (Decel + Disable)    │
│                                                             │
│  Thermal Fault    →  Machine State  →  Controlled Stop     │
│   (Over-temp)         = ALARM         (Disable heaters)    │
│                                                             │
│  Endstop Fault    →  Machine State  →  Controlled Stop     │
│  (Unexpected hit)     = ALARM         (Hold position)      │
└────────────────────────────────────────────────────────────┘
```

---

## Extension Points

1. **New Axis** - Add to `pins_pic32mx795.h`, extend planner for 5/6 axes
2. **New Driver Type** - Implement in HAL (TMC SPI/UART, closed-loop)
3. **New G-code** - Add to parser, planner, and machine control
4. **New Peripheral** - Add HAL driver, register in `halInit()`
5. **New Kinematics** - Implement in planner (CoreXY, Delta, SCARA, etc.)

---

## Porting Notes (from Marlin AVR/ARM)

| Marlin Feature | PIC32 Adaptation |
|----------------|------------------|
| FastIO (AVR) | Direct register access (PORT/LAT/TRIS) |
| Timer0/1/2 (AVR) | Timer1/2/3 (PIC32) |
| SPI (AVR) | SPI1 module (PIC32) |
| ADC (AVR) | ADC1 module (PIC32) |
| UART (AVR) | UART2/USB (PIC32) |
| EEPROM | Flash emulation or external SPI EEPROM |
| PROGMEM | Not needed (Von Neumann, unified memory) |
| ISR Macros | `ISR(vector, ipl) __attribute__((interrupt))` |

---

*Document Version: 1.0*
*Last Updated: 2026-11-19*