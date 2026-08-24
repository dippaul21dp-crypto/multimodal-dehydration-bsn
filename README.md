# Multimodal Body Sensor Network for Real-Time Dehydration Monitoring

[![Platform: Arduino](https://img.shields.io/badge/Platform-Arduino%20Uno-blue.svg)](https://www.arduino.cc/)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B%20%2F%20Wiring-orange.svg)](https://www.arduino.cc/)
[![License: MIT](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)
[![Protocol: I2C | 1-Wire | SoftSerial](https://img.shields.io/badge/Protocols-I2C%20%7C%201--Wire%20%7C%20UART-brightgreen.svg)]()

A wearable Body Sensor Network (BSN) designed to evaluate clinical hydration status in real-time using edge-computed sensor fusion. By combining cardiovascular, thermal, and electrodermal biosignals on an Arduino Uno edge node, the system computes a continuous **Dehydration Hazard Index (DHI)** and broadcasts real-time diagnostic telemetry via Bluetooth.

## System Architecture

```mermaid
flowchart TD
    subgraph SENSORS ["1. Biosensing Layer"]
        S1["MAX30102 Pulse Sensor<br/>(Heart Rate / PPG)"]
        S2["DS18B20 Temp Probe<br/>(Skin Temperature)"]
        S3["GSR Transducer<br/>(Electrodermal Conductance)"]
    end

    subgraph EDGE ["2. Edge Processing Node (Arduino Uno)"]
        direction TB
        P1["Signal Conditioning<br/>(300 ms Refractory Filter & IR Threshold Gate)"]
        P2["Feature Extraction & 10s Window Averaging"]
        P3["Physiological Scale Normalization (0.0 – 1.0)"]
        P4["Weighted Sensor Fusion Engine<br/><b>DHI = 0.70·N_GSR + 0.15·N_HR + 0.15·N_Temp</b>"]
        P5{"DHI ≥ 0.55?"}
        
        P1 --> P2 --> P3 --> P4 --> P5
    end

    subgraph OUTPUT ["3. Actuation & Telemetry"]
        A1["Active Buzzer (D8)<br/>Auditory Alarm"]
        A2["Status LED (D13)<br/>Visual Indicator"]
        BT["HC-05 Bluetooth Module (D10/D11)<br/>SoftwareSerial Telemetry"]
        APP[("Mobile Terminal App<br/>Real-Time Data Display")]
    end

    S1 -->|"I2C (A4/A5)"| P1
    S2 -->|"1-Wire (D2)"| P1
    S3 -->|"Analog (A0)"| P1

    P5 -- "Yes (Strain Detected)" --> A1
    P5 -- "Yes (Strain Detected)" --> A2
    P4 -->|"10-Second Periodic Stream"| BT
    BT -->|"Wireless 9600 Baud"| APP
