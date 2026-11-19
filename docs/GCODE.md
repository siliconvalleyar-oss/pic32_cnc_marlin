# G-Code Reference - PIC32 CNC Firmware

Based on Marlin 2.1 G-code implementation with CNC-specific extensions.

---

## G-Codes - Motion

### G0 - Rapid Positioning
**Rapid move** - Same as G1 but uses `G0_FEEDRATE` if defined.
```
G0 X<pos> Y<pos> Z<pos> A<pos> [F<feedrate>]
```
- Moves at maximum rapid feedrate
- Non-cutting moves between operations

### G1 - Coordinated Linear Move
**Linear interpolation** - Coordinated movement in a straight line.
```
G1 X<pos> Y<pos> Z<pos> A<pos> [F<feedrate>] [S<spindle_power>]
```
- `X Y Z A` - Target position (mm or degrees for rotational axes)
- `F` - Feedrate in mm/min (modal, persists)
- `S` - Spindle/laser power for this move (requires `LASER_POWER_SYNC`)

**CNC Usage:**
```
G1 X50 Y50 F1000        ; Cut to X50 Y50 at 1000 mm/min
G1 Z-5 F200             ; Plunge to Z-5 at 200 mm/min
G1 X100 F1500 S200      ; Cut with spindle power 200 (0-255)
```

### G2 / G3 - Arc Moves (CW/CCW)
**Circular interpolation** - Requires `ARC_SUPPORT`.
```
G2 X<end> Y<end> I<center_offset_x> J<center_offset_y> [F<feedrate>]
G3 X<end> Y<end> I<center_offset_x> J<center_offset_y> [F<feedrate>]
```
- `G2` = Clockwise, `G3` = Counter-clockwise
- `I J K` = Center offsets from current position (not absolute!)
- Active plane determines which axes used (G17=XY, G18=ZX, G19=YZ)
- `R<radius>` alternative to I/J/K (radius format)

**Example:**
```
G17                    ; XY plane
G2 X10 Y10 I5 J0 F500  ; CW arc from current to X10 Y10, center at X+5
G3 X0 Y0 I-5 J0        ; CCW arc back
```

### G4 - Dwell
**Pause execution** for specified time.
```
G4 P<milliseconds>    ; Milliseconds
G4 S<seconds>         ; Seconds
```

### G5 - Cubic B-Spline
**Bézier curve** - Requires `BEZIER_CURVE_SUPPORT`.
```
G5 X<end> Y<end> I<cp1_x> J<cp1_y> P<cp2_x> Q<cp2_y> [F<feedrate>]
```
- Two control points for cubic Bézier

### G17 / G18 / G19 - Plane Selection
**Select active plane** for arcs (G2/G3) and canned cycles.
```
G17  ; XY plane (default)
G18  ; ZX plane
G19  ; YZ plane
```
Requires `CNC_WORKSPACE_PLANES`.

### G20 / G21 - Units
```
G20  ; Inches mode
G21  ; Millimeters mode (default)
```
Requires `INCH_MODE_SUPPORT`.

### G28 - Home Axes
**Home one or more axes** to endstops.
```
G28 [X] [Y] [Z] [A]   ; Home specified axes (all if none)
```
- Moves toward endstops at homing feedrate
- Backs off and re-approaches slowly for accuracy
- Sets machine position to 0 for homed axes

### G30 - Single Probe
**Probe Z at current or specified XY**.
```
G30 [X<pos>] [Y<pos>] [Z<pos>]
```
- Uses Z probe (requires `HAS_BED_PROBE`)
- Reports probed Z height

### G38 - Probe Target (Straight Probe)
**Probe toward/away from workpiece** - Requires `G38_PROBE_TARGET`.
```
G38.2 X<pos> Y<pos> Z<pos> [F<feedrate>]  ; Probe toward, error if no hit
G38.3 X<pos> Y<pos> Z<pos> [F<feedrate>]  ; Probe toward, no error if no hit
G38.4 X<pos> Y<pos> Z<pos> [F<feedrate>]  ; Probe away, error if no release
G38.5 X<pos> Y<pos> Z<pos> [F<feedrate>]  ; Probe away, no error if no release
```
- Stops on probe trigger
- Updates position to trigger point
- Optional multiple probing (`MULTIPLE_PROBING > 1`)

### G53 - Machine Coordinate System
**Non-modal** - Execute next move in machine coordinates (ignore workspace offsets).
```
G53 G1 X0 Y0 Z0  ; Move to machine origin
G53 G0 Z0        ; Rapid to machine Z=0
```

### G54 - G59.3 - Work Coordinate Systems
**Select work offset coordinate system** - Requires `CNC_COORDINATE_SYSTEMS`.
```
G54  ; Work offset 1
G55  ; Work offset 2
G56  ; Work offset 3
G57  ; Work offset 4
G58  ; Work offset 5
G59  ; Work offset 6
G59.1 - G59.3 ; Work offsets 7-9 (with subcode)
```
- Each system has its own XYZ offset from machine origin
- Offsets set with G10 L2 or G92
- Persist in EEPROM if `EEPROM_SETTINGS` enabled

### G80 - Cancel Motion Modes
**Cancel canned cycles** and return to normal G1 mode.
```
G80
```
Requires `GCODE_MOTION_MODES`.

### G90 / G91 - Distance Mode
```
G90  ; Absolute positioning (default)
G91  ; Relative positioning
```
- Affects all axis words (X, Y, Z, A, I, J, K, U, V, W)
- E-axis has separate M82/M83 modes

### G92 - Coordinate System Offset
**Set current position** to specified values (workpiece zero).
```
G92 [X<pos>] [Y<pos>] [Z<pos>] [A<pos>]
```
- Sets offset in active coordinate system (G54-G59)
- `G92.1` - Reset G92 offsets (LinuxCNC extension)
- `G92.2` - Suspend G92 offsets
- `G92.3` - Restore G92 offsets

---

## M-Codes - Spindle & Coolant

### M3 - Spindle ON (Clockwise)
```
M3 [S<speed>] [O<ocr>] [I]
```
- `S` - Speed in RPM (spindle) or power 0-255 (laser)
- `O` - Direct PWM value 0-255
- `I` - Enable inline continuous mode (laser only)

### M4 - Spindle ON (Counter-Clockwise)
```
M4 [S<speed>] [O<ocr>] [I]
```
- Same as M3 but reverse direction
- `I` - Enable dynamic mode (power follows feedrate, laser only)

### M5 - Spindle OFF
```
M5 [I]
```
- Stops spindle/laser
- `I` - Clear inline mode (laser only)

### M7 - Mist Coolant ON
```
M7
```
Requires `COOLANT_MIST`.

### M8 - Flood Coolant / Air Assist ON
```
M8
```
Requires `COOLANT_FLOOD` or `AIR_ASSIST`.

### M9 - Coolant / Air Assist OFF
```
M9
```
Turns off all coolant and air assist.

### M10 / M11 - Vacuum / Blower
```
M10  ; Vacuum/Blower ON
M11  ; Vacuum/Blower OFF
```
Requires `AIR_EVACUATION`.

---

## M-Codes - Coordinate Systems

### G10 L2 - Set Work Offset
```
G10 L2 P<1-9> X<offset> Y<offset> Z<offset> [A<offset>]
```
- `P` = Work offset number (1=G54, 2=G55, ..., 9=G59.3)
- Sets persistent workpiece zero

### G10 L20 - Set Work Offset (Alternative)
```
G10 L20 P<1-9> X<offset> Y<offset> Z<offset>
```

---

## M-Codes - Machine Control

### M0 / M1 - Programmed Stop
```
M0 [P<ms>] [S<sec>] [message]
M1 [P<ms>] [S<sec>] [message]
```
- Pauses execution, waits for user interaction (LCD button)
- Optional timeout

### M17 - Enable All Steppers
```
M17
```

### M18 / M84 - Disable Steppers
```
M18
M84 [S<seconds>]  ; Auto-disable after timeout
```

### M112 - Emergency Stop
```
M112
```
- Immediate shutdown, requires reset

### M114 - Report Position
```
M114
```
Reports current position in all coordinate systems.

### M115 - Capabilities Report
```
M115
```
Reports firmware capabilities and version.

---

## M-Codes - Configuration (Runtime)

### M201 - Set Max Acceleration
```
M201 X<accel> Y<accel> Z<accel> A<accel>
```
Units: mm/s²

### M203 - Set Max Feedrate
```
M203 X<fr> Y<fr> Z<fr> A<fr>
```
Units: mm/s

### M204 - Set Default Accelerations
```
M204 P<printing> R<extruder> T<travel>
```
- `P` - Print/cut moves
- `T` - Travel/rapid moves

### M205 - Advanced Settings
```
M205 S<min_print_speed> T<min_travel_speed>
   B<min_segment_time> X<max_x_jerk> Y<max_y_jerk> Z<max_z_jerk> A<max_a_jerk>
```

### M220 - Feedrate Override
```
M220 S<percent>  ; 0-200%
M220 B          ; Backup current
M220 R          ; Restore backup
```

### M500 - Save to EEPROM
```
M500
```
Saves current settings to persistent storage.

### M501 - Load from EEPROM
```
M501
```

### M502 - Revert to Defaults
```
M502
```
Does NOT write to EEPROM.

### M503 - Print Settings
```
M503 [S<verbose>]
```
`S0` = compact output

---

## CNC-Specific Workflow Examples

### Basic CNC Job
```
G21           ; Millimeters
G90           ; Absolute positioning
G17           ; XY plane
G54           ; Work offset 1
M3 S12000     ; Spindle ON CW 12000 RPM
G0 Z10        ; Rapid to safe Z
G0 X0 Y0      ; Rapid to workpiece zero
G1 Z-2 F200   ; Plunge
G1 X50 F1000  ; Cut
G1 Y50        ; Cut
G0 Z10        ; Retract
M5            ; Spindle OFF
M30           ; Program end (if supported)
```

### Probing Workpiece Zero
```
G21 G90 G17 G54
G38.2 Z-50 F100   ; Probe toward workpiece
G92 Z0            ; Set probed point as Z=0
G0 Z10            ; Retract
```

### Using Work Offsets
```
G10 L2 P1 X0 Y0 Z0    ; Set G54 to machine origin
G10 L2 P2 X100 Y0 Z0  ; Set G55 to 100mm right
G54 G0 X0 Y0          ; Move to first workpiece
... machining ...
G55 G0 X0 Y0          ; Move to second workpiece
... machining ...
```

### Spindle Speed Control
```
M3 S6000      ; 6000 RPM CW
M4 S6000      ; 6000 RPM CCW (if VFD supports)
M5            ; Stop
```
With PWM: `S` value maps to 0-255 PWM duty cycle.

---

## G-Code Parser Behavior

### Parameter Handling
- Parameters can be in any order: `G1 X10 Y20 F100` = `G1 F100 Y20 X10`
- Missing parameters use current modal values
- Multiple commands per line separated by whitespace

### Number Format
- Integer or float: `X10` `X10.5` `X-10.5`
- No leading `+` sign
- Scientific notation NOT supported

### Comments
- `;` - End-of-line comment
- `(comment)` - Inline comment (if `GCODE_COMMENTS` enabled)

### Line Numbers
- `N<num>` - Optional line number for error recovery
- `M110 N<num>` - Set expected line number

---

## Modal Groups (State Persistence)

| Group | Codes | Default |
|-------|-------|---------|
| Motion | G0, G1, G2, G3, G5, G38.x, G80 | G1 |
| Plane | G17, G18, G19 | G17 |
| Distance | G90, G91 | G90 |
| Units | G20, G21 | G21 |
| Coord System | G53, G54-G59.3 | G54 |
| Feedrate | F | - |
| Spindle | M3, M4, M5 | M5 |
| Coolant | M7, M8, M9 | M9 |

---

## Error Handling

- Unknown G/M codes: Reports "Unknown command"
- Invalid parameters: Reports "Invalid parameter"
- Soft limits hit: Stops motion, reports error
- Probe failure (G38.2/4): Reports "Failed to reach target"

---

## Implementation Notes for PIC32 Port

### Required Defines for CNC Features
```c
#define CNC_WORKSPACE_PLANES      // G17/G18/G19
#define CNC_COORDINATE_SYSTEMS    // G54-G59
#define G38_PROBE_TARGET          // G38.2-G38.5
#define SPINDLE_FEATURE           // M3/M4/M5 spindle
#define COOLANT_MIST              // M7
#define COOLANT_FLOOD             // M8/M9
#define ARC_SUPPORT               // G2/G3
#define INCH_MODE_SUPPORT         // G20/G21
#define GCODE_MOTION_MODES        // G80
#define EDITABLE_STEPS_PER_UNIT   // M92
#define EEPROM_SETTINGS           // M500/M501
```

### Parser Implementation
The parser (`parser.h/.cpp`) uses:
- `FASTER_GCODE_PARSER` - Bit-flag based parameter detection
- `GCODE_QUOTED_STRINGS` - Support for quoted string parameters
- Custom `dec_to_float`/`dec_to_long` - Avoids `strtof` overhead

### Motion Flow
1. G-code parsed → `get_destination_from_command()`
2. `motion.prepare_line_to_destination()` called
3. `planner.buffer_line()` adds to move buffer
4. Stepper ISR consumes blocks from planner buffer
5. `planner.synchronize()` waits for buffer empty

---

*Document Version: 1.0*
*Based on Marlin 2.1 G-code Implementation*
*Last Updated: 2026-11-19*