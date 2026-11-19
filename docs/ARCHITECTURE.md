
# Smart Lab Monitor — System Architecture

## 1. Project Overview

The Smart Lab Monitor is an IoT-based aggregate power monitoring and safety alert system for an electronics laboratory.

It monitors electrical parameters, environmental conditions and laboratory occupancy. It provides local safety alerts and remote monitoring with historical data storage.

## 2. Final System Architecture

```text
                  ELECTRONICS LAB
                         |
        +----------------+----------------+
        |                |                |
      PZEM             DHT22            MQ-2
        |                |                |
        +----------------+----------------+
                         |
                       LD2410
                         |
                         v
                  ESP32 CONTROLLER
                         |
              Sensor Validation
                         |
                  Rules Engine
                         |
          +--------------+--------------+
          |              |              |
         OLED          Buzzer          LEDs
          |
          |
     Local Web Server
          |
      Local JSON API
                         |
             +-----------+-----------+
             |                       |
         HTTPS REST               MQTT over TLS
             |                       |
             +-----------+-----------+
                         |
                  BACKEND SERVICES
                         |
                 Flask REST API
                         |
                  MQTT Worker
                         |
                Telemetry Service
                         |
                         v
                  PostgreSQL
                         |
                  Future Frontend
```

## 3. Hardware Layer

| Component | Function |
|---|---|
| ESP32 | Main controller |
| PZEM-004T | Voltage, current, power and energy |
| DHT22 | Temperature and humidity |
| MQ-2 | Gas/smoke sensing |
| LD2410 | Human presence detection |
| SSD1306 OLED | Local status display |
| Buzzer | Audible safety alert |
| LEDs | Visual status indication |

Note: The current prototype and final hardware configuration must be documented separately. Do not mark a sensor integrated until it has been tested.

## 4. ESP32 Firmware Layer

The ESP32 performs:

- Sensor data acquisition.
- Sensor reading validation.
- Local safety and energy-wastage rules.
- NORMAL, WARNING and ALERT status calculation.
- OLED, buzzer and LED control.
- Local web server and JSON endpoint.
- HTTPS REST communication.
- MQTT over TLS communication.

Local safety logic must continue working even when Wi-Fi, the backend or MQTT broker is unavailable.

## 5. Communication Layer

### HTTPS REST

Initial integration path:

ESP32 → Flask API → PostgreSQL

Purpose:
- Initial cloud integration.
- Telemetry submission.
- API validation and error handling.

### MQTT over TLS

Final remote telemetry path:

ESP32 → MQTT Broker → MQTT Worker → Telemetry Service → PostgreSQL

Proposed topics:

- smartlab/{device_id}/telemetry
- smartlab/{device_id}/status
- smartlab/{device_id}/availability

Use authenticated connections and TLS certificate validation.

Both communication methods must use the same telemetry data contract.

## 6. Backend Layer

The backend uses:

- Flask for REST APIs.
- MQTT worker for MQTT message consumption.
- Shared telemetry service for validation and storage.
- PostgreSQL for cloud data persistence.

The REST API and MQTT worker must use the same validation and database-storage logic.

## 7. Database Layer

PostgreSQL is the selected cloud database.

Initial tables:

- devices
- telemetry
- alerts

The existing SQLite database will be retained during development and migration.

## 8. Frontend Layer

Frontend development is deferred.

The future dashboard will retrieve data through backend APIs.

It will display:

- Current sensor readings.
- Electrical measurements.
- Occupancy.
- System status.
- Historical measurements.
- Safety alerts.

The frontend must never connect directly to PostgreSQL.

## 9. Reliability and Safety

- Local alerts must not depend on internet connectivity.
- Invalid sensor readings must be identified.
- MQTT reconnect handling must be implemented.
- REST failures must not stop sensor acquisition.
- Duplicate MQTT messages must not create duplicate telemetry records.
- Safety thresholds must be configurable and documented.

## 10. Implementation Strategy

1. Finalize architecture and telemetry contract.
2. Configure PostgreSQL.
3. Implement backend database services.
4. Stabilize HTTPS REST.
5. Implement MQTT over TLS.
6. Integrate ESP32 communication modules.
7. Perform reliability and integration tests.
8. Develop frontend later.
9. Implement OTA and low-power features.
