# Project Status — Smart Lab Monitor

## 1. Current Project Overview

Smart Lab Monitor is under active development.

The current demonstrated prototype uses ESP32, DHT-based temperature/humidity sensing, SSD1306 OLED, Wi-Fi, Flask, and SQLite.

Development is proceeding incrementally to preserve existing functionality while adding new capabilities.

## 2. Demonstrated Features

The following features have been demonstrated in the current prototype:

- [x] PlatformIO firmware project.
- [x] ESP32 firmware build and upload.
- [x] DHT-based temperature and humidity sensing.
- [x] SSD1306 OLED display.
- [x] ESP32 Wi-Fi connectivity.
- [x] Flask backend.
- [x] `POST /api/sensor` endpoint.
- [x] ESP32-to-Flask HTTP communication.
- [x] HTTP 200 response in the documented prototype test.
- [x] SQLite database file.
- [x] Sensor data persistence.

## 3. Documentation Progress

- [x] `ARCHITECTURE.md` updated.
- [x] `DATA_FORMAT.md` updated.
- [x] `API.md` updated.
- [x] `DATABASE.md` updated.
- [x] `HARDWARE.md` updated.
- [x] `PRD.md` updated.
- [x] `README.md` updated.
- [x] `ROADMAP.md` updated.
- [x] `SETUP.md` updated.
- [ ] `STATUS.md` finalized.
- [ ] `TEAM_RESPONSIBILITIES.md` updated.
- [ ] `TEST_PLAN.md` updated.

## 4. Hardware Integration Status

| Component | Status |
|---|---|
| ESP32 | Prototype demonstrated |
| DHT11 | Prototype demonstrated |
| DHT22 | Planned final integration |
| SSD1306 OLED | Prototype demonstrated |
| PZEM-004T | Integration and validation required |
| MQ-2 | Testing/integration required |
| LD2410 | Testing/integration required |
| Buzzer | Hardware present; complete alert validation pending |
| LEDs | Hardware present; complete status validation pending |

Component availability, connection, and complete functional validation are separate milestones.

## 5. Firmware Status

| Feature | Status |
|---|---|
| PlatformIO project | Demonstrated |
| ESP32 build/upload | Demonstrated |
| DHT sensor reading | Prototype demonstrated |
| OLED output | Demonstrated |
| PZEM measurement handling | Validation required |
| MQ-2 measurement handling | Validation required |
| LD2410 presence handling | Validation required |
| Unified sensor data structure | Pending review |
| Local monitoring rules | Pending validation |
| NORMAL/WARNING/ALERT logic | Pending validation |
| Buzzer alert patterns | Pending validation |
| LED status logic | Pending validation |
| Sensor power management | Planned |
| Deep-sleep operation | Planned |
| OTA updates | Planned |

## 6. Backend and Database Status

| Feature | Status |
|---|---|
| Flask application | Prototype demonstrated |
| `POST /api/sensor` | Demonstrated |
| ESP32 HTTP communication | Demonstrated |
| SQLite storage | Demonstrated |
| Existing database schema review | Pending |
| Request validation review | Pending |
| Versioned REST endpoint | Planned |
| Shared telemetry processing service | Planned |
| PostgreSQL schema | Designed, not implemented |
| PostgreSQL integration | Planned |
| MQTT worker | Planned |
| MQTT over TLS | Planned |
| Device authentication | Planned |
| Latest telemetry API | Planned |
| Historical telemetry API | Planned |
| Alert history API | Planned |

## 7. Frontend Status

Frontend development is deferred until the backend API and telemetry contract are stable.

Planned features:

- [ ] Current sensor readings.
- [ ] Temperature and humidity display.
- [ ] Electrical measurements.
- [ ] Occupancy status.
- [ ] NORMAL/WARNING/ALERT indicators.
- [ ] Historical charts.
- [ ] Alert history.
- [ ] Responsive dashboard.

## 8. Architecture Decisions

The following decisions have been selected:

| Area | Decision |
|---|---|
| Main controller | ESP32 |
| Firmware framework | Arduino framework with PlatformIO |
| Backend | Python Flask |
| REST transport | HTTPS REST for secured deployment |
| Messaging | MQTT over TLS |
| Production database | PostgreSQL |
| Current prototype database | SQLite |
| Frontend | Deferred |
| Local alert processing | ESP32 |

Local monitoring and alerting must continue even when network or cloud services are unavailable.

## 9. Current Development Milestone

**Milestone:** Documentation finalization and existing prototype review.

Next activities:

1. Complete remaining documentation.
2. Review existing `backend/app.py`.
3. Review `database/database.py`.
4. Review firmware source files.
5. Verify the existing prototype.
6. Identify and resolve implementation issues.
7. Begin incremental feature integration.

## 10. Known Implementation Boundaries

- DHT11 is the current prototype sensor; DHT22 is the planned final sensor.
- MQ-2 raw ADC values are not calibrated gas concentration measurements.
- PostgreSQL has not yet replaced SQLite.
- MQTT over TLS is not yet implemented.
- The versioned REST API is a design target, not a demonstrated endpoint.
- Frontend development is deferred.
- Planned OTA and low-power features are not yet demonstrated.

## 11. Evidence and Completion Rules

A feature may be marked complete only after:

1. Implementation is available in the project.
2. The feature has been tested on the intended hardware or software environment.
3. Expected behavior has been observed.
4. Relevant logs, measurements, or screenshots have been recorded.
5. The test result is documented in `TEST_PLAN.md`.

Do not mark a feature complete solely because its documentation or code has been written.

## 12. Status Maintenance

Update this document whenever an implementation milestone changes.

Keep demonstrated, in-progress, and planned features clearly separated.