# CNC Features - PIC32 CNC Firmware

Based on Marlin 2.1 CNC/Spindle/Laser features adapted for PIC32MX795.

---

## Feature Overview

| Feature | Define | Status | Description |
|---------|--------|--------|-------------|
| Workspace Planes | `CNC_WORKSPACE_PLANES` | 🔴 Required | G17/G18/G19 plane selection |
| Work Coordinate Systems | `CNC_COORDINATE_SYSTEMS` | 🔴 Required | G54-G59.3 offsets |
| Probe Target | `G38_PROBE_TARGET` | 🔴 Required | G38.2-G38.5 probing |
| Spindle Control | `SPINDLE_FEATURE` | 🔴 Required | M3/M4/M5 with PWM |
| Coolant Control | `COOLANT_MIST` / `COOLANT_FLOOD` | 🟡 Optional | M7/M8/M9 |
| Arc Support | `ARC_SUPPORT` | 🔴 Required | G2/G3 circular interpolation |
| Inch Mode | `INCH_MODE_SUPPORT` | 🟢 Optional | G20/G21 units |
| Motion Modes | `GCODE_MOTION_MODES` | 🟢 Optional | G80 cancel |
| EEPROM Settings | `EEPROM_SETTINGS` | 🔴 Required | M500/M501 persistence |

---

## 1. Workspace Planes (G17/G18/G19)

### Purpose
Select the active plane for arc moves (G2/G3), canned cycles, and tool radius compensation.

### Implementation
```c
// gcode.h
#if ENABLED(CNC_WORKSPACE_PLANES)
  enum WorkspacePlane : char { PLANE_XY, PLANE_ZX, PLANE_YZ };
  static WorkspacePlane workspace_plane;
#endif

// G17-G19.cpp
void GcodeSuite::G17() { set_workspace_plane(PLANE_XY); }
void GcodeSuite::G18() { set_workspace_plane(PLANE_ZX); }
void GcodeSuite::G19() { set_workspace_plane(PLANE_YZ); }
```

### Arc Plane Mapping
| Plane | G-code | Axes Used | I/J/K Mapping |
|-------|--------|-----------|---------------|
| XY | G17 | X, Y | I=X, J=Y |
| ZX | G18 | Z, X | I=Z, J=X |
| YZ | G19 | Y, Z | I=Y, J=Z |

### Usage in Motion
```cpp
// G2_G3.cpp - Arc planning uses workspace_plane
#if ENABLED(CNC_WORKSPACE_PLANES)
  // Determine active axes based on plane
  AxisEnum axis_0, axis_1;
  switch (workspace_plane) {
    case PLANE_XY: axis_0 = X_AXIS; axis_1 = Y_AXIS; break;
    case PLANE_ZX: axis_0 = Z_AXIS; axis_1 = X_AXIS; break;
    case PLANE_YZ: axis_0 = Y_AXIS; axis_1 = Z_AXIS; break;
  }
#endif
```

### PIC32 Implementation Notes
- Default to `PLANE_XY` on startup/reset
- Homing (G28) resets to `PLANE_XY`
- Store in EEPROM for persistence across restarts

---

## 2. Work Coordinate Systems (G54-G59.3)

### Purpose
Multiple workpiece zero references (work offsets) for batch machining.

### Data Structure
```cpp
// gcode.h
#define MAX_COORDINATE_SYSTEMS 9  // G54-G59.3
static int8_t active_coordinate_system;  // -1 = machine, 0-8 = G54-G59.3
static xyz_pos_t coordinate_system[MAX_COORDINATE_SYSTEMS];
```

### Offset Application
```cpp
// G53-G59.cpp
bool GcodeSuite::select_coordinate_system(const int8_t _new) {
  if (active_coordinate_system == _new) return false;
  active_coordinate_system = _new;
  xyz_float_t new_offset{0};
  if (WITHIN(_new, 0, MAX_COORDINATE_SYSTEMS - 1))
    new_offset = coordinate_system[_new];
  motion.workspace_offset = new_offset;  // Applied to all positions
  return true;
}
```

### G53 - Machine Coordinates (Non-Modal)
```cpp
void GcodeSuite::G53() {
  const int8_t old_system = active_coordinate_system;
  select_coordinate_system(-1);  // Remove workspace offsets
  if (parser.chain()) {          // Chain next command
    process_parsed_command();
    select_coordinate_system(old_system);  // Restore
  }
}
```

### Setting Offsets
- **G10 L2 Pn** - Set work offset `n` (1-9) to current position
- **G92** - Set offset in active coordinate system
- **M500** - Save to EEPROM
- **M501** - Restore from EEPROM

### PIC32 Implementation Notes
- 9 coordinate systems × 4 axes (X,Y,Z,A) = 36 floats ≈ 144 bytes
- Store in emulated EEPROM (Flash or SRAM)
- Apply offset in `planner.apply_modifiers()` or motion layer

---

## 3. Probe Target (G38.2-G38.5)

### Purpose
Straight-line probing for workpiece alignment, tool length measurement, and digitizing.

### Sub-codes
| Code | Action | Error on Fail |
|------|--------|---------------|
| G38.2 | Probe toward, stop on contact | Yes |
| G38.3 | Probe toward, stop on contact | No |
| G38.4 | Probe away, stop on release | Yes |
| G38.5 | Probe away, stop on release | No |

### Implementation Flow
```cpp
// G38.cpp
FORCE_INLINE bool G38_run_probe() {
  // 1. Calculate retract distance per axis
  // 2. Wait for planner idle
  // 3. Enable endstops for probe
  // 4. Move until trigger or destination
  // 5. If triggered: update position, optionally re-probe slower
  // 6. Disable endstops, return success/fail
}
```

### Multi-Probing (MULTIPLE_PROBING > 1)
```cpp
// Second pass at 25% feedrate for accuracy
REMEMBER(fr, motion.feedrate_mm_s, motion.feedrate_mm_s * 0.25);
motion.destination -= retract_mm * 2;  // Re-approach
G38_single_probe(move_value);
```

### Probe Position Update
```cpp
motion.set_current_from_steppers_for_axis(ALL_AXES_ENUM);
motion.sync_plan_position();  // Sync planner with actual position
```

### PIC32 Implementation Notes
- Use dedicated probe pin or share with Z_MIN
- Endstop interrupts for fast trigger detection
- Debounce: `ENDSTOP_DEBOUNCE_COUNT` (typically 3-5)
- Minimum move: `G38_MINIMUM_MOVE` (0.01mm default)

---

## 4. Spindle Control (M3/M4/M5)

### Architecture
```
M3/M4/M5 G-code → GcodeSuite::M3_M4()/M5()
                    ↓
            SpindleLaser::set_enabled()/apply_power()
                    ↓
            Hardware: PWM + Enable + Direction pins
```

### Operating Modes
```cpp
// spindle_laser.h
enum CutterMode : int8_t {
  CUTTER_MODE_STANDARD,     // Direct power, waits for moves
  CUTTER_MODE_CONTINUOUS,   // Inline power via planner (M3 I)
  CUTTER_MODE_DYNAMIC       // Power follows feedrate (M4 I)
};
```

### Standard Mode (Default)
```cpp
// M3_M5.cpp
void GcodeSuite::M3_M4(const bool is_M4) {
  if (cutter.cutter_mode == CUTTER_MODE_STANDARD)
    planner.synchronize();  // Wait for moves to complete
  
  // Parse S (RPM/Power) or O (PWM 0-255)
  get_s_power();
  
  cutter.set_enabled(true);
  cutter.apply_power(
    TERN(SPINDLE_LASER_USE_PWM, cutter.upower_to_ocr(cutter.unitPower), 255)
  );
  TERN_(SPINDLE_CHANGE_DIR, cutter.set_reverse(is_M4));
}
```

### Power Mapping
```cpp
// Spindle: Configured in RPM, output as PWM
#define RPM_TO_PWM(X) ((X) * 255 / (SPEED_POWER_MAX))
#define PWM_TO_RPM(X) ((X) * (SPEED_POWER_MAX) / 255)

// Laser: Configured in Percent (0-100), output as PWM
#define PCT_TO_PWM(X) ((X) * 255 / 100)
```

### Spindle Acceleration (Optional)
```cpp
#if HAS_SPINDLE_ACCELERATION
  // Ramp PWM smoothly over time
  const millis_t duration = (float(SPEED_POWER_MAX) * (60000.f / 2550.f) 
                           / float(acceleration_spindle_deg_per_s2)) * abs_diff;
  while (current_ocr != target_ocr) {
    hal.set_pwm_duty(pin_t(SPINDLE_LASER_PWM_PIN), current_ocr ^ SPINDLE_LASER_PWM_OFF);
    delay(duration);
    current_ocr += diff > 0 ? 1 : -1;
  }
#endif
```

### PIC32 Hardware Mapping
| Signal | Pin | PIC32 Port | Timer/OC |
|--------|-----|------------|----------|
| PWM | 8 | RB8 | OC4 |
| Enable | 7 | RB7 | GPIO |
| Direction | - | - | GPIO (if VFD supports) |

### PWM Configuration
```c
// 10 kHz PWM for VFD (above audible range)
#define SPINDLE_PWM_FREQ 10000
#define SPINDLE_LASER_FREQUENCY SPINDLE_PWM_FREQ
#define SPINDLE_LASER_USE_PWM
```

---

## 5. Coolant Control (M7/M8/M9)

### Implementation
```cpp
// M7-M9.cpp
#if ENABLED(COOLANT_MIST)
void GcodeSuite::M7() {
  planner.synchronize();
  WRITE(COOLANT_MIST_PIN, !(COOLANT_MIST_INVERT));
}
#endif

#if ANY(COOLANT_FLOOD, AIR_ASSIST)
void GcodeSuite::M8() {
  planner.synchronize();
  #if ENABLED(COOLANT_FLOOD)
    WRITE(COOLANT_FLOOD_PIN, !(COOLANT_FLOOD_INVERT));
  #endif
  #if ENABLED(AIR_ASSIST)
    cutter.air_assist_enable();
  #endif
}

void GcodeSuite::M9() {
  planner.synchronize();
  #if ENABLED(COOLANT_MIST)
    WRITE(COOLANT_MIST_PIN, COOLANT_MIST_INVERT);
  #endif
  #if ENABLED(COOLANT_FLOOD)
    WRITE(COOLANT_FLOOD_PIN, COOLANT_FLOOD_INVERT);
  #endif
  #if ENABLED(AIR_ASSIST)
    cutter.air_assist_disable();
  #endif
}
#endif
```

### Pin Mapping
| Function | Pin | Active State |
|----------|-----|--------------|
| Mist | 6 (RB6) | Configurable |
| Flood | 5 (RB5) | Configurable |

### PIC32 Notes
- Simple GPIO outputs, no PWM needed
- `planner.synchronize()` ensures coolant changes at move boundaries
- Active state configurable per pin (`_INVERT` defines)

---

## 6. Arc Support (G2/G3)

### Implementation Overview
```cpp
// G2_G3.cpp
void GcodeSuite::G2_G3(const bool clockwise) {
  // 1. Get destination and center offsets (I/J/K or R)
  // 2. Calculate arc parameters (radius, center, angles)
  // 3. Segment arc into linear moves
  // 4. Buffer each segment via planner.buffer_line()
}
```

### Segmentation
- Arcs split into small line segments
- Segment length controlled by `MM_PER_ARC_SEGMENT` or `ARC_SEGMENTS_PER_SECOND`
- Default: 1mm segments or 200 segments/second

### Plane Awareness
```cpp
#if ENABLED(CNC_WORKSPACE_PLANES)
  // Use workspace_plane to determine active axes
  AxisEnum axis_0, axis_1;
  switch (workspace_plane) { ... }
#endif
```

---

## 7. Motion Planner Integration

### Cutter Power in Planner Blocks
```cpp
// planner.h
#if HAS_CUTTER
  cutter_power_t cutter_power;  // Power level for this block
#endif

// planner.cpp - Buffering moves with power
bool Planner::buffer_segment(...) {
  block_t *block = get_next_free_block(...);
  block->cutter_power = cutter.power;  // Capture current power
  ...
}
```

### Laser Power Sync (LASER_POWER_SYNC)
```cpp
// M3_M5.cpp - Inline power mode
if (cutter.cutter_mode == CUTTER_MODE_CONTINUOUS) {
  planner.laser_inline.status.isPowered = true;
  get_s_power();  // Update power
  #if ENABLED(LASER_POWER_SYNC)
    planner.buffer_sync_block(BLOCK_BIT_LASER_PWR);  // Queue power sync block
  #else
    planner.synchronize();
    cutter.inline_power(cutter.power);
  #endif
}
```

---

## 8. Safety Features

### Thermal Protection (Monitoring Only)
```cpp
#define THERMAL_PROTECTION_HOTENDS  // Monitor spindle motor temp
#define THERMAL_PROTECTION_BED      // Monitor ambient
#define WATCH_TEMP_PERIOD 40        // Seconds
#define WATCH_TEMP_INCREASE 2       // °C min increase
```

### Emergency Stop
```cpp
// M112 - Immediate halt
static void M112() {
  quick_stop();           // Stop all motion
  kill();                 // Disable heaters, steppers
  disable();              // Disable spindle
  SERIAL_ERROR_MSG("Emergency Stop");
}
```

### Soft Limits
```cpp
#define MIN_SOFTWARE_ENDSTOPS
#define MAX_SOFTWARE_ENDSTOPS
// Checks in planner.buffer_line() and motion.prepare_line_to_destination()
```

### Position Sanity Check
```cpp
#define MAX_POSITION_CHECK 10000  // mm
```

---

## 9. Configuration for PIC32 CNC

### Required Configuration.h Settings
```c
// CNC Core
#define CNC_WORKSPACE_PLANES
#define CNC_COORDINATE_SYSTEMS
#define G38_PROBE_TARGET
#define ARC_SUPPORT

// Spindle
#define SPINDLE_FEATURE
#define SPINDLE_LASER_USE_PWM
#define SPINDLE_LASER_PWM_PIN 8
#define SPINDLE_LASER_ENA_PIN 7
#define SPINDLE_CHANGE_DIR
#define SPINDLE_DIR_PIN 9
#define SPEED_POWER_MIN 0
#define SPEED_POWER_MAX 24000  // Max RPM
#define SPEED_POWER_STARTUP 6000

// Coolant
#define COOLANT_MIST
#define COOLANT_MIST_PIN 6
#define COOLANT_FLOOD
#define COOLANT_FLOOD_PIN 5

// Probing
#define Z_MIN_PROBE_USES_Z_MIN_ENDSTOP_PIN
#define G38_MINIMUM_MOVE 0.01

// Units
#define INCH_MODE_SUPPORT

// EEPROM
#define EEPROM_SETTINGS
#define FLASH_EEPROM_EMULATION

// Safety
#define EMERGENCY_STOP_FEATURE
#define USE_WATCHDOG
```

### Configuration_adv.h Tuning
```c
// Motion
#define JUNCTION_DEVIATION_MM 0.01
#define MINIMUM_STEP_PULSE 2
#define DIRECTION_SETUP_TIME 1
#define HOMING_BUMP_MM { 5, 5, 2, 2 }
#define HOMING_BUMP_DIVISOR { 2, 2, 4, 4 }

// Spindle
#define SPINDLE_LASER_POWERUP_DELAY 1000
#define SPINDLE_LASER_POWERDOWN_DELAY 5000
#define SPINDLE_LASER_FREQUENCY 10000

// Probe
#define MULTIPLE_PROBING 2
#define G38_MINIMUM_MOVE 0.01

// Planner
#define BLOCK_BUFFER_SIZE 16
#define MIN_SEGMENT_TIME_US 200
```

---

## 10. Implementation Priority for PIC32

### Phase 1: Core Motion (Week 1-2)
- [ ] Stepper ISR with Timer1
- [ ] Basic G0/G1 motion
- [ ] Planner buffer + trapezoidal acceleration

### Phase 2: CNC G-codes (Week 3-4)
- [ ] G17/G18/G19 plane selection
- [ ] G54-G59 work offsets
- [ ] G2/G3 arc segmentation
- [ ] G38 probing

### Phase 3: Spindle/Coolant (Week 5)
- [ ] PWM output on RB8 (OC4)
- [ ] Enable pin RB7
- [ ] Direction pin (if VFD)
- [ ] M7/M8/M9 coolant GPIO

### Phase 4: Advanced (Week 6+)
- [ ] EEPROM emulation
- [ ] Serial communication
- [ ] SD card streaming
- [ ] LCD/UI

---

*Document Version: 1.0*
*Based on Marlin 2.1 CNC Feature Implementation*
*Last Updated: 2026-11-19*