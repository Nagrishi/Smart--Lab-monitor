# Hardware Design and Interface Plan — Smart Lab Monitor

## 1. Purpose

This document describes the hardware components, their roles, communication interfaces, and development status for the Smart Lab Monitor.

The system monitors laboratory electrical power, environmental conditions, and occupancy. It also provides local alerts and sends telemetry to the backend.

## 2. Main Controller

**Controller:** ESP32 DevKit

Responsibilities:

- Read sensor measurements.
- Validate sensor readings.
- Execute local monitoring and alert rules.
- Update the OLED display.
- Control buzzer and LED indicators.
- Communicate with the backend through Wi-Fi.
- Support REST communication first and MQTT over TLS in a later integration stage.

The exact GPIO assignments must be documented after the final wiring is tested.

## 3. Sensor and Display Hardware

| Component | Purpose | Interface | Development status |
|---|---|---|---|
| PZEM-004T-100A | Voltage, current, power and energy measurement | UART | Integration/testing required |
| DHT11 | Current prototype temperature and humidity | Digital | Prototype demonstrated |
| DHT22 | Final temperature and humidity sensor | Digital | Planned |
| MQ-2 | Gas/smoke sensing | ADC | Testing/integration required |
| LD2410 | Human presence detection | UART | Testing/integration required |
| SSD1306 OLED | Local readings and status display | I2C | Prototype demonstrated |

**Important:** DHT11 is currently used in the prototype. DHT22 is the planned final sensor. Do not describe them as interchangeable or claim final DHT22 integration until it has been tested.

## 4. Local Alert Hardware

The local alert subsystem consists of:

- SSD1306 OLED display.
- Buzzer.
- LED indicators.

Expected behavior:

- Display sensor readings and system status.
- Provide audible warnings through the buzzer.
- Use LEDs to indicate operating or alert conditions.

The final LED color mapping, buzzer patterns, and alert thresholds must be documented after firmware testing.

Local alerts must continue operating even when Wi-Fi, the backend, MQTT broker, or cloud database is unavailable.

## 5. Communication Interfaces

| Interface | Components | Purpose |
|---|---|---|
| UART | PZEM-004T | Electrical measurements |
| UART | LD2410 | Presence information |
| Digital GPIO | DHT11/DHT22 | Temperature and humidity |
| ADC | MQ-2 | Analog sensor measurement |
| I2C | SSD1306 OLED | Local display |
| Wi-Fi | ESP32 | Network communication |

UART pin assignments, I2C pins, ADC pins, baud rates, and GPIO configuration must be recorded in the firmware documentation after verification on the physical board.

## 6. Power Subsystem

The proposed design includes:

- IRLZ44N MOSFET for sensor-rail power control.
- A proposed 18650 Li-ion battery subsystem.
- TP4056 charging module as a proposed charging component.

These components are part of the design plan and must not be considered a validated power system until the complete circuit has been tested.

Before finalizing the battery subsystem, verify:

- Battery cell configuration and protection.
- Charger compatibility with the selected battery configuration.
- Required voltage and current for every load.
- ESP32 regulator input requirements.
- MOSFET voltage drop and thermal performance.
- Safe charging and discharge protection.

Do not connect a battery pack or charging circuit until its electrical ratings and protection requirements have been verified.

## 7. Hardware Safety

### PZEM-004T

PZEM measurement installation may involve hazardous mains voltage.

- Use a properly rated insulated enclosure.
- Provide appropriate electrical protection and isolation.
- Do not touch exposed mains conductors.
- Do not perform live wiring or modifications.
- Have mains-side installation checked and performed by a qualified person.

### Battery subsystem

- Avoid short circuits and reverse polarity.
- Use suitable cell protection.
- Do not charge damaged or unsuitable cells.
- Verify charger and battery compatibility before operation.

### Sensor power control

- Verify MOSFET pinout and electrical ratings.
- Check load current and temperature.
- Confirm that sensor power switching does not disturb ESP32 operation.

## 8. Current Prototype vs Final Hardware

### Demonstrated prototype

- ESP32 controller.
- DHT-based temperature/humidity sensing.
- SSD1306 OLED.
- Wi-Fi communication.
- Flask backend communication.
- SQLite data persistence.

### Planned final hardware

- PZEM-004T-100A.
- DHT22.
- MQ-2.
- LD2410.
- OLED, buzzer, and LEDs.
- Sensor power management.
- Battery and low-power subsystem.

A component being available or connected does not automatically mean its complete measurement and alert functionality has been validated.

## 9. Hardware Validation Checklist

- [ ] Verify ESP32 power supply and stable boot.
- [ ] Verify DHT sensor readings and failed-read handling.
- [ ] Verify OLED display and I2C communication.
- [ ] Verify PZEM UART communication using safe test conditions.
- [ ] Verify MQ-2 ADC readings and document calibration limitations.
- [ ] Verify LD2410 presence detection.
- [ ] Verify buzzer operation.
- [ ] Verify LED status indications.
- [ ] Verify sensor power switching.
- [ ] Verify complete battery subsystem before deployment.
- [ ] Verify local alerts during network failure.

Record actual wiring, measured values, and test evidence in the project test plan.

## 10. Design Principle

The ESP32 is responsible for sensing, local processing, and immediate alerts. Backend and cloud services provide remote monitoring and historical storage.

A network or database failure must not disable the local safety-alert functionality.