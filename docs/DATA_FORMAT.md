
# Smart Lab Monitor — Telemetry Data Format

## 1. Purpose

This document defines the standard telemetry data structure used by the ESP32, Flask REST API, MQTT communication and PostgreSQL database.

Both HTTPS REST and MQTT must use the same JSON data structure.

## 2. Schema Version

Current schema version: 1

Every telemetry message must contain:

- schema_version
- device_id
- message_id
- timestamp
- sensors
- status
- alerts

## 3. Standard JSON Payload

```json
{
  "schema_version": 1,
  "device_id": "slm-esp32-001",
  "message_id": "unique-message-id",
  "timestamp": "2026-11-18T10:30:00Z",
  "sensors": {
    "dht": {
      "temperature_c": 31.3,
      "humidity_rh": 75.0,
      "valid": true
    },
    "pzem": {
      "voltage_v": 230.1,
      "current_a": 1.25,
      "power_w": 287.6,
      "energy_wh": 1250.5,
      "valid": true
    },
    "mq2": {
      "raw_adc": 1250,
      "voltage_v": 1.01,
      "valid": true
    },
    "ld2410": {
      "presence": true,
      "valid": true
    }
  },
  "status": "NORMAL",
  "alerts": {
    "high_temperature": false,
    "high_gas": false,
    "energy_wastage": false
  }
}
```

This is the target payload. Values are illustrative, not actual sensor measurements.

## 4. Field Definitions

### 4.1 Device Information

| Field | Type | Description |
|---|---|---|
| schema_version | Integer | Telemetry schema version |
| device_id | String | Unique ESP32 device identifier |
| message_id | String | Unique telemetry message identifier |
| timestamp | String | UTC timestamp in ISO 8601 format |

### 4.2 DHT Sensor

| Field | Type | Unit |
|---|---|---|
| temperature_c | Number or null | °C |
| humidity_rh | Number or null | %RH |
| valid | Boolean | Sensor validity |

### 4.3 PZEM-004T

| Field | Type | Unit |
|---|---|---|
| voltage_v | Number or null | V |
| current_a | Number or null | A |
| power_w | Number or null | W |
| energy_wh | Number or null | Wh |
| valid | Boolean | Sensor validity |

### 4.4 MQ-2

| Field | Type | Unit |
|---|---|---|
| raw_adc | Integer or null | ADC counts |
| voltage_v | Number or null | V |
| valid | Boolean | Sensor validity |

Important: Raw ADC values must not be treated as gas concentration in ppm. PPM requires a documented calibration process.

### 4.5 LD2410

| Field | Type | Description |
|---|---|---|
| presence | Boolean or null | Human presence detected |
| valid | Boolean | Sensor validity |

## 5. System Status

Allowed values:

| Status | Meaning |
|---|---|
| NORMAL | No active warning or safety alert |
| WARNING | Energy wastage or configured warning condition |
| ALERT | Safety threshold exceeded |

The ESP32 rules engine calculates the status locally.

## 6. Alert Fields

| Field | Type | Meaning |
|---|---|---|
| high_temperature | Boolean | Temperature threshold exceeded |
| high_gas | Boolean | Validated gas/smoke threshold exceeded |
| energy_wastage | Boolean | Unoccupied and power above idle baseline for the configured duration |

Absence alone must not trigger an energy-wastage alert.

Temperature and gas/smoke safety alerts are evaluated independently of occupancy.

## 7. Data Validation Rules

1. Missing or failed sensor readings must not be represented as genuine zero values.
2. An unavailable measurement must be null.
3. A failed sensor must have valid set to false.
4. A valid Boolean sensor reading must be true or false.
5. All measurement units must remain consistent.
6. Timestamps must use UTC.
7. Every device must have a unique device_id.
8. Every telemetry message must have a unique message_id.
9. Unknown schema versions must be rejected or handled explicitly.
10. Invalid messages must not be stored as valid telemetry.

## 8. Communication

### HTTPS REST

Endpoint:

POST /api/v1/telemetry

Content-Type:

application/json

### MQTT over TLS

Proposed topics:

- smartlab/{device_id}/telemetry
- smartlab/{device_id}/status
- smartlab/{device_id}/availability

Telemetry messages must use the same JSON payload defined in this document.

## 9. Data Flow

Sensors
  ↓
ESP32
  ↓
Sensor validation
  ↓
Rules engine
  ↓
Telemetry JSON
  ↓
HTTPS REST or MQTT over TLS
  ↓
Backend validation
  ↓
PostgreSQL

## 10. Implementation Status

This document defines the target data contract.

The final payload must be implemented and tested before marking the data-format integration complete.
