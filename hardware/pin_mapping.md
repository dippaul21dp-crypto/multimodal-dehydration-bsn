# Hardware Interfacing and Pin Configuration

This document specifies the hardware wiring, communication protocols, and electrical requirements for the multimodal Body Sensor Network.

## Microcontroller Pin Assignment

| Sensor / Peripheral | Arduino Uno Pin | Protocol / Interface | Operating Voltage | Notes |
| :--- | :--- | :--- | :--- | :--- |
| **MAX30102** | `A4` (SDA), `A5` (SCL)[cite: 1] | I2C[cite: 1] | 3.3V / 5V | Optical pulse and PPG acquisition[cite: 1]. |
| **DS18B20** | `D2`[cite: 1] | 1-Wire[cite: 1] | 5V | Requires a 4.7kΩ pull-up resistor between VCC and Data[cite: 1]. |
| **GSR Sensor** | `A0`[cite: 1] | Analog In[cite: 1] | 5V | Measures electrodermal resistance across skin electrodes[cite: 1]. |
| **HC-05 (TX)** | `D10`[cite: 1] | SoftSerial (RX)[cite: 1] | 5V | Receives data from module[cite: 1]. |
| **HC-05 (RX)** | `D11`[cite: 1] | SoftSerial (TX)[cite: 1] | 3.3V (Logic level)[cite: 1] | Transmits data to module; use a voltage divider if needed[cite: 1]. |
| **Active Buzzer** | `D8`[cite: 1] | Digital Out[cite: 1] | 5V | Local auditory alarm triggered at DHI ≥ 0.55[cite: 1]. |
| **Status LED** | `D13`[cite: 1] | Digital Out[cite: 1] | 5V (Built-in)[cite: 1] | Local visual indicator triggered during strain[cite: 1]. |

## Circuit Construction Notes

1. **DS18B20 1-Wire Bus**: Connect a 4.7kΩ pull-up resistor between the signal pin (`D2`) and `5V` power rail to guarantee signal integrity[cite: 1].
2. **HC-05 Logic Levels**: The HC-05 RX pin operates at 3.3V logic. When driving from Arduino `D11`, a simple 1kΩ / 2kΩ voltage divider is recommended.
3. **Common Ground**: Ensure all sensor ground lines share a common ground with the Arduino Uno.
