
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

const char* SERVER_URL =
    "http://10.231.23.184:5000/api/sensor";

// Upload interval: 5 seconds
const unsigned long UPLOAD_INTERVAL = 5000;

// HTTP timeout: 2 seconds
const unsigned long HTTP_TIMEOUT = 2000;

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
        return;
    }

    // --------------------------------------
    // CHECK DHT11 VALIDITY
    // Backend requires temperature and humidity
    // --------------------------------------

    if (!data.dhtValid) {

        Serial.println(
            "Cloud upload skipped: DHT11 data invalid"
        );

        return;
    }

    // Update upload timer
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


    // ======================================
    // BUILD JSON PAYLOAD
    // ======================================

    String payload;

    payload.reserve(350);

    payload = "{";


    // --------------------------------------
    // DHT11 DATA
    // --------------------------------------

    payload += "\"temperature\":";
    payload += String(data.temperature, 2);

    payload += ",\"humidity\":";
    payload += String(data.humidity, 2);


    // --------------------------------------
    // PZEM-004T DATA
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
    // MMWAVE PRESENCE DATA
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
    // SEND HTTP POST REQUEST
    // ======================================

    Serial.println();
    Serial.println("===== CLOUD JSON PAYLOAD =====");

    Serial.println(payload);

    Serial.println("==============================");

    Serial.println("Uploading sensor data...");

    int httpCode = http.POST(payload);


    // ======================================
    // HANDLE SERVER RESPONSE
    // ======================================

    if (httpCode == HTTP_CODE_CREATED) {

        Serial.println(
            "Cloud upload successful!"
        );

        Serial.println(
            "Database record created."
        );

    }

    else if (httpCode > 0) {

        Serial.printf(
            "Cloud upload failed. HTTP code: %d\n",
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


    // Release HTTP resources
    http.end();
}
