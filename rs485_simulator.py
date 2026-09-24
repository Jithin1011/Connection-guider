"""
rs485_simulator.py

Simulate KCM-IIG/A telemetry frames and send them over a serial port (USB-RS485 adapter or direct TTL).

Usage:
  python rs485_simulator.py COM_PORT [--baud 9600] [--interval 1.0]
                              [--addr 2] [--comm 0x11]
                              [--rpm START] [--bat START]

Examples:
  python rs485_simulator.py COM4 --baud 9600 --interval 2.0

Notes:
- Use the COM port of your USB-RS485 adapter. The adapter handles DE/RE automatically.
- For direct TTL to MCU RX (e.g. PA10), use a USB-TTL adapter and connect TX->PA10, GND->GND.

Frame format (28 bytes):
  EB 90 EB 90 EB 90 [ADDR] [COMM] [LEN=19] D0..D18

D0 = run minutes
D1 = run hours low
D2 = run hours high
D3 = lights1
D4 = lights2
D5 = relays1
D6 = relays2
D7 D8 = rpm (high, low)
D9 D10 = battery (high, low)
D11 D12 = oil pressure (high, low)
D13 D14 = oil temp (high, low)
D15 D16 = coolant temp (high, low)
D17 = alarm1
D18 = alarm2
"""

import serial
import time
import argparse


def build_frame(addr, comm, run_mins, run_hours, lights1, lights2, relay1, relay2,
                rpm, battery, oil_press, oil_temp, cool_temp, alarm1, alarm2):
    frame = bytearray(28)
    # header
    frame[0:6] = bytes([0xEB,0x90,0xEB,0x90,0xEB,0x90])
    frame[6] = addr & 0xFF
    frame[7] = comm & 0xFF
    frame[8] = 19  # data length
    frame[9] = run_mins & 0xFF
    frame[10] = run_hours & 0xFF
    frame[11] = (run_hours >> 8) & 0xFF
    frame[12] = lights1 & 0xFF
    frame[13] = lights2 & 0xFF
    frame[14] = relay1 & 0xFF
    frame[15] = relay2 & 0xFF
    # rpm
    frame[16] = (rpm >> 8) & 0xFF
    frame[17] = rpm & 0xFF
    # battery
    frame[18] = (battery >> 8) & 0xFF
    frame[19] = battery & 0xFF
    # oil pressure
    frame[20] = (oil_press >> 8) & 0xFF
    frame[21] = oil_press & 0xFF
    # oil temp
    frame[22] = (oil_temp >> 8) & 0xFF
    frame[23] = oil_temp & 0xFF
    # coolant temp
    frame[24] = (cool_temp >> 8) & 0xFF
    frame[25] = cool_temp & 0xFF
    # alarms
    frame[26] = alarm1 & 0xFF
    frame[27] = alarm2 & 0xFF
    return frame


def main():
    p = argparse.ArgumentParser(description='KCM-IIG/A RS485 frame simulator')
    p.add_argument('port', help='COM port (e.g. COM4)')
    p.add_argument('--baud', type=int, default=9600, help='baud rate')
    p.add_argument('--interval', type=float, default=2.0, help='seconds between frames')
    p.add_argument('--addr', type=int, default=2, help='machine address')
    p.add_argument('--comm', type=lambda x: int(x,0), default=0x11, help='COMM byte (hex ok)')
    p.add_argument('--rpm', type=int, default=1000, help='starting RPM')
    p.add_argument('--bat', type=int, default=2400, help='battery value (e.g. 2400 => 24.00V)')
    p.add_argument('--minutes', type=int, default=0, help='starting run minutes')
    p.add_argument('--hours', type=int, default=0, help='starting run hours')
    args = p.parse_args()

    try:
        ser = serial.Serial(args.port, args.baud, timeout=1)
    except Exception as e:
        print('Failed to open serial port', args.port, e)
        return

    run_mins = args.minutes
    run_hours = args.hours
    rpm = args.rpm
    battery = args.bat
    lights1 = 0x00
    lights2 = 0x04  # running bit set
    relay1 = 0x00
    relay2 = 0x00
    oil_press = 50
    oil_temp = 1050
    cool_temp = 1050
    alarm1 = 0x00
    alarm2 = 0x00

    print('Sending frames on', args.port, 'baud', args.baud)
    try:
        while True:
            frame = build_frame(args.addr, args.comm, run_mins, run_hours, lights1, lights2, relay1, relay2,
                                rpm, battery, oil_press, oil_temp, cool_temp, alarm1, alarm2)
            ser.write(frame)
            ser.flush()
            print('Sent:', frame.hex(' '))
            # advance some demo values
            run_mins += 1
            if run_mins >= 60:
                run_mins = 0
                run_hours += 1
            rpm = (rpm + 37) % 5000
            battery = 2400 + ((rpm // 100) % 50)
            time.sleep(args.interval)
    except KeyboardInterrupt:
        print('\nStopped')
    finally:
        ser.close()

if __name__ == '__main__':
    main()
