# Product Requirements Document (PRD)
## Smart Lab Monitor

**Project title:** IoT-Based Aggregate Power Monitoring and Safety Alert System for the Electronics Laboratory

## 1. Product Overview

Smart Lab Monitor is an IoT-based laboratory monitoring system designed to monitor aggregate electrical power consumption, environmental conditions, and human presence.

The system combines ESP32-based sensing, local alert processing, network communication, backend services, and historical data storage.

Its primary goal is to improve laboratory energy awareness and provide timely local safety alerts.

## 2. Problem Statement

Electronics laboratories contain equipment that may remain powered after working sessions. This can result in unnecessary energy consumption.

Laboratory activities may also involve elevated temperatures and gas or smoke conditions that require attention.

Manual supervision cannot continuously monitor every laboratory area or provide remote access to historical measurements.

Smart Lab Monitor aims to address these challenges through automated monitoring and alerts.

## 3. Product Objectives

1. Monitor aggregate electrical voltage, current, power, and energy.
2. Monitor environmental temperature and humidity.
3. Detect gas/smoke conditions using an appropriate sensor.
4. Detect human presence in the monitored area.
5. Correlate occupancy with power consumption to identify possible energy wastage.
6. Execute immediate monitoring and alert rules locally on the ESP32.
7. Display readings and system status locally.
8. Send telemetry to a backend for remote monitoring and storage.
9. Maintain historical sensor and alert records.
10. Support future OTA updates and low-power operation.

## 4. Target Users

- Students using the electronics laboratory.
- Laboratory technicians and staff.
- Faculty members and laboratory supervisors.

## 5. Functional Requirements

| ID | Requirement | Description |
|---|---|---|
| FR-01 | Power monitoring | PZEM-004T measures aggregate voltage, current, power, and energy. |
| FR-02 | Environmental monitoring | DHT22 measures temperature and humidity. |
| FR-03 | Gas/smoke monitoring | MQ-2 provides gas/smoke-related sensor readings. |
| FR-04 | Occupancy detection | LD2410 detects human presence. |
| FR-05 | Energy wastage detection | Unoccupied conditions combined with power above a configured idle baseline for a configured duration indicate possible energy wastage. |
| FR-06 | Safety monitoring | Configured temperature or gas/smoke threshold crossings generate alerts. |
| FR-07 | Local rules engine | Alert decisions execute on the ESP32 so local monitoring continues during network outages. |
| FR-08 | Local output | OLED, buzzer, and LEDs communicate readings and status. |
| FR-09 | REST communication | ESP32 sends telemetry to the Flask backend through HTTPS REST in the secured deployment. |
| FR-10 | MQTT communication | MQTT over TLS provides an additional telemetry transport for remote monitoring. |
| FR-11 | Data storage | PostgreSQL is the planned production/cloud database. SQLite remains in use for the current prototype. |
| FR-12 | Data retrieval | Backend APIs provide current readings, historical measurements, and alert information. |
| FR-13 | Local network access | ESP32 local web and JSON access with mDNS are planned. |
| FR-14 | Network configuration | Wi-Fi Manager or another self-configuration method is planned. |
| FR-15 | OTA updates | HTTPS-based firmware updates are planned. |
| FR-16 | Low-power operation | Sensor power control and deep-sleep/wake operation are planned. |

## 6. Non-Functional Requirements

### Reliability
- Local alerting must continue during Wi-Fi, backend, MQTT broker, or database outages.
- Sensor failures must not silently produce misleading readings.
- Network failures must be handled without stopping local monitoring.

### Performance
- Sensor readings should be collected at a configurable interval.
- Local status updates should be responsive.
- Backend telemetry processing should avoid unnecessary duplicate records.

### Security
- Secure network communication using HTTPS and MQTT over TLS.
- Authenticate device communication.
- Keep credentials and secrets outside source-controlled files.
- Validate incoming telemetry before storage.

### Maintainability
- Use modular firmware components.
- Keep sensor interfaces and backend services organized.
- Maintain a version-controlled codebase.
- Document data fields, units, and interfaces.

### Scalability
- Support multiple monitoring devices in the future.
- Use device identifiers to separate telemetry.
- Keep frontend communication independent of database implementation.

## 7. Data and Communication Requirements

Both REST and MQTT must use the same telemetry JSON contract defined in `DATA_FORMAT.md`.

The backend must:

1. Receive telemetry.
2. Validate the payload.
3. Verify the device identity.
4. Handle duplicate messages.
5. Store valid telemetry.
6. Update device last-seen information.
7. Record relevant alert events.

The future frontend must consume backend APIs. It must not connect directly to PostgreSQL or open the SQLite database file.

## 8. Alert and Energy-Wastage Logic

### Energy wastage

Possible energy wastage is identified when:

- No human presence is detected.
- Power remains above a configurable idle baseline.
- The condition persists for a configured duration.

Absence alone does not prove energy wastage.

### Safety alerts

Temperature and gas/smoke measurements are compared with configured thresholds.

The system should provide consistent status categories:

- `NORMAL`
- `WARNING`
- `ALERT`

Threshold values must be configurable and validated during testing.

MQ-2 raw ADC readings must not be presented as gas concentration in ppm unless a suitable calibration method has been implemented and validated.

## 9. Success Criteria

The project will be considered successful when:

1. Sensor measurements are reliable and validated.
2. Local alert logic responds correctly to configured conditions.
3. OLED, buzzer, and LED outputs behave as expected.
4. Local alerts continue during network failure.
5. ESP32 telemetry reaches the backend.
6. Valid telemetry is stored without duplicate records.
7. Historical data can be retrieved through backend APIs.
8. REST and MQTT use the same telemetry contract.
9. The frontend can display current readings, historical measurements, and alerts.

## 10. Current Prototype vs Final Target

### Current demonstrated prototype

- ESP32 controller.
- DHT-based temperature and humidity sensing.
- SSD1306 OLED.
- Wi-Fi connectivity.
- Flask backend.
- `POST /api/sensor`.
- SQLite database persistence.

### Final target

- PZEM-004T-100A.
- DHT22.
- MQ-2.
- LD2410.
- ESP32 local rules engine.
- OLED, buzzer, and LED alerts.
- HTTPS REST and MQTT over TLS.
- Flask backend with PostgreSQL.
- Historical data and alert APIs.
- Future web frontend.
- OTA and low-power operation.

## 11. Development Priorities

Implementation will follow this sequence:

1. Finalize and verify project documentation.
2. Review the existing firmware and backend.
3. Stabilize the current ESP32-to-Flask-to-SQLite prototype.
4. Integrate and validate sensors individually.
5. Implement local alert and energy-wastage rules.
6. Implement the versioned REST API.
7. Migrate storage to PostgreSQL after testing.
8. Add MQTT over TLS.
9. Develop the frontend.
10. Test reliability, OTA, and low-power operation.

## 12. Scope Management

Features marked as planned must not be reported as completed until implementation and testing evidence are available.

The current prototype must remain functional while new capabilities are introduced incrementally.