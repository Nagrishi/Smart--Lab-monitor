# Smart Lab Monitor — Test Plan

## 1. Purpose

This document defines the verification plan for the Smart Lab Monitor system.

Testing will cover:
- Sensor accuracy and failure handling
- ESP32 firmware behaviour
- REST and MQTT communication
- Backend and database operations
- Alert rules and local notifications
- Dashboard integration
- Power management and OTA updates

Tests must be recorded with their actual results and supporting evidence. A planned test must not be marked as passed until it has been executed.

## 2. Test Status Convention

| Status | Meaning |
|---|---|
| Not Started | Test has not been executed |
| In Progress | Test is currently being executed |
| Passed | Expected result was achieved |
| Failed | Actual result did not meet expectations |
| Blocked | Test cannot proceed because of a dependency |

Previously demonstrated prototype functionality should be retested and supported with current evidence.

## 3. Sensor Tests

| Test ID | Component | Test procedure | Expected result |
|---|---|---|---|
| SEN-01 | DHT11 | Read temperature and humidity repeatedly | Valid readings are received |
| SEN-02 | DHT11 | Disconnect sensor and attempt a read | Firmware detects the failed read |
| SEN-03 | DHT22 | Repeat temperature and humidity tests after integration | Stable readings within the sensor's specified accuracy |
| SEN-04 | PZEM-004T | Compare readings against suitable reference instruments and loads | Voltage, current, power and energy readings are plausible |
| SEN-05 | MQ-2 | Record raw ADC values under controlled conditions | Raw readings are recorded consistently |
| SEN-06 | MQ-2 | Evaluate baseline and calibration behaviour | Threshold decisions use documented calibration assumptions |
| SEN-07 | LD2410 | Test occupied and unoccupied conditions | Presence state is reported correctly |
| SEN-08 | LD2410 | Test different distances and movement conditions | Detection behaviour and false positives are documented |

**Important sensor notes:**
- DHT11 is the current prototype sensor; DHT22 is planned for the final configuration.
- MQ-2 ADC readings must not be reported as gas concentration in ppm without appropriate calibration.
- Sensor failures must be represented as unavailable or invalid data, not silently converted into zero.
- PZEM mains-side testing must use appropriate isolation, enclosure and protection. Mains wiring and live testing must be performed by a qualified person.

## 4. Firmware Tests

| Test ID | Test procedure | Expected result |
|---|---|---|
| FW-01 | Power on the ESP32 | Firmware starts without unexpected reset |
| FW-02 | Verify sensor initialization | Available sensors initialize correctly |
| FW-03 | Disconnect a sensor | Firmware continues operating and reports the sensor fault |
| FW-04 | Verify OLED output | Current readings and system status are displayed |
| FW-05 | Trigger warning condition | Correct local warning is generated |
| FW-06 | Trigger alert condition | Buzzer and LED respond correctly |
| FW-07 | Disconnect Wi-Fi | Local monitoring and alerts continue |
| FW-08 | Restore Wi-Fi | Communication resumes without requiring an unnecessary manual reset |

## 5. REST API and Communication Tests

### 5.1 REST communication

| Test ID | Test procedure | Expected result |
|---|---|---|
| COM-01 | Connect ESP32 to Wi-Fi | Device obtains network connectivity |
| COM-02 | Send telemetry to the documented REST endpoint | Backend receives the request |
| COM-03 | Send valid telemetry JSON | Backend validates and accepts the payload |
| COM-04 | Send malformed JSON | Backend returns an appropriate error |
| COM-05 | Send missing required fields | Backend rejects invalid data |
| COM-06 | Send telemetry with unavailable sensor values | Backend handles null or invalid readings correctly |
| COM-07 | Submit the same message ID twice | Duplicate handling follows the API contract |
| COM-08 | Disconnect backend or network | Device handles communication failure safely |
| COM-09 | Restore backend connectivity | Telemetry transmission resumes according to the retry policy |

The existing prototype uses `POST /api/sensor`. The planned versioned API uses `POST /api/v1/telemetry`. Test the actual implemented endpoint and document any migration differences.

### 5.2 MQTT communication — planned

| Test ID | Test procedure | Expected result |
|---|---|---|
| MQTT-01 | Connect device to MQTT broker using TLS | Secure connection is established |
| MQTT-02 | Publish telemetry to the documented topic | Broker receives the message |
| MQTT-03 | Publish device status | Status message is received |
| MQTT-04 | Disconnect the broker | Local monitoring remains operational |
| MQTT-05 | Restore broker connectivity | Device reconnects according to policy |
| MQTT-06 | Send equivalent REST and MQTT telemetry | Both transports use the same data contract |

Proposed topic structure:

- `smartlab/{device_id}/telemetry`
- `smartlab/{device_id}/status`
- `smartlab/{device_id}/availability`

## 6. Backend and Database Tests

| Test ID | Test procedure | Expected result |
|---|---|---|
| DB-01 | Send valid telemetry to Flask | Backend processes the request |
| DB-02 | Verify SQLite prototype storage | Telemetry is persisted correctly |
| DB-03 | Restart backend and inspect stored data | Previously stored data remains available |
| DB-04 | Submit invalid sensor values | Invalid data is rejected or handled according to validation rules |
| DB-05 | Verify device identification | Telemetry is associated with the correct device |
| DB-06 | Verify timestamps | Timestamps are stored consistently in UTC |
| DB-07 | Verify duplicate message handling | Duplicate messages do not create unintended duplicate records |
| DB-08 | Test PostgreSQL integration | Data is stored and retrieved correctly after migration |
| DB-09 | Simulate database unavailability | Backend handles the failure without falsely reporting successful persistence |

SQLite is retained for the prototype. PostgreSQL tests apply after the production database integration is implemented.

## 7. Alert and Rule Tests

The system must distinguish normal operation, warning conditions and alert conditions.

| Test ID | Scenario | Expected result |
|---|---|---|
| ALT-01 | Occupied laboratory with normal power consumption | NORMAL |
| ALT-02 | Unoccupied laboratory with low or idle power consumption | NORMAL |
| ALT-03 | Unoccupied laboratory with power above the configured idle baseline for the configured duration | WARNING or ALERT according to the rule configuration |
| ALT-04 | Temperature exceeds the configured threshold | Appropriate temperature warning |
| ALT-05 | MQ-2 reading exceeds the configured threshold | Appropriate gas/smoke warning |
| ALT-06 | Multiple warning conditions occur together | All relevant conditions are represented |
| ALT-07 | Sensor reading becomes invalid | Sensor fault is reported; invalid data does not silently trigger a normal state |

Absence alone must not be treated as proof of energy wastage. The system should evaluate occupancy together with power consumption, baseline and configured duration.

Thresholds must be configurable and documented rather than assumed to be universal values.

## 8. Local Alert and Display Tests

| Test ID | Test procedure | Expected result |
|---|---|---|
| LOC-01 | Verify normal operating condition | OLED displays normal status |
| LOC-02 | Trigger warning | Correct warning appears on OLED |
| LOC-03 | Trigger alert | Buzzer and LED indicate alert |
| LOC-04 | Disconnect Wi-Fi during an alert | Local alert continues |
| LOC-05 | Stop backend during an alert | Local alert continues |
| LOC-06 | Restore network connectivity | Local monitoring remains stable |

**Critical requirement:** Local safety-related monitoring and alerting must not depend on cloud, broker, database or dashboard availability.

## 9. Dashboard Tests — planned

| Test ID | Test procedure | Expected result |
|---|---|---|
| UI-01 | Open dashboard | Current system status is displayed |
| UI-02 | Verify live sensor values | Values match backend data |
| UI-03 | Open historical data | Stored readings are displayed |
| UI-04 | Verify timestamps | Time and date are represented correctly |
| UI-05 | Simulate API failure | Dashboard displays a clear error or unavailable