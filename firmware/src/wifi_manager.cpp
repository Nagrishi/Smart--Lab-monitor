
#include <Arduino.h>
#include <WiFi.h>

#include "config_secrets.h"
#include "wifi_manager.h"

// ========================================
// WI-FI CONFIGURATION
// ========================================

// Reconnection interval
const unsigned long WIFI_RECONNECT_INTERVAL = 10000;

// Timers
unsigned long lastReconnectAttempt = 0;

// Track connection state for Serial Monitor
bool previousConnectionState = false;

// ========================================
// INITIALIZE WI-FI
// ========================================

void wifiManagerInit() {

    Serial.println();
    Serial.println("Initializing Wi-Fi...");

    // ESP32 works as a Wi-Fi client
    WiFi.mode(WIFI_STA);

    // Do not save credentials permanently in flash
    WiFi.persistent(false);

    // Enable automatic reconnection
    WiFi.setAutoReconnect(true);

    // Disable Wi-Fi power saving
    WiFi.setSleep(false);

    // Do not print the actual SSID or password
    Serial.println("Connecting to configured hotspot...");

    // Start connection without blocking sensor initialization
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    lastReconnectAttempt = millis();
}

// ========================================
// MAINTAIN WI-FI CONNECTION
// ========================================

void wifiManagerUpdate() {

    unsigned long now = millis();

    bool connected = (WiFi.status() == WL_CONNECTED);

    // Detect successful connection
    if (connected && !previousConnectionState) {

        Serial.println();
        Serial.println("Wi-Fi connected!");

        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        Serial.print("Signal strength: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");

        previousConnectionState = true;
    }

    // Detect disconnection
    if (!connected && previousConnectionState) {

        Serial.println();
        Serial.println("Wi-Fi disconnected!");
        Serial.println("Local monitoring remains active.");

        previousConnectionState = false;
    }

    // Retry connection periodically
    if (!connected &&
        now - lastReconnectAttempt >= WIFI_RECONNECT_INTERVAL) {

        lastReconnectAttempt = now;

        Serial.println("Attempting Wi-Fi reconnection...");

        WiFi.reconnect();
    }
}

// ========================================
// CHECK CONNECTION
// ========================================

bool wifiManagerIsConnected() {

    return WiFi.status() == WL_CONNECTED;
}
