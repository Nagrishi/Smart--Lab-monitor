
# Smart Lab Monitor — Backend API Contract

## 1. Purpose

This document defines communication between the ESP32, Flask backend and future frontend.

The system supports two telemetry transports:

1. HTTPS REST
2. MQTT over TLS

Both transports use the same telemetry structure defined in DATA_FORMAT.md.

## 2. Backend Technology

- Framework: Flask
- Database: PostgreSQL
- Initial communication: HTTPS REST
- Final remote communication: MQTT over TLS
- Data format: JSON

## 3. REST Telemetry Endpoint

### POST /api/v1/telemetry

Purpose:
Receives telemetry data from the ESP32.

Request headers:

Content-Type: application/json
X-API-Key: device-api-key

Request body:

The JSON payload must follow the schema defined in DATA_FORMAT.md.

Required top-level fields:

- schema_version
- device_id
- message_id
- timestamp
- sensors
- status
- alerts

### Successful response — new message

HTTP status: 201 Created

```json
{
  "status": "success",
  "message": "Telemetry received and saved",
  "device_id": "slm-esp32-001",
  "message_id": "unique-message-id"
}
```

### Successful response — duplicate message

HTTP status: 200 OK

```json
{
  "status": "duplicate",
  "message": "Message was already processed"
}
```

A duplicate message must not create another telemetry record.

## 4. API Error Responses

| HTTP status | Meaning |
|---|---|
| 400 | Invalid JSON |
| 401 | Missing or invalid API key |
| 404 | Endpoint not found |
| 415 | Unsupported content type |
| 422 | Invalid telemetry fields or values |
| 500 | Internal backend error |
| 503 | Database or required service unavailable |

Example:

```json
{
  "status": "error",
  "message": "Invalid telemetry data"
}
```

## 5. Health Endpoint

### GET /api/health

Purpose:
Checks whether the backend is running and whether its database connection is healthy.

Expected successful response:

HTTP 200 OK

```json
{
  "status": "healthy",
  "database": "connected"
}
```

If the database is unavailable, the response must indicate degraded or unhealthy status.

## 6. Future Read Endpoints

These endpoints are planned for the future frontend.

They must not be considered implemented until tested.

### GET /api/v1/latest

Returns the latest telemetry record.

### GET /api/v1/history

Returns historical telemetry records.

Planned query parameters:

- device_id
- start_time
- end_time
- limit

### GET /api/v1/status

Returns the latest computed status and active alerts.

## 7. MQTT Communication

MQTT messages use the same telemetry JSON schema.

Proposed topics:

- smartlab/{device_id}/telemetry
- smartlab/{device_id}/status
- smartlab/{device_id}/availability

MQTT security requirements:

- TLS encryption
- Broker authentication
- Certificate validation
- Authorized topic access

The MQTT worker must validate messages before storing them.

## 8. Backend Processing

Both REST and MQTT messages must use the same processing sequence:

1. Receive telemetry.
2. Validate the message.
3. Verify device identity.
4. Check message_id for duplicates.
5. Normalize the data.
6. Store telemetry in PostgreSQL.
7. Update device last-seen information.
8. Record relevant alert events.

## 9. Frontend Integration

Frontend development is deferred.

When implemented, the browser must retrieve information through Flask APIs.

The frontend must not connect directly to PostgreSQL or access database credentials.

## 10. Compatibility with Existing Prototype

The existing endpoint:

POST /api/sensor

will be retained temporarily for the current ESP32 firmware.

The new versioned endpoint:

POST /api/v1/telemetry

will be used for the finalized telemetry structure.

The old endpoint will only be deprecated after the new endpoint is tested successfully.

## 11. Implementation Status

Currently demonstrated:

- POST /api/sensor
- ESP32 to Flask HTTP communication
- Prototype sensor data persistence in SQLite

Planned:

- POST /api/v1/telemetry
- PostgreSQL persistence
- MQTT over TLS
- Future read APIs
- Device authentication

Do not mark planned endpoints as implemented until integration tests pass.
