# Engine Data Converter

This project is a STM32F103 Blue Pill based engine data converter and Modbus gateway.

It receives binary engine telemetry from an external controller over RS-485, decodes the payload, updates Modbus registers, exposes them over Ethernet, and drives a 4-digit 7-segment display.

## Features
- STM32F103 based firmware using STM32Duino / Arduino Core
- Ethernet static IP: 192.168.1.10
- MAC: A5:A6:DE:0B:A8:C2
- Modbus TCP server over Ethernet
- RS-485 Modbus RTU on USART1 (PA9/PA10)
- Debug serial on USART3 (PB10/PB11)
- 4-digit 7-segment display via 74HC595
- Telemetry frame parsing for KCM-IIG/A-like packet format

## Hardware wiring summary

### RS-485 UART (USART1)
- PA9 = TX (DI / driver input)
- PA10 = RX (RO / receiver output)
- PA11 = DE/RE (direction control)

### Debug UART (USART3)
- PB10 = TX
- PB11 = RX

### Ethernet CS
- PA4 = Ethernet chip select

### 7-segment display
- PA8
- PB4, PB5, PB12, PB13, PB14, PB15

## Communication details

### Ethernet
- IP: 192.168.1.10
- Modbus TCP port: 502 (typical)
- Unit id: 2 (configured in code)

### RS-485 Modbus RTU
- Baud: 9600
- 8N1
- Slave ID: 2

### Telemetry input frame
The firmware receives a binary frame whose payload begins with a start marker and then engine values such as:
- run minutes / hours
- lamp states
- relay states
- engine speed
- battery voltage
- oil pressure
- oil temperature
- coolant temperature
- alarm states

The code expects a 28-byte telemetry packet with the pattern:
- EB 90 EB 90 EB 90 ...

## Build instructions
1. Open the project folder in VS Code.
2. Install the PlatformIO extension.
3. From PlatformIO, run Build.
4. Upload the firmware using ST-Link.

### PowerShell command line (if PlatformIO CLI is installed)
```powershell
cd "D:\For my refferance\code\try1 engine data cv"
pio run
pio run -t upload
```

## Serial monitor
Use a USB-TTL adapter on PB10/PB11 (USART3) at 115200 baud.

Example:
```powershell
pio device monitor -p COMx -b 115200
```

Expected debug output examples:
- `[BOOT] Engine Data Converter started`
- `[ETH] MAC: ... IP: 192.168.1.10`
- `[HB] alive`
- `[PARSED] RunMins=... RunHours=... RPM=...`

## Testing
A simple Python sender script can be used to inject a test frame on the RS-485 bus.

The project folder includes example scripts in the reference copy, but this folder currently focuses on the core firmware.

## Notes
- The current code is intended for engine telemetry received from an external generator controller / genset ECU.
- Some values and byte ordering should be verified against the real sensor device datasheet because scaling and endianness can vary.
- If the device sends different frame lengths or a different header byte sequence, update `In_data_len` and the parser block in `src/main.cpp`.
