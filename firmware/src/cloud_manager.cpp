
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

#include "cloud_manager.h"
#include "wifi_manager.h"
#include "buzzer_manager.h"

// ==========================================
// RENDER CLOUD BACKEND CONFIGURATION
// ==========================================

const char* SERVER_URL =
    "https://smart-lab-monitor-api.onrender.com/api/sensor";

// Upload interval: 5 seconds
const unsigned long UPLOAD_INTERVAL = 5000;

// HTTPS timeout: 15 seconds
const unsigned long HTTP_TIMEOUT = 15000;

unsigned long lastUploadTime = 0;


// ==========================================
// CLOUD MANAGER INITIALIZATION
// ==========================================

void cloudManagerInit() {

    Serial.println();
    Serial.println("================================");
    Serial.println("Initializing Cloud Manager...");
    Serial.println("================================");

    Serial.print("Backend URL: ");
    Serial.println(SERVER_URL);

    Serial.println("Protocol: HTTPS");
    Serial.println("Upload interval: 5 seconds");

    Serial.println("Cloud Manager initialized.");
}


// ==========================================
// UPLOAD SENSOR DATA
// ==========================================

void cloudManagerUpdate(const SensorData &data) {

    unsigned long now = millis();

    // --------------------------------------
    // CHECK UPLOAD INTERVAL
    // --------------------------------------

    if (now - lastUploadTime < UPLOAD_INTERVAL) {
        return;
    }

    // --------------------------------------
    // CHECK WIFI CONNECTION
    // --------------------------------------

    if (!wifiManagerIsConnected()) {
        Serial.println("Cloud upload skipped: WiFi disconnected");
        return;
    }

    // --------------------------------------
    // CHECK DHT11 DATA
    // --------------------------------------

    if (!data.dhtValid) {

        Serial.println(
            "Cloud upload skipped: Invalid DHT11 data"
        );

        return;
    }

    // Update upload timer
    lastUploadTime = now;


    // ======================================
    // HTTPS CLIENT INITIALIZATION
    // ======================================

    WiFiClientSecure client;

    // TEMPORARY TESTING ONLY:
    // Disables server certificate verification.
    // Replace with proper certificate validation
    // before production deployment.

    client.setInsecure();

    HTTPClient http;

    http.setTimeout(HTTP_TIMEOUT);

    if (!http.begin(client, SERVER_URL)) {

        Serial.println(
            "Cloud: HTTPS initialization failed"
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


    // --------------------------------------
    // DHT11 SENSOR DATA
    // --------------------------------------

    payload += "\"temperature\":";
    payload += String(data.temperature, 2);

    payload += ",\"humidity\":";
    payload += String(data.humidity, 2);


    // --------------------------------------
    // PZEM-004T ELECTRICAL DATA
    // --------------------------------------

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


    // --------------------------------------
    // MMWAVE PRESENCE DETECTION
    // --------------------------------------

    if (data.presenceValid) {

        payload += ",\"mmwave_person_detected\":";

        payload += data.presence
            ? "true"
            : "false";
    }


    // --------------------------------------
    // TEMPERATURE WARNING
    // --------------------------------------

    bool temperatureWarning =
        buzzerManagerTemperatureWarning();

    payload += ",\"temperature_warning\":";

    payload += temperatureWarning
        ? "true"
        : "false";


    // --------------------------------------
    // BUZZER STATUS
    // --------------------------------------

    bool buzzerOn =
        buzzerManagerIsActive();

    payload += ",\"buzzer_on\":";

    payload += buzzerOn
        ? "true"
        : "false";


    // Finish JSON
    payload += "}";


    // ======================================
    // SEND HTTPS POST REQUEST
    // ======================================

    Serial.println();
    Serial.println("========== CLOUD UPLOAD ==========");

    Serial.println("Uploading sensor data...");

    Serial.print("Temperature: ");
    Serial.println(data.temperature);

    Serial.print("Humidity: ");
    Serial.println(data.humidity);

    Serial.println("Sending HTTPS POST request...");


    int httpCode = http.POST(payload);


    // ======================================
    // HANDLE SERVER RESPONSE
    // ======================================

    if (httpCode >= 200 && httpCode < 300) {

        Serial.println();
        Serial.println("[SUCCESS] Cloud upload completed!");

        Serial.printf(
            "HTTP Status: %d\n",
            httpCode
        );

        String response = http.getString();

        Serial.print("Server response: ");
        Serial.println(response);

        Serial.println("Database record submitted successfully.");
    }

    else if (httpCode > 0) {

        Serial.println();
        Serial.println("[ERROR] Server rejected the request.");

        Serial.printf(
            "HTTP Status: %d\n",
            httpCode
        );

        Serial.print("Server response: ");
        Serial.println(http.getString());
    }

    else {

        Serial.println();
        Serial.println("[ERROR] HTTPS connection failed.");

        Serial.print("Error: ");

        Serial.println(
            http.errorToString(httpCode).c_str()
        );
    }


    // ======================================
    // RELEASE HTTP RESOURCES
    // ======================================

    http.end();

    Serial.println("==================================");
}
