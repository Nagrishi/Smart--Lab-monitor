# Smart Lab Monitor

### IoT-Based Aggregate Power Monitoring and Safety Alert System for the Electronics Laboratory

## 1. Project Overview

Smart Lab Monitor is an IoT-based monitoring system designed for electronics laboratories.

It combines electrical power monitoring, environmental sensing, human presence detection, local alert processing, and remote telemetry storage.

The system aims to improve energy awareness, detect potentially unsafe environmental conditions, and provide historical monitoring data.

## 2. Key Features

### Monitoring
- Aggregate electrical voltage, current, power, and energy monitoring.
- Temperature and humidity monitoring.
- Gas/smoke-related sensor monitoring.
- Human presence detection.

### Local Processing
- ESP32-based sensor processing.
- Local monitoring and alert rules.
- OLED status display.
- Buzzer and LED indicators.
- Local alerts independent of backend availability.

### Network and Backend
- ESP32 Wi-Fi communication.
- HTTPS REST telemetry transport.
- MQTT over TLS as an additional planned transport.
- Flask backend.
- SQLite prototype storage.
- PostgreSQL planned for production/cloud storage.

### Future Capabilities
- Local web interface and mDNS.
- Historical monitoring dashboard.
- Alert history.
- Secure OTA firmware updates.
- Battery operation and low-power management.

## 3. Hardware

| Component | Purpose |
|---|---|
| ESP32 DevKit | Main controller |
| DHT11 | Current prototype temperature/humidity sensor |
| DHT22 | Planned final temperature/humidity sensor |
| PZEM-004T-100A | Electrical measurement |
| MQ-2 | Gas/smoke sensing |
| LD2410 | Human presence detection |
| SSD1306 OLED | Local display |
| Buzzer | Audible alerts |
| LEDs | Visual status indication |

Refer to `HARDWARE.md` for interface details and hardware safety requirements.

## 4. System Architecture

```text
Sensors
   |
   v
ESP32
   |
   +---- Local rules engine
   |         |
   |         +---- OLED
   |         +---- Buzzer
   |         +---- LEDs
   |
   +---- HTTPS REST
   |
   +---- MQTT over TLS
             |
             v
       Flask Backend
             |
       Telemetry Service
             |
        Database Layer
          /       \
         v         v
      SQLite   PostgreSQL
      Current   Planned
      Prototype Production
             |
             v
       Future Frontend
```

The ESP32 handles immediate local monitoring and alerting. Backend and database services support remote monitoring and historical storage.

## 5. Technology Stack

| Layer | Technology |
|---|---|
| Microcontroller | ESP32 |
| Firmware framework | Arduino framework |
| Firmware build system | PlatformIO |
| Programming languages | C/C++ |
| Network | Wi-Fi |
| REST backend | Python Flask |
| Current database | SQLite |
| Planned production database | PostgreSQL |
| Planned messaging | MQTT over TLS |
| Version control | Git and GitHub |
| Frontend | Planned |

## 6. Repository Structure

```text
Smart-Lab-Monitor/
|
|-- backend/
|   |-- app.py
|   |-- .venv/
|
|-- database/
|   |-- database.py
|   |-- sensor_data.db
|
|-- firmware/
|   |-- platformio.ini
|   |-- src/
|       |-- main.cpp
|       |-- buzzer_manager.cpp
|       |-- dht_manager.cpp
|       |-- mmwave_manager.cpp
|       |-- oled_manager.cpp
|       |-- pzem_manager.cpp
|
|-- docs/
|   |-- README.md
|   |-- PRD.md
|   |-- ARCHITECTURE.md
|   |-- HARDWARE.md
|   |-- DATA_FORMAT.md
|   |-- API.md
|   |-- DATABASE.md
|   |-- SETUP.md
|   |-- ROADMAP.md
|   |-- STATUS.md
|   |-- TEST_PLAN.md
|   |-- TEAM_RESPONSIBILITIES.md
|
```

This structure represents the documented project layout. Update it when actual project files change.

## 7. Current Prototype

The demonstrated prototype includes:

- ESP32 controller.
- DHT-based temperature and humidity sensing.
- SSD1306 OLED display.
- Wi-Fi connectivity.
- Flask backend.
- HTTP telemetry using `POST /api/sensor`.
- SQLite database persistence.

The ESP32-to-Flask communication and database persistence have been tested.

## 8. Development Roadmap

1. Documentation and architecture finalization.
2. Existing firmware and backend review.
3. Sensor integration and validation.
4. Local monitoring and alert rules.
5. REST API stabilization.
6. PostgreSQL integration.
7. MQTT over TLS integration.
8. Frontend development.
9. Reliability and failure testing.
10. OTA and low-power improvements.

## 9. Getting Started

### Firmware

Open the `firmware` directory in VS Code with PlatformIO.

Build:

```bash
pio run
```

Upload:

```bash
pio run --target upload
```

### Backend

From the project root:

```powershell
cd backend
.\.venv\Scripts\Activate.ps1
python app.py
```

The current Flask prototype is configured to run locally on port 5000.

### Database

The current SQLite database is:

```text
database/sensor_data.db
```

SQLite is file-based and does not require a separate database server.

See `SETUP.md` for the complete development instructions.

## 10. Project Documentation

| Document | Purpose |
|---|---|
| PRD.md | Product requirements |
| ARCHITECTURE.md | System architecture |
| HARDWARE.md | Hardware and interfaces |
| DATA_FORMAT.md | Telemetry JSON contract |
| API.md | Backend API contract |
| DATABASE.md | Database design |
| SETUP.md | Development setup |
| ROADMAP.md | Development phases |
| STATUS.md | Current implementation status |
| TEST_PLAN.md | Verification and testing |
| TEAM_RESPONSIBILITIES.md | Team roles |

## 11. Safety Notes

- PZEM installation may involve hazardous mains voltage. Use qualified assistance and appropriate isolation and protection.
- Verify battery protection and charger compatibility before connecting a battery subsystem.
- Do not treat uncalibrated MQ-2 ADC readings as gas concentration in ppm.
- Local alert functionality