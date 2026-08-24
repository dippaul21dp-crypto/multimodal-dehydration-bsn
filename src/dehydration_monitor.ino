#include <Wire.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "MAX30105.h"

// --- Pin Definitions ---
#define ONE_WIRE_BUS 2     // DS18B20 Data Pin
#define BUZZER_PIN 8       // Active Buzzer Pin
#define BT_RX_PIN 10       // Arduino RX (Connect to HC-05 TX)
#define BT_TX_PIN 11       // Arduino TX (Connect to HC-05 RX)
#define LED_PIN 13         // Status LED
#define GSR_PIN A0         // GSR Analog Pin

// --- Constants & Thresholds ---
const long IR_FINGER_THRESHOLD = 20000;      // Minimum IR value for finger detection
const unsigned long REFRACTORY_PERIOD = 300; // 300 ms debounce (~200 BPM limit)
const unsigned long WINDOW_DURATION = 10000; // 10-second data processing window
const float DHI_THRESHOLD = 0.55;            // Moderate strain threshold

// --- Hardware Objects ---
MAX30105 particleSensor;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);
SoftwareSerial BTSerial(BT_RX_PIN, BT_TX_PIN);

// --- Peak Detection Variables ---
long lastIRValue = 0;
bool rising = false;
unsigned long lastPeakTime = 0;
unsigned long intervalSum = 0;
int beatCount = 0;

void setup() {
  Serial.begin(9600);
  BTSerial.begin(9600);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // Initialize DS18B20
  tempSensor.begin();

  // Initialize MAX30102
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 not found. Check wiring/pull-up resistors.");
    while (1);
  }

  // Configure MAX30102 sensor settings
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x0A);
  particleSensor.setPulseAmplitudeGreen(0);
}

void loop() {
  unsigned long windowStartTime = millis();
  intervalSum = 0;
  beatCount = 0;
  lastPeakTime = 0;
  lastIRValue = 0;
  rising = false;

  long gsrSum = 0;
  int sampleCount = 0;

  // Run a 10-second sampling window
  while (millis() - windowStartTime < WINDOW_DURATION) {
    long irValue = particleSensor.getIR();

    // Finger presence validation
    if (irValue > IR_FINGER_THRESHOLD) {
      unsigned long currentTime = millis();

      // Peak-to-peak derivative detection
      if (irValue > lastIRValue) {
        rising = true;
      } else if (rising && (irValue < lastIRValue)) {
        rising = false;

        // Apply 300 ms refractory filter to reject motion artifacts
        if (lastPeakTime > 0) {
          unsigned long delta = currentTime - lastPeakTime;
          if (delta >= REFRACTORY_PERIOD) {
            intervalSum += delta;
            beatCount++;
          }
        }
        lastPeakTime = currentTime;
      }
    }

    // Accumulate GSR readings across the window
    gsrSum += analogRead(GSR_PIN);
    sampleCount++;
    lastIRValue = irValue;
    delay(20);
  }

  // --- Calculate Heart Rate (BPM) ---
  float finalHR = 0.0;
  if (beatCount > 0) {
    float avgIntervalMs = (float)intervalSum / beatCount;
    finalHR = 60000.0 / avgIntervalMs;
  } else {
    finalHR = 0.0;
  }

  // --- Read Temperature (°C) ---
  tempSensor.requestTemperatures();
  float finalTemp = tempSensor.getTempCByIndex(0);

  // --- Read Average GSR ---
  float finalGSR = (sampleCount > 0) ? ((float)gsrSum / sampleCount) : 0.0;

  // --- Compute Normalizations ---
  float n_gsr = finalGSR / 1023.0;
  float n_hr = max(0.0, (finalHR - 40.0) / 160.0);
  float n_temp = max(0.0, (finalTemp - 20.0) / 20.0);

  // --- Sensor Fusion: Dehydration Hazard Index (DHI) ---
  float dhi = (0.70 * n_gsr) + (0.15 * n_hr) + (0.15 * n_temp);

  // --- Determine Status and Drive Alarms ---
  bool isAlarm = (dhi >= DHI_THRESHOLD && finalHR > 0);

  if (isAlarm) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  }

  // --- Transmit Report via Bluetooth & Serial ---
  sendReport(finalHR, finalTemp, finalGSR, dhi, isAlarm);
}

void sendReport(float hr, float temp, float gsr, float dhi, bool alarmState) {
  String statusText = alarmState ? "DEHYDRATION STRAIN" : "NORMAL";

  Stream* targets[] = {&Serial, &BTSerial};
  for (int i = 0; i < 2; i++) {
    targets[i]->println("=======================");
    targets[i]->println("      BSN REPORT       ");
    targets[i]->println("=======================");
    targets[i]->print("HR   : "); targets[i]->print((int)hr); targets[i]->println(" BPM");
    targets[i]->print("Temp : "); targets[i]->print(temp, 1); targets[i]->println(" C");
    targets[i]->print("GSR  : "); targets[i]->println((int)gsr);
    targets[i]->print("DHI  : "); targets[i]->println(dhi, 2);
    targets[i]->println("-----------------------");
    targets[i]->print("STATUS: "); targets[i]->println(statusText);
    targets[i]->println("=======================");
  }
}
