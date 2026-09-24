# Modbus Register Map — Engine Data Converter (STM32F103)

## Connection details

| Parameter | Value |
|---|---|
| Protocol | Modbus **TCP** (via Mudbus library, RJ45/Ethernet) |
| IP Address | 192.168.1.22 |
| Port | 502 |
| Device / Unit ID | 2 (Mudbus generally does not enforce unit ID filtering, but 2 is what's configured/used in testing with ModScan) |
| Supported function code | **03 — Read Holding Registers** |
| Address base | 0-based (register 0 = Modbus address 40001 in 1-based/PLC notation) |
| Register data type | 16-bit unsigned integer (`unsigned int`), big-endian per register |

> Note: the Mudbus library may also technically respond to function code 06/16 (write register), but nothing in this firmware reads back written values for control — writing to these registers has no effect on engine behavior and will simply be overwritten on the next valid parsed frame (or zeroed by the 3-second no-data watchdog). Treat this device as **read-only**.

## Holding register map

| Reg (0-based) | Modbus Addr | Name | Source field | Description | Data type / Notes |
|---:|---:|---|---|---|---|
| 0 | 40001 | Run_Mins | `Run_Mins` | Engine running minutes, rolls 0–59 within the current hour | uint8 in a uint16 register |
| 1 | 40002 | Run_Hours | `Run_Hours` | Total accumulated engine running hours | uint16, byte order: low byte then high byte in source frame |
| 2 | 40003 | Light_State1 | `light_state1` | Light/indicator status, bitfield 1 | uint8 bitfield — bit meanings not yet documented from engine controller spec |
| 3 | 40004 | Light_State2 | `light_state2` | Light/indicator status, bitfield 2 | uint8 bitfield |
| 4 | 40005 | Relay_State1 | `Relay_state1` | Relay output status, bitfield 1 | uint8 bitfield |
| 5 | 40006 | Relay_State2 | `Relay_state2` | Relay output status, bitfield 2 | uint8 bitfield |
| 6 | 40007 | Engine_Speed | `Engine_Speed` | Engine speed (RPM) | uint16, byte order: high byte then low byte in source frame |
| 7 | 40008 | Bat_Volt | `Bat_Volt` | Battery voltage | uint16 — scaling (e.g. ×10 for one decimal place) not yet confirmed against engine controller datasheet |
| 8 | 40009 | Oil_Press | `Oil_Press` | Oil pressure | uint16 — units/scaling not yet confirmed |
| 9 | 40010 | Oil_Temp | `Oil_Temp` | Oil temperature | uint16 — units/scaling not yet confirmed |
| 10 | 40011 | Cool_Temp | `Cool_Temp` | Coolant temperature | uint16 — units/scaling not yet confirmed |
| 11 | 40012 | Alarm_State1 | `Alarm_state1` | Alarm/fault status, bitfield 1 | uint8 bitfield — bit meanings not yet documented |
| 12 | 40013 | Alarm_State2 | `Alarm_state2` | Alarm/fault status, bitfield 2 | uint8 bitfield |
| 13 | 40014 | (unused) | — | Always 0 — allocated in `holdingRegs[]` but not populated by the parser | reserved |
| 14 | 40015 | (unused) | — | Always 0 — reserved, part of `HOLDING_REGS_SIZE = 16` | reserved |
| 15 | 40016 | (unused) | — | Always 0 — reserved | reserved |

## Behavior notes

- **All registers reset to 0** if no valid frame has been received on the RS-485 link (`Serial`/USART1/J2) for more than 3 seconds — this is the firmware's built-in "stale data" watchdog (`reset_regs_to_zero()`), not a communication fault on the Modbus TCP side itself.
- Registers update **only when a full, header-valid 28-byte frame is parsed** from the engine controller (`0xEB 0x90` repeated 3× at the start of the frame). Partial/garbled frames do not update any register.
- **Bitfield registers (2–5, 11–12):** the individual bit meanings (e.g., which bit = "low oil pressure alarm", which bit = "run relay energized") are not documented in the source code comments — these need to be confirmed against the engine controller's own protocol/wiring datasheet before being used for alarm decoding on the SCADA/HMI side.
- **Scaled values (7–10):** battery voltage, oil pressure, oil temp, and coolant temp are transmitted as raw 16-bit integers by the engine controller. The actual engineering-unit scale factor (e.g., raw ÷ 10 = volts, or a lookup table) depends on that controller's own protocol spec — worth getting that documentation from the controller manufacturer to interpret these correctly rather than assuming a scale.

## Example: reading via ModScan (as already tested)

- Address Base: 0-based
- Start Address: 00000
- Length: 13
- Point Type: 03 HOLDING REGISTER
- Device Id: 2


Reg	ModScan shows	Expected (running scenario)	Match
40001 (RunHours)	1236	started at 1234, incremented as minutes wrapped	
40004 (Relay1)	1	0b00000001 fixed in running scenario	
40006 (RPM)	1493	1500 ± 15 → range 1485–1515	
40007 (Bat)	132	fixed default (untouched in running scenario)	
40008 (OilP)	392	400 ± 10 → range 390–410	
40009 (OilT)	200	fixed default (untouched)	
40010 (CoolT)	799	800 ± 5 → range 795–805	

