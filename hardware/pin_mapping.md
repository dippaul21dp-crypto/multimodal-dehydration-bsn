# Hardware Interfacing and Pin Configuration

This document specifies the hardware wiring, communication protocols, and electrical requirements for the multimodal Body Sensor Network.

## Microcontroller Pin Assignment

| Sensor / Peripheral | Arduino Uno Pin | Protocol / Interface | Operating Voltage | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **MAX30102** | `A4` (SDA), `A5` (SCL) | I2C | 3.3V / 5V | Optical pulse and PPG acquisition |
| **DS18B20** | `D2` | 1-Wire | 5V | Requires a 4.7kΩ pull-up resistor between VCC and Data |
| **GSR Sensor** | `A0` | Analog In | 5V | Measures electrodermal resistance across skin electrodes |
| **HC-05 (TX)** | `D10` | SoftSerial (RX) | 5V | Receives data from module |
| **HC-05 (RX)** | `D11` | SoftSerial (TX) | 3.3V (Logic level) | Transmits data to module; use a voltage divider if needed |
| **Active Buzzer** | `D8` | Digital Out | 5V | Local auditory alarm triggered at DHI ≥ 0.55 |
| **Status LED** | `D13` | Digital Out | 5V (Built-in) | Local visual indicator triggered during strain |

## Circuit Construction Notes

1. **DS18B20 1-Wire Bus**: Connect a 4.7kΩ pull-up resistor between the signal pin (`D2`) and `5V` power rail to guarantee signal integrity.
2. **HC-05 Logic Levels**: The HC-05 RX pin operates at 3.3V logic. When driving from Arduino `D11`, a simple 1kΩ / 2kΩ voltage divider is recommended.
3. **Common Ground**: Ensure all sensor ground lines share a common ground with the Arduino Uno.
