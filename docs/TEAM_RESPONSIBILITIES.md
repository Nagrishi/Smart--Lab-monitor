# Team Responsibilities — Smart Lab Monitor

## 1. Purpose

This document defines the responsibilities of team members working on the Smart Lab Monitor project.

The objective is to establish clear ownership of hardware, firmware, backend, database, frontend, documentation, and integration activities.

## 2. Project Workstreams

The project is divided into the following workstreams:

1. Embedded hardware and firmware.
2. Backend and database.
3. Frontend and dashboard.
4. System integration and testing.
5. Documentation and project management.

## 3. Embedded Hardware and Firmware

### Responsibilities

- ESP32 firmware development using PlatformIO.
- Sensor interfacing and validation.
- GPIO, UART, I2C, and ADC configuration.
- DHT sensor integration.
- PZEM-004T integration.
- MQ-2 sensor integration.
- LD2410 presence detection.
- Sensor reading validation and failure handling.
- Local monitoring and alert rules.
- OLED display management.
- Buzzer and LED control.
- Wi-Fi communication.
- REST and MQTT telemetry publishing.
- Sensor power management.
- OTA and low-power features when scheduled.

### Deliverables

- Firmware source code.
- Hardware interface documentation.
- Verified sensor measurements.
- Wiring and GPIO records.
- Firmware build and testing evidence.

## 4. Backend and Database

### Responsibilities

- Flask backend development.
- REST API implementation.
- MQTT worker integration.
- Telemetry payload validation.
- Device authentication.
- Duplicate-message handling.
- Shared telemetry processing and storage service.
- SQLite prototype maintenance.
- PostgreSQL integration.
- Device and telemetry record management.
- Alert event storage.
- Health and status endpoints.
- Historical data APIs.
- Backend error handling and logging.

### Deliverables

- Backend source code.
- API documentation.
- Database schema.
- Database migration plan.
- API test results.
- Database persistence evidence.

## 5. Frontend and Dashboard

Frontend development is scheduled after the backend API contract is stable.

### Responsibilities

- Dashboard layout and implementation.
- Current sensor readings.
- Temperature and humidity visualization.
- Electrical power information.
- Occupancy status.
- NORMAL/WARNING/ALERT indicators.
- Historical charts.
- Alert history.
- Backend API integration.
- Responsive user interface.
- API and network error handling.

### Deliverables

- Frontend source code.
- Dashboard interface.
- API integration.
- UI testing evidence.

The frontend must consume backend APIs and must not directly access PostgreSQL or the SQLite database file.

## 6. System Integration and Testing

### Shared responsibilities

- Verify hardware and firmware integration.
- Verify ESP32-to-backend communication.
- Verify REST and MQTT telemetry consistency.
- Verify database persistence.
- Test sensor failure conditions.
- Test network and backend failures.
- Verify local alerts during network outages.
- Test alert thresholds and status transitions.
- Maintain test evidence.
- Resolve integration issues.

### Deliverables

- Integration test reports.
- Sensor test records.
- Communication logs.
- Database verification.
- Demonstration evidence.

## 7. Documentation and Project Management

### Responsibilities

- Maintain project documentation.
- Update architecture and requirements.
- Maintain the telemetry data contract.
- Maintain API and database documentation.
- Update the roadmap.
- Track implementation status.
- Maintain the test plan.
- Coordinate Git/GitHub workflow.
- Record decisions and integration changes.

### Deliverables

- Updated project documents