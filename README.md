# PIC32 CNC Firmware - Marlin Based

[![Release](https://img.shields.io/github/v/release/siliconvalleyar-oss/pic32_cnc_marlin)](https://github.com/siliconvalleyar-oss/pic32_cnc_marlin/releases)
[![License](https://img.shields.io/github/license/siliconvalleyar-oss/pic32_cnc_marlin)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-PIC32MX795-blue)](https://www.microchip.com/en-us/product/PIC32MX795F512L)

Firmware for PIC32MX795F512L based CNC controller, adapted from **Marlin 2.1** 3D printer firmware for CNC operations with Pololu stepper drivers, endstops, spindle control, and coolant systems.

---

## 🎯 Features

### Core Motion Control
- **4-5 Axis Support**: X, Y, Z, A (optional 4th rotational axis)
- **Direct Register Access**: Maximum performance GPIO/SPI via hardware registers
- **Trapezoidal Acceleration**: Smooth motion profiles with configurable jerk
- **Step/Dir Output**: Compatible with Pololu A4988, DRV8825, TMC2208/2209 drivers

### CNC-Specific G-Codes (from Marlin)
| Feature | G-Codes | Description |
|---------|---------|-------------|
| **Workspace Planes** | G17/G18/G19 | XY, ZX, YZ plane selection for arcs |
| **Work Offsets** | G54-G59.3 | 9 coordinate systems for batch machining |
| **Probing** | G38.2-G38.5 | Straight probe toward/away from workpiece |
| **Arcs** | G2/G3 | CW/CCW circular interpolation |
| **Units** | G20/G21 | Inches / Millimeters |
| **Machine Coords** | G53 | Non-modal machine coordinate move |

### Spindle & Coolant
- **Spindle Control**: M3 (CW), M4 (CCW), M5 (Stop) with PWM speed control
- **Coolant**: M7 (Mist), M8 (Flood), M9 (Off)
- **Air Assist**: M10/M11 for vacuum/blower

### Hardware Interface
| Peripheral | Pins | Implementation |
|------------|------|----------------|
| Stepper X | 22,23,24 | Step/Dir/Enable |
| Stepper Y | 25,26,27 | Step/Dir/Enable |
| Stepper Z | 28,29,30 | Step/Dir/Enable |
| Stepper A | 31,32,33 | Step/Dir/Enable |
| Endstops | 34-39 | X/Y/Z Min/Max |
| SPI/SD | 10-13 | Hardware SPI1 |
| Spindle PWM | 8 | OC4 (10kHz) |
| Spindle Enable | 7 | GPIO |
| Coolant Mist | 6 | GPIO |
| Coolant Flood | 5 | GPIO |

---

## 📦 Project Structure

```
pic32_cnc.X/
├── main.cpp                 # Entry point
├── HAL.h / HAL.cpp          # Hardware Abstraction Layer (GPIO, SPI, Timers)
├── HAL_SPI.cpp              # SPI implementation
├── PIC32/
│   ├── HAL.h / HAL.cpp      # PIC32-specific HAL (Arduino-style)
│   └── HAL_SPI.cpp          # SPI1 register-level implementation
├── pic32/
│   └── pins_pic32mx795.h    # Pin assignments for PIC32MX795
├── Configuration.h          # Marlin basic configuration
├── Configuration_adv.h      # Marlin advanced configuration
├── Arduino.h                # Minimal Arduino compatibility
├── Marduino.h               # Marlin-Arduino macros
├── progmem.h                # PROGMEM compatibility
├── platforms.h              # Architecture detection
├── pins.h                   # Board selector
├── docs/                    # Documentation
│   ├── API.md               # HAL API reference
│   ├── ARCHITECTURE.md      # System architecture
│   ├── PINOUT.md            # Pin assignments
│   ├── CONFIGURATION.md     # Configuration guide
│   ├── GCODE.md             # G-code reference
│   ├── FEATURES.md          # CNC features deep-dive
│   └── TODO.md              # Task tracking
└── nbproject/               # MPLAB X project files
```

---

## 🛠️ Building

### Requirements
- **MPLAB X IDE** v6.00+
- **XC32 Compiler** v4.30+
- **PIC32MX795F512L** target device

### Build Steps
1. Open `pic32_cnc.X` in MPLAB X IDE
2. Select configuration: `pic32_cnc_default`
3. Build: `Clean and Build` (hammer icon)
4. Output: `dist/pic32_cnc_default/production/pic32_cnc.X.production.hex`

### Configuration
Edit `Configuration.h` for your machine:
```c
// Travel limits (mm)
#define X_MAX_POS 300
#define Y_MAX_POS 300
#define Z_MAX_POS 100

// Steps per unit (calculate for your hardware)
#define DEFAULT_AXIS_STEPS_PER_UNIT { 1600, 1600, 1600, 1600 }

// Max feedrates (mm/min)
#define DEFAULT_MAX_FEEDRATE { 3000, 3000, 500, 1000 }

// Spindle
#define SPEED_POWER_MAX 24000  // Max RPM
```

Pin assignments in `pic32/pins_pic32mx795.h`.

---

## 📚 Documentation

| Document | Description |
|----------|-------------|
| [API.md](docs/API.md) | HAL function reference |
| [ARCHITECTURE.md](docs/ARCHITECTURE.md) | System design & data flow |
| [PINOUT.md](docs/PINOUT.md) | Complete pin mapping |
| [CONFIGURATION.md](docs/CONFIGURATION.md) | All configuration options |
| [GCODE.md](docs/GCODE.md) | Supported G/M codes |
| [FEATURES.md](docs/FEATURES.md) | CNC feature implementation |
| [TODO.md](docs/TODO.md) | Development roadmap |

---

## 🔧 Hardware

### Tested Configuration
- **MCU**: PIC32MX795F512L (100-pin TQFP, 80 MHz, 512 KB Flash, 128 KB RAM)
- **Stepper Drivers**: Pololu A4988 / DRV8825 (Step/Dir interface)
- **Endstops**: Mechanical switches (NC recommended)
- **SD Card**: MicroSD via SPI1
- **Spindle**: VFD with 0-10V or PWM input
- **Coolant**: Relay boards for mist/flood

### Schematic Reference
See [PINOUT.md](docs/PINOUT.md) for complete pin assignments and electrical interface notes.

---

## 🚀 Roadmap

- [ ] Stepper ISR with Timer1 (multi-axis, acceleration)
- [ ] G-code parser + Motion planner
- [ ] G28 Homing + G38 Probing
- [ ] Spindle PWM + Direction control
- [ ] SD Card G-code streaming
- [ ] USB/Serial host communication
- [ ] Work offsets (G54-G59) persistence
- [ ] LCD + Rotary encoder UI

See [TODO.md](docs/TODO.md) for detailed milestones.

---

## 📄 License

Based on **Marlin Firmware** (GPLv3+).
This adaptation retains GPLv3 license.

Original Marlin: https://github.com/MarlinFirmware/Marlin
Copyright (c) 2020 MarlinFirmware

---

## 🤝 Contributing

1. Fork the repository
2. Create feature branch: `git checkout -b feature/my-feature`
3. Commit changes: `git commit -am 'Add my feature'`
4. Push: `git push origin feature/my-feature`
5. Open Pull Request

---

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/siliconvalleyar-oss/pic32_cnc_marlin/issues)
- **Discussions**: [GitHub Discussions](https://github.com/siliconvalleyar-oss/pic32_cnc_marlin/discussions)

---

## 📋 Version History

| Version | Date | Changes |
|---------|------|---------|
| **v1.00** | 2026-11-19 | Initial release - HAL, pins, config, docs |

---

*Built for the PIC32MX795 CNC project. Based on Marlin 2.1 architecture.*