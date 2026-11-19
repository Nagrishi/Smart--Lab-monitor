# Development Setup — Smart Lab Monitor

## 1. Purpose

This document explains how to configure and run the current Smart Lab Monitor prototype.

The current development environment uses:

- Windows 11
- Visual Studio Code
- PlatformIO
- ESP32 DevKit
- Arduino framework
- Python
- Flask
- SQLite

PostgreSQL, MQTT over TLS, and the frontend are planned for later development.

## 2. Prerequisites

Install the following software:

| Software | Purpose |
|---|---|
| Visual Studio Code | Development environment |
| PlatformIO IDE extension | ESP32 firmware development |
| Python 3 | Backend development |
| Git | Version control |
| USB driver for ESP32 board | Serial communication |

Verify the installations using PowerShell:

```powershell
python --version
git --version
pio --version
```

If `pio` is not recognized, open the PlatformIO terminal in VS Code or verify that PlatformIO Core is installed.

## 3. Open the Project

1. Open Visual Studio Code.
2. Select **File → Open Folder**.
3. Select the `Smart-Lab-Monitor` project directory.
4. Open the PlatformIO project from the `firmware` directory when working on ESP32 firmware.

Keep the project root as the working directory when following the backend and database instructions.

## 4. Firmware Setup

### Open PlatformIO

Open:

```text
Smart-Lab-Monitor/firmware
```

Confirm that `platformio.ini` is present.

### Build firmware

From the firmware directory:

```powershell
pio run
```

### Upload firmware

Connect the ESP32 through USB and select the correct serial port.

```powershell
pio run --target upload
```

### Open serial monitor

```powershell
pio device monitor -b 115200
```

The monitor baud rate must match the baud rate configured in the firmware.

## 5. Backend Setup

Open a VS Code terminal from the project root.

### Navigate to backend

```powershell
cd backend
```

### Activate the existing virtual environment

```powershell
.\.venv\Scripts\Activate.ps1
```

If PowerShell blocks activation, use the following command in the current terminal session:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
```

Then activate the environment again.

### Install dependencies

Use the project's `requirements.txt` if one is available.

If the dependency file has not yet been created, first inspect the existing environment and backend imports before generating one. Do not blindly replace the existing virtual environment.

### Start Flask

```powershell
python app.py
```

The current prototype is configured to listen on port `5000`.

The server should be reachable from the development computer at:

```text
http://127.0.0.1:5000
```

The exact available routes depend on the current Flask implementation.

## 6. SQLite Database

The current prototype uses SQLite.

Database file:

```text
database/sensor_data.db
```

SQLite is file-based. It does not need a separate database server process.

The database initialization script is:

```text
database/database.py
```

If initialization is required, run the script from the project root:

```powershell
python database\database.py
```

Do not delete or overwrite the existing database during development. It may contain previously collected sensor records.

## 7. Recommended Development Terminals

### Terminal 1 — Backend

From the project root:

```powershell
cd backend
.\.venv\Scripts\Activate.ps1
python app.py
```

### Terminal 2 — Firmware

From the project root:

```powershell
cd firmware
pio device monitor -b 115200
```

### Terminal 3 — Firmware build

From the project root:

```powershell
cd firmware
pio run
```

## 8. ESP32 and Backend Network

For the ESP32 to send data to a Flask server running on the development computer:

1. Connect the ESP32 and computer to the same local network.
2. Find the computer's local IPv4 address using:

```powershell
ipconfig
```

3. Configure the ESP32 firmware to use the computer's local IPv4 address instead of `127.0.0.1`.
4. Ensure Flask listens on the appropriate network interface.
5. Allow the required port through the Windows firewall only on a trusted private network.

`127.0.0.1` refers to the device itself. It cannot be used by the ESP32 to reach Flask running on another computer.

The current prototype may use HTTP for local development. Production or remote communication must use the planned authenticated HTTPS and MQTT-over-TLS configuration.

## 9. Prototype Verification

A successful prototype test should demonstrate:

- [ ] ESP32 boots correctly.
- [ ] Wi-Fi connection succeeds.
- [ ] DHT sensor returns valid readings.
- [ ] OLED displays the readings.
- [ ] ESP32 sends an HTTP request.
- [ ] Flask receives the request.
- [ ] Backend returns the expected HTTP response.
- [ ] SQLite receives a new record.
- [ ] Sensor values in the database match the transmitted values.

Record test results in `TEST_PLAN.md`.

## 10. Troubleshooting

### ESP32 is not detected

- Check the USB cable.
- Verify the USB-to-serial driver.
- Check the selected COM port.
- Reconnect the board.

### Firmware upload fails

- Verify the selected PlatformIO board.
- Check the serial port.
- Close applications using the same port.
- Check the USB connection.

### Flask does not start

- Confirm the virtual environment is activated.
- Check Python imports and dependencies.
- Read the full terminal error.
- Verify that port 5000 is not already occupied.

### ESP32 cannot reach Flask

- Confirm both devices are on the same network.
- Verify the computer's IPv4 address.
- Check the Flask host binding.
- Review Windows firewall settings.
- Confirm the endpoint path and port.

### SQLite database is not updated

- Verify the database path used by the application.
- Check for database errors in the Flask terminal.
- Confirm that the telemetry request was accepted.
- Verify that the backend commits the transaction.


## 11. Security Rules

- Never commit Wi-Fi passwords, API keys, database credentials, or private tokens.
- Keep secrets in environment variables or ignored configuration files.
- Do not expose Flask's debug server to the public internet.
- Use authenticated HTTPS for production REST communication.
- Use MQTT over TLS with certificate validation when MQTT is introduced.
- Protect PostgreSQL credentials when cloud storage is introduced.
- Authenticate ESP32 devices before accepting telemetry.
- Validate incoming sensor data before database storage.
- Never expose private backend credentials in the React frontend.


## 12. Future Setup Requirements

The following setup instructions will be added after implementation and testing:

- PostgreSQL installation and configuration.
- MQTT broker configuration.
- TLS certificate and device authentication setup.
- Backend production deployment.
- Frontend development and deployment.
- OTA update configuration.

Do not treat planned services as operational until they have been implemented and verified.