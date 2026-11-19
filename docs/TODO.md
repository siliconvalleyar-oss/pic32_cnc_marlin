# TODO - PIC32 CNC Project Task Tracking

## Project Overview
PIC32MX795-based CNC firmware based on Marlin architecture, adapted for CNC operations with Pololu stepper drivers and endstops.

---

## Current Status: Foundation Phase

### ✅ Completed
- [x] Basic project structure with MPLAB X
- [x] HAL layer with GPIO and SPI (direct register access)
- [x] Pin definitions for PIC32MX795 (4-5 axes + SD card)
- [x] Marlin Configuration.h and Configuration_adv.h adapted
- [x] Minimal Arduino.h compatibility layer
- [x] Marduino.h for Marlin macro compatibility
- [x] PROGMEM compatibility macros
- [x] Platform detection for PIC32 architecture

---

## 🔴 Critical Priority (Must Have)

### Core Motion Control
- [ ] **Stepper ISR Implementation** - Timer-based interrupt service routine for step generation
  - [ ] Timer1 configuration for stepper timing
  - [ ] Multi-axis step generation (X, Y, Z, A)
  - [ ] Acceleration/deceleration ramping (trapezoidal or S-curve)
  - [ ] Step/dir output with proper timing
- [ ] **Motion Planner** - G-code to step conversion
  - [ ] Line segment planner with look-ahead
  - [ ] Junction deviation / cornering algorithm
  - [ ] Feed rate calculation per axis
- [ ] **G-code Parser** - Command interpretation
  - [ ] G0/G1 linear moves
  - [ ] G2/G3 arc moves (CW/CCW)
  - [ ] G28 homing
  - [ ] G90/G91 absolute/relative
  - [ ] G92 coordinate offset
  - [ ] M3/M4/M5 spindle control
  - [ ] M7/M8/M9 coolant control

### Endstop & Homing
- [ ] **Endstop Handling**
  - [ ] X_MIN, X_MAX, Y_MIN, Y_MAX, Z_MIN, Z_MAX interrupts
  - [ ] Debouncing (hardware/software)
  - [ ] Homing state machine
- [ ] **Homing Sequences**
  - [ ] Standard 3-axis homing
  - [ ] Optional 4th axis (A) homing
  - [ ] Probe-based Z homing

### Spindle Control
- [ ] **Spindle PWM** - Variable speed control
  - [ ] PWM output on SPINDLE_SPEED_PIN (pin 8)
  - [ ] RPM to PWM mapping
  - [ ] Spindle enable/disable (SPINDLE_ENABLE_PIN)
  - [ ] Direction control (if VFD supports)

---

## 🟠 High Priority (Should Have)

### SD Card & File System
- [ ] **SD Card Driver** - FAT32 support
  - [ ] SPI block read/write
  - [ ] File open/read/close
  - [ ] G-code file streaming
- [ ] **G-code Streaming** - Real-time execution from SD

### Temperature Monitoring (Optional for CNC)
- [ ] **ADC Driver** - Thermistor reading
  - [ ] TEMP_0_PIN (spindle motor temp)
  - [ ] TEMP_BED_PIN (ambient/bed if heated)
- [ ] **Thermal Protection** - Over-temp shutdown

### Communication
- [ ] **Serial/USB Communication**
  - [ ] UART initialization (USB CDC or UART2)
  - [ ] Command buffer handling
  - [ ] OK/Error response protocol
  - [ ] Status reporting (M114, M115, M105)

### Coolant & Mist
- [ ] **Coolant Control** - FLOOD (FAN_PIN) and MIST (COOLANT_MIST_PIN)
  - [ ] M7 (mist on), M8 (flood on), M9 (both off)

---

## 🟡 Medium Priority (Nice to Have)

### Advanced Motion
- [ ] **Probe Support** - Touch probe for workpiece zeroing
  - [ ] G38.2/G38.3 probe toward/away
  - [ ] Probe offset calibration
- [ ] **Tool Length Offset** - G43/G44/G49
- [ ] **Coordinate Systems** - G54-G59 work offsets
- [ ] **Canned Cycles** - G81-G89 drilling cycles

### User Interface
- [ ] **LCD Support** - Character/Graphic display
  - [ ] Status display
  - [ ] Menu navigation
  - [ ] Jog controls
- [ ] **Rotary Encoder / Buttons** - Manual control
- [ ] **LED Status Indicators** - Power, Run, Alarm, Error

### Diagnostics
- [ ] **Self-Test on Boot** - Hardware verification
- [ ] **Error Logging** - Persistent error history
- [ ] **Watchdog Timer** - System health monitoring

---

## 🟢 Low Priority (Future Enhancements)

### Networking
- [ ] **Ethernet/WiFi** - Remote monitoring/control
- [ ] **Web Interface** - Browser-based control panel

### Advanced Features
- [ ] **Rigid Tapping** - Spindle synchronized with Z
- [ ] **Thread Cutting** - Spindle synchronized with X/Z
- [ ] **Camera Integration** - Workpiece visualization
- [ ] **Auto Tool Changer** - ATC support

---

## 📋 Technical Debt / Refactoring

- [ ] **HAL Refactoring** - Separate PIC32-specific from generic HAL
- [ ] **Interrupt Safety** - Critical sections for shared data
- [ ] **Memory Optimization** - Reduce RAM usage
- [ ] **Code Documentation** - Doxygen-style comments
- [ ] **Unit Tests** - Hardware abstraction tests
- [ ] **Static Analysis** - MISRA-C compliance check

---

## 🐛 Known Issues

1. **HAL.cpp vs PIC32/HAL.cpp** - Duplicate implementations exist; consolidate
2. **ADC Stub** - `adcRead()` returns 0; needs real implementation
3. **Timer Not Configured** - `stepperTimerInit()` is empty
4. **No Interrupt Handlers** - ISR vectors not defined
5. **Serial Not Implemented** - No UART communication yet
6. **Configuration.h is 3D Printer Oriented** - Needs CNC-specific cleanup

---

## 📅 Milestones

| Milestone | Target | Status |
|-----------|--------|--------|
| M1: HAL Complete (GPIO, SPI, Timer, ADC, UART) | Week 1-2 | 🔄 In Progress |
| M2: Stepper ISR + Basic Motion (G0/G1) | Week 3-4 | ⏳ Pending |
| M3: G-code Parser + Planner | Week 5-6 | ⏳ Pending |
| M4: Homing + Endstops | Week 7 | ⏳ Pending |
| M5: Spindle + Coolant Control | Week 8 | ⏳ Pending |
| M6: SD Card + File Streaming | Week 9-10 | ⏳ Pending |
| M7: Serial Communication + Host Control | Week 11-12 | ⏳ Pending |
| M8: LCD + User Interface | Week 13-14 | ⏳ Pending |
| M9: Beta Release - Basic CNC Operational | Week 15-16 | ⏳ Pending |

---

## 📝 Notes

- This project is based on **Marlin 2.1** firmware architecture
- Target MCU: **PIC32MX795F512L** (80 MHz, 512 KB Flash, 128 KB RAM)
- Stepper Drivers: **Pololu A4988/DRV8825** compatible
- Endstops: **Mechanical switches** (NC/NO configurable)
- Build System: **MPLAB X IDE** with **XC32 Compiler**

---

*Last Updated: 2026-11-19*