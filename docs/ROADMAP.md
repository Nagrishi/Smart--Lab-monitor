# Development Roadmap — Smart Lab Monitor

## 1. Project Goal

Develop a reliable IoT-based laboratory monitoring system using ESP32, multiple sensors, local alert processing, secure network communication, backend services, and historical data storage.

Development will proceed incrementally. Existing working functionality must remain operational while new features are introduced.

## 2. Phase 1 — Requirements and Documentation

**Objective:** Establish a consistent technical specification.

Tasks:
- [x] Define product requirements.
- [x] Document system architecture.
- [x] Define telemetry JSON format.
- [x] Define backend API contract.
- [x] Document database design.
- [x] Document hardware interfaces.
- [ ] Review all remaining project documentation.
- [ ] Review documentation against actual implementation.

**Completion criteria:** Documentation is consistent and accurately distinguishes implemented features from planned features.

## 3. Phase 2 — Existing Prototype Stabilization

**Objective:** Preserve and verify the current working prototype.

Tasks:
- [x] ESP32 PlatformIO project.
- [x] DHT-based temperature/humidity prototype.
- [x] SSD1306 OLED operation.
- [x] Wi-Fi connectivity.
- [x] ESP32-to-Flask HTTP communication.
- [x] SQLite persistence.
- [ ] Review existing firmware source files.
- [ ] Review Flask backend source.
- [ ] Review SQLite schema and database operations.
- [ ] Verify request validation and error handling.
- [ ] Record repeatable prototype test results.

**Completion criteria:** Existing sensor readings reach Flask and are stored reliably without breaking the current prototype.

## 4. Phase 3 — Sensor Integration

**Objective:** Integrate and validate each planned sensor individually.

Tasks:
- [ ] Validate DHT22 temperature and humidity readings.
- [ ] Integrate PZEM-004T voltage, current, power, and energy measurements.
- [ ] Validate MQ-2 analog readings.
- [ ] Establish MQ-2 baseline and calibration approach.
- [ ] Integrate LD2410 presence detection.
- [ ] Validate sensor failure handling.
- [ ] Document actual wiring and GPIO assignments.

**Completion criteria:** Each sensor produces validated measurements with documented units and failure behavior.

## 5. Phase 4 — Local Monitoring and Alert Logic

**Objective:** Make the ESP32 responsible for immediate monitoring and alerts.

Tasks:
- [ ] Implement periodic sensor polling.
- [ ] Validate measurement ranges and sensor availability.
- [ ] Implement configurable thresholds.
- [ ] Implement NORMAL/WARNING/ALERT status logic.
- [ ] Implement occupancy and power correlation.
- [ ] Implement OLED status updates.
- [ ] Implement buzzer alert patterns.
- [ ] Implement LED status indications.
- [ ] Test local alert behavior during network failure.

**Completion criteria:** Local monitoring and alerts continue operating independently of backend or cloud availability.

## 6. Phase 5 — Network Communication

**Objective:** Establish reliable communication between ESP32 and backend services.

### REST first

- [ ] Implement the versioned telemetry endpoint.
- [ ] Use the shared JSON contract.
- [ ] Validate HTTP request and response handling.
- [ ] Handle network disconnection and retry behavior.
- [ ] Verify duplicate-message handling.

### MQTT over TLS

- [ ] Select and configure an MQTT broker.
- [ ] Configure TLS certificate validation.
- [ ] Configure device authentication.
- [ ] Implement telemetry topic publishing.
- [ ] Implement device status and availability topics.
- [ ] Test broker disconnection and reconnection.

**Completion criteria:** REST and MQTT use the same telemetry schema and backend processing rules.

## 7. Phase 6 — Backend and Database

**Objective:** Build a reliable backend and persistent data layer.

Tasks:
- [ ] Review the current Flask application.
- [ ] Separate telemetry validation from storage logic.
- [ ] Preserve the existing SQLite prototype.
- [ ] Implement PostgreSQL schema.
- [ ] Configure PostgreSQL connection securely.
- [ ] Implement telemetry storage.
- [ ] Implement duplicate-message protection.
- [ ] Implement device last-seen tracking.
- [ ] Implement alert event storage.
- [ ] Add health and database connectivity checks.
- [ ] Add current and historical data APIs.

**Completion criteria:** Valid telemetry from REST and MQTT is processed through shared services and stored reliably in PostgreSQL.

## 8. Phase 7 — Frontend Dashboard

**Objective:** Provide a user interface for laboratory monitoring.

This phase is intentionally deferred until the backend APIs and data contract are stable.

Tasks:
- [ ] Create dashboard layout.
- [ ] Display current sensor readings.
- [ ] Display aggregate electrical measurements.
- [ ] Display occupancy and system status.
- [ ] Display NORMAL/WARNING/ALERT indicators.
- [ ] Display historical charts.
- [ ] Display alert history.
- [ ] Handle API and connectivity failures.
- [ ] Implement responsive layout.

**Completion criteria:** The frontend displays backend data without directly accessing the database.

## 9. Phase 8 — Reliability and Security

**Objective:** Verify safe behavior under failure conditions.

Tasks:
- [ ] Test sensor disconnection.
- [ ] Test invalid sensor measurements.
- [ ] Test Wi-Fi disconnection.
- [ ] Test Flask backend failure.
- [ ] Test MQTT broker failure.
- [ ] Test database unavailability.
- [ ] Verify local alerts continue during network failure.
- [ ] Verify authentication and access control.
- [ ] Verify TLS certificate validation.
- [ ] Review secret and credential handling.

**Completion criteria:** The system handles common failures safely and does not depend on remote services for immediate local alerts.

## 10. Phase 9 — OTA and Firmware Maintenance

**Objective:** Support controlled firmware updates.

Tasks:
- [ ] Define firmware versioning.
- [ ] Define release process.
- [ ] Configure GitHub Actions for build verification.
- [ ] Implement secure OTA update mechanism.
- [ ] Verify update success.
- [ ] Test update failure and recovery.

**Completion criteria:** Firmware updates are versioned, tested, and recoverable.

## 11. Phase 10 — Power Optimization

**Objective:** Prepare the system for unattended operation.

Tasks:
- [ ] Validate sensor power requirements.
- [ ] Test sensor-rail switching.
- [ ] Validate MOSFET operation and thermal performance.
- [ ] Verify battery protection and charging design.
- [ ] Measure system current consumption.
- [ ] Implement deep-sleep/wake operation if suitable.
- [ ] Verify sensor readings after wake-up.

**Completion criteria:** The power subsystem is electrically validated and the system operates reliably under the intended power conditions.

## 12. Development Rules

1. Work on one subsystem at a time.
2. Preserve the working prototype.
3. Test each hardware component independently.
4. Use the same telemetry contract across REST and MQTT.
5. Keep local alert processing independent of cloud connectivity.
6. Do not expose credentials in source control.
7. Record test evidence before marking features complete.
8. Update `STATUS.md` whenever implementation status changes.

## 13. Current Milestone

**Current focus:** Documentation finalization and existing prototype review.

The demonstrated baseline is:

```text
ESP32
  |
  v
Wi-Fi
  |
  v
Flask Backend
  |
  v
SQLite Database
```

The next engineering activity is to review the existing backend and firmware before modifying implementation.

Do not begin PostgreSQL migration, MQTT integration, or frontend development until the current prototype has been reviewed and stabilized.