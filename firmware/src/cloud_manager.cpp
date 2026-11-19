
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>

#include "cloud_manager.h"
#include "wifi_manager.h"
#include "buzzer_manager.h"

// ==========================================
// FLASK BACKEND CONFIGURATION
// ==========================================

// LOCAL DEVELOPMENT:
// Keep this URL while testing on the same Wi-Fi.
//
// PRODUCTION:
// Replace it with your deployed Render HTTPS URL.

const char* SERVER_URL =
    "http://10.231.23.184:5000/api/sensor";

// Upload every 5 seconds
const unsigned long UPLOAD_INTERVAL = 5000;

// HTTP timeout
const unsigned long HTTP_TIMEOUT = 5000;

unsigned long lastUploadTime = 0;


// ==========================================
// CLOUD MANAGER INITIALIZATION
// ==========================================

void cloudManagerInit() {

    Serial.println();
    Serial.println("Initializing Cloud Manager...");

    Serial.print("Backend URL: ");
    Serial.println(SERVER_URL);

    Serial.println("Upload interval: 5 seconds");
}


// ==========================================
// UPLOAD SENSOR DATA
// ==========================================

void cloudManagerUpdate(const SensorData &data) {

    unsigned long now = millis();

    // Check upload interval
    if (now - lastUploadTime < UPLOAD_INTERVAL) {
        return;
    }

    // Check Wi-Fi connection
    if (!wifiManagerIsConnected()) {
        return;
    }

    // Backend requires valid temperature and humidity
    if (!data.dhtValid) {

        Serial.println(
            "Cloud upload skipped: Invalid DHT11 data"
        );

        return;
    }

    lastUploadTime = now;

    // ======================================
    // HTTP INITIALIZATION
    // ======================================

    WiFiClient client;
    HTTPClient http;

    http.setTimeout(HTTP_TIMEOUT);

    if (!http.begin(client, SERVER_URL)) {

        Serial.println(
            "Cloud: HTTP initialization failed"
        );

        return;
    }

    http.addHeader(
        "Content-Type",
        "application/json"
    );

    http.addHeader(
        "Accept",
        "application/json"
    );

    // ======================================
    // BUILD JSON PAYLOAD
    // ======================================

    String payload;

    payload.reserve(350);

    payload = "{";

    // DHT11
    payload += "\"temperature\":";
    payload += String(data.temperature, 2);

    payload += ",\"humidity\":";
    payload += String(data.humidity, 2);

    // PZEM-004T
    if (data.pzemValid) {

        payload += ",\"voltage\":";
        payload += String(data.voltage, 2);

        payload += ",\"current\":";
        payload += String(data.current, 3);

        payload += ",\"power\":";
        payload += String(data.power, 2);

        payload += ",\"energy\":";
        payload += String(data.energy, 4);

        payload += ",\"frequency\":";
        payload += String(data.frequency, 2);

        payload += ",\"power_factor\":";
        payload += String(data.powerFactor, 2);
    }

    // MMWAVE
    if (data.presenceValid) {

        payload += ",\"mmwave_person_detected\":";

        payload += data.presence
            ? "true"
            : "false";
    }

    // Temperature warning
    bool temperatureWarning =
        buzzerManagerTemperatureWarning();

    payload += ",\"temperature_warning\":";

    payload += temperatureWarning
        ? "true"
        : "false";

    // Buzzer
    bool buzzerOn =
        buzzerManagerIsActive();

    payload += ",\"buzzer_on\":";

    payload += buzzerOn
        ? "true"
        : "false";

    payload += "}";

    // ======================================
    // SEND HTTP POST
    // ======================================

    Serial.println();
    Serial.println("===== CLOUD UPLOAD =====");

    Serial.println(payload);

    Serial.println("========================");

    int httpCode = http.POST(payload);

    // ======================================
    // HANDLE SERVER RESPONSE
    // ======================================

    if (httpCode >= 200 && httpCode < 300) {

        Serial.printf(
            "Cloud upload successful! HTTP: %d\n",
            httpCode
        );

        String response = http.getString();

        Serial.print("Server response: ");
        Serial.println(response);
    }

    else if (httpCode > 0) {

        Serial.printf(
            "Cloud upload failed. HTTP: %d\n",
            httpCode
        );

        Serial.println(
            http.getString()
        );
    }

    else {

        Serial.printf(
            "Cloud connection error: %s\n",
            http.errorToString(httpCode).c_str()
        );
    }

    // Release resources
    http.end();
}
