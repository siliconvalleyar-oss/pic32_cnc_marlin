# Configuration Guide - PIC32 CNC Firmware

## Overview
This firmware uses Marlin's configuration system adapted for CNC operations. Configuration is split into two files:
- **Configuration.h** - Basic machine settings
- **Configuration_adv.h** - Advanced tuning parameters

---

## Configuration.h - Essential Settings

### Machine Identification
```c
#define STRING_CONFIG_H_AUTHOR "(YourName/Company)"
// #define CUSTOM_MACHINE_NAME "MyCNC"
// #define MACHINE_UUID "00000000-0000-0000-0000-000000000000"
```

### Board Selection
```c
#define MOTHERBOARD BOARD_RAMPS_14_EFB  // Placeholder - using custom pins_pic32mx795.h
```

### Serial Communication
```c
#define SERIAL_PORT 0          // USB CDC (virtual serial)
#define BAUDRATE 250000        // High speed for responsive control
```

---

### Stepper Drivers
```c
// Pololu A4988/DRV8825 compatible drivers
#define X_DRIVER_TYPE  A4988
#define Y_DRIVER_TYPE  A4988
#define Z_DRIVER_TYPE  A4988
#define E0_DRIVER_TYPE A4988  // Used for A-axis (4th axis)

// Microstepping (configured via MS1/MS2/MS3 pins on driver)
// A4988: 1/16 default, DRV8825: 1/32 default
// Set in pins file or hardware jumpers
```

### Axes Configuration
```c
// 3-axis CNC (X, Y, Z) + optional 4th axis (A)
#define EXTRUDERS 1  // Repurposed for A-axis

// Axis names (for G-code)
#define AXIS4_NAME 'A'
#define AXIS4_ROTATES  // If A is rotational
```

### Endstops
```c
// Endstop logic: true = NC (triggered when open), false = NO (triggered when closed)
// Mechanical switches typically NC for safety
#define X_MIN_ENDSTOP_INVERTING true
#define Y_MIN_ENDSTOP_INVERTING true
#define Z_MIN_ENDSTOP_INVERTING true
#define X_MAX_ENDSTOP_INVERTING true
#define Y_MAX_ENDSTOP_INVERTING true
#define Z_MAX_ENDSTOP_INVERTING true

// Pullup resistors (enable for mechanical switches)
#define ENDSTOPPULLUPS
// #define ENDSTOPPULLUPS_XMIN
// #define ENDSTOPPULLUPS_YMIN
// #define ENDSTOPPULLUPS_ZMIN
```

### Homing
```c
#define HOMING_FEEDRATE_XY 50*60    // mm/min
#define HOMING_FEEDRATE_Z  4*60     // mm/min
#define HOMING_BUMP_MM { 5, 5, 2 }  // Backoff distance after trigger
#define HOMING_BUMP_DIVISOR { 2, 2, 4 }  // Re-approach speed divisor

// Homing order
// #define HOME_Y_BEFORE_X
// #define HOME_Z_FIRST  // Requires real Z endstop
```

### Workspace / Travel Limits
```c
#define X_MIN_POS 0
#define Y_MIN_POS 0
#define Z_MIN_POS 0
#define X_MAX_POS 300   // mm - adjust to your machine
#define Y_MAX_POS 300
#define Z_MAX_POS 100

// Software endstops
#define MIN_SOFTWARE_ENDSTOPS
#define MAX_SOFTWARE_ENDSTOPS
```

### Steps per Unit
```c
// Calculate: (steps_per_rev * microsteps) / (pitch * gear_ratio)
// Example: 200 steps/rev * 16 microsteps / (2mm pitch * 1) = 1600 steps/mm
#define DEFAULT_AXIS_STEPS_PER_UNIT { 1600, 1600, 1600, 1600 }  // X, Y, Z, A

// Max feedrates (mm/min)
#define DEFAULT_MAX_FEEDRATE { 3000, 3000, 500, 1000 }  // X, Y, Z, A

// Max acceleration (mm/s²)
#define DEFAULT_MAX_ACCELERATION { 500, 500, 100, 200 }

// Acceleration for travel moves
#define DEFAULT_TRAVEL_ACCELERATION 500

// Jerk (mm/s) - instantaneous speed change
#define DEFAULT_XJERK 10.0
#define DEFAULT_YJERK 10.0
#define DEFAULT_ZJERK  0.4
#define DEFAULT_AJERK  5.0
```

---

### Spindle / Laser
```c
#define SPINDLE_FEATURE
#define SPINDLE_LASER_ACTIVE_HIGH false  // Active LOW for VFD enable
#define SPINDLE_LASER_PWM_INVERT false   // PWM polarity

// Spindle speed range (RPM)
#define SPINDLE_MAX_RPM 24000
#define SPINDLE_MIN_RPM 0

// PWM frequency (Hz)
#define SPINDLE_PWM_FREQ 10000  // 10 kHz for VFD

// Spindle control pins (defined in pins_pic32mx795.h)
// SPINDLE_ENABLE_PIN = 7
// SPINDLE_SPEED_PIN  = 8 (PWM)
```

### Coolant
```c
#define COOLANT_MIST
#define COOLANT_FLOOD

// Pins (defined in pins_pic32mx795.h)
// COOLANT_MIST_PIN = 6
// FAN_PIN = 5 (used for flood coolant)
```

### Probe (Touch Probe)
```c
// #define BLTOUCH  // If using BLTouch
// #define FIX_MOUNTED_PROBE  // Fixed probe

// Probe offset from spindle (mm)
#define NOZZLE_TO_PROBE_OFFSET { 0, 0, 0 }  // Adjust for your probe mount

// Probing feedrate
#define PROBING_FEEDRATE_MM 50*60  // mm/min

// Probe deployment (if servo/deployable)
// #define PROBE_DEPLOY_FEEDRATE 3000
// #define PROBE_STOW_FEEDRATE 3000
```

---

### Temperature (Optional - for Spindle Motor Monitoring)
```c
#define TEMP_SENSOR_0 1        // 100k thermistor (EPCOS 100k)
#define TEMP_SENSOR_BED 1      // Bed/ambient thermistor

#define HEATER_0_MINTEMP 0     // Disable min temp for spindle
#define HEATER_0_MAXTEMP 150   // Max spindle motor temp

// Thermal protection (monitor only, no heating)
#define THERMAL_PROTECTION_HOTENDS
#define THERMAL_PROTECTION_BED
```

---

### SD Card
```c
#define SDSUPPORT
#define SD_DETECT_INVERTED false
#define SD_DETECT_PIN -1  // Not connected

// SPI speed for SD
// Configured in HAL: SPI1BRG = 4 (~10 MHz at 80 MHz CPU)
```

---

### LCD / UI (If Connected)
```c
// #define REPRAP_DISCOUNT_FULL_GRAPHIC_SMART_CONTROLLER
// #define REPRAP_DISCOUNT_SMART_CONTROLLER
// #define ULTIPANEL
// #define ULTRA_LCD
```

---

## Configuration_adv.h - Advanced Tuning

### Thermal Protection (Monitoring Only)
```c
#define THERMAL_PROTECTION_PERIOD 40      // seconds
#define THERMAL_PROTECTION_HYSTERESIS 4   // °C
#define WATCH_TEMP_PERIOD 40              // seconds
#define WATCH_TEMP_INCREASE 2             // °C
```

### Motion Tuning
```c
// Junction deviation (mm) - smaller = tighter corners
#define JUNCTION_DEVIATION_MM 0.01

// S-curve acceleration (smoother motion)
// #define S_CURVE_ACCELERATION

// Minimum planner speed (mm/s)
#define MIN_PLANNER_SPEED 0.05

// Minimum segment time (µs) - prevents too many short segments
#define MINIMUM_PLANNER_SPEED 0.05
```

### Stepper Timing
```c
// Step pulse width (µs) - minimum 1µs for A4988/DRV8825
#define MINIMUM_STEP_PULSE 2

// Direction setup/hold time (µs)
#define DIRECTION_SETUP_TIME 1
#define DIRECTION_HOLD_TIME 1

// Enable pin timing
#define ENABLE_PINS_ACTIVE_TIME 0  // No delay needed
```

### Endstop Debouncing
```c
// Endstop debounce count
#define ENDSTOP_DEBOUNCE_COUNT 3

// Homing backoff (mm) after trigger
#define HOMING_BACKOFF_POST_MM { 2, 2, 2 }
```

### Motor Current (If Using TMC Drivers)
```c
// For TMC2130/TMC2208/TMC2209 in SPI/UART mode
// #define X_CURRENT 800  // mA
// #define Y_CURRENT 800
// #define Z_CURRENT 800
// #define A_CURRENT 800

// StealthChop / SpreadCycle
// #define STEALTHCHOP_XY
// #define STEALTHCHOP_Z
// #define STEALTHCHOP_A
```

### PID (Not Used for CNC Spindle)
```c
// Disable PID for spindle (VFD handles internally)
// #define PIDTEMP
// #define PIDTEMPBED
```

### EEPROM
```c
#define EEPROM_SETTINGS
// #define EEPROM_BOOT_SILENT

// Storage type (choose one)
// #define FLASH_EEPROM_EMULATION
// #define SRAM_EEPROM_EMULATION
// #define SPI_EEPROM
// #define I2C_EEPROM
// #define SDCARD_EEPROM_EMULATION
```

### Safety Features
```c
// Emergency stop
#define EMERGENCY_STOP_FEATURE

// Kill method on error
#define KILL_METHOD 1  // 1 = halt, 2 = reboot

// Watchdog timer
#define USE_WATCHDOG
#define WATCHDOG_RESET_MANUAL  // Reset in main loop

// Position sanity check
#define MAX_POSITION_CHECK 10000  // mm
```

---

## CNC-Specific Configuration Checklist

### Required Changes for Your Machine

| Setting | Your Value | Notes |
|---------|------------|-------|
| `X_MAX_POS` | ___ mm | Machine X travel |
| `Y_MAX_POS` | ___ mm | Machine Y travel |
| `Z_MAX_POS` | ___ mm | Machine Z travel |
| `DEFAULT_AXIS_STEPS_PER_UNIT` | { __, __, __, __ } | Calculate per axis |
| `DEFAULT_MAX_FEEDRATE` | { __, __, __, __ } | Max rapid speeds |
| `DEFAULT_MAX_ACCELERATION` | { __, __, __, __ } | Accel limits |
| `SPINDLE_MAX_RPM` | ___ | Your spindle max |
| `ENDSTOP_INVERTING` | true/false | NC= true, NO= false |
| `HOMING_FEEDRATE` | ___ mm/min | Homing speed |

### Calculating Steps/mm

```
Steps/mm = (Motor_Steps_Per_Rev × Microsteps) / (Lead_Screw_Pitch × Gear_Ratio)

Examples:
- NEMA 17 (200 steps/rev) + A4988 (1/16) + 2mm pitch leadscrew, direct drive:
  = (200 × 16) / (2 × 1) = 1600 steps/mm

- NEMA 23 (200 steps/rev) + DRV8825 (1/32) + 5mm pitch ballscrew, 2:1 belt:
  = (200 × 32) / (5 × 2) = 640 steps/mm

- Rotational axis (A): 200 × 16 / (360° / 90° per rev worm gear)
  = 3200 / 4 = 800 steps/degree
```

### Calculating Max Feedrate

```
Max Feedrate (mm/min) = (Max_Motor_RPM × Lead_Pitch × 60) / (Microsteps × Gear_Ratio)

Example: 3000 RPM motor, 2mm pitch, 1/16 microstep, direct:
= (3000 × 2 × 60) / 16 = 225,000 mm/min (theoretical)
Practical limit: 3000-5000 mm/min for most hobby CNCs
```

---

## Pin Configuration (pins_pic32mx795.h)

All pin assignments are in `pins_pic32mx795.h`. Modify if your hardware differs:

```c
// Stepper pins - change if using different PORT/pins
#ifndef X_STEP_PIN
  #define X_STEP_PIN             22
#endif
// ... etc for all pins

// Endstop pins
#ifndef X_MIN_PIN
  #define X_MIN_PIN              34
#endif
// ... etc

// SPI pins (hardware SPI1)
#ifndef SD_SS_PIN
  #define SD_SS_PIN              10
#endif
// ... etc
```

---

## Building with Custom Configuration

### MPLAB X Project Properties
1. **Project Properties → XC32 Compiler → Preprocessing**
   - Add include paths:
     - `../pic32`
     - `../PIC32`
   - Defines: `ARDUINO_ARCH_PIC32`, `F_CPU=80000000UL`

2. **Project Properties → XC32 Linker**
   - Heap size: 4096
   - Stack size: 8192

### Configuration Validation
Run `M115` after flashing to verify configuration:
```
FIRMWARE_NAME:Marlin PIC32 CNC
MACHINE_TYPE:CNC
EXTRUDER_COUNT:1
UUID:00000000-0000-0000-0000-000000000000
```

---

## Common Issues & Solutions

| Issue | Cause | Solution |
|-------|-------|----------|
| Homing wrong direction | ENDSTOP_INVERTING wrong | Flip `X_MIN_ENDSTOP_INVERTING` |
| Motors don't move | ENABLE pin logic wrong | Check `X_ENABLE_PIN` active state |
| Steps/mm wrong | Wrong calculation | Recalculate with actual hardware |
| Homing too fast/slow | HOMING_FEEDRATE wrong | Adjust feedrate values |
| SD card not working | SPI pins/speed wrong | Verify SD_SS, SD_SCK, SD_MOSI, SD_MISO |
| Spindle PWM not working | Wrong pin/PWM config | Check SPINDLE_SPEED_PIN, PWM freq |

---

*Document Version: 1.0*
*Last Updated: 2026-11-19*
*Based on Marlin 2.1 Configuration System*