
#include <Arduino.h>

#include "config.h"
#include "sensor_data.h"

#include "cloud_manager.h"
#include "wifi_manager.h"
#include "dht_manager.h"
#include "mmwave_manager.h"
#include "oled_manager.h"
#include "pzem_manager.h"
#include "buzzer_manager.h"

// Shared sensor data
SensorData sensors;

// Update intervals
const unsigned long DHT_INTERVAL = 2000;
const unsigned long OLED_INTERVAL = 500;
const unsigned long SERIAL_INTERVAL = 2000;

// Timers
unsigned long lastDHT = 0;
unsigned long lastOLED = 0;
unsigned long lastSerial = 0;

void setup() {

    Serial.begin(SERIAL_BAUD);

    delay(1000);

    Serial.println();
    Serial.println("==============================");
    Serial.println("     SMART LAB MONITOR");
    Serial.println("==============================");

    // Initialize DHT11
    dhtManagerInit();

    // Initialize mmWave
    mmwaveManagerInit();

    // Initialize OLED
    oledManagerInit();

    // Initialize PZEM
    pzemManagerInit(sensors);
    
    // Initialize buzzer
    buzzerManagerInit();
    Serial.println("System initialization complete");

    // Initialize temporary Wi-Fi provisioning
    wifiManagerInit();
    // for cloud
    cloudManagerInit();
}

void loop() {

    unsigned long now = millis();

    // DHT11 update every 2 seconds
    if (now - lastDHT >= DHT_INTERVAL) {

        lastDHT = now;

        dhtManagerUpdate(sensors);
    }

    // mmWave continuous monitoring
    mmwaveManagerUpdate(sensors);

    // Update buzzer alarm
    buzzerManagerUpdate(sensors);

    // Maintain Wi-Fi provisioning and connection state
    wifiManagerUpdate();

    // Upload sensor data to Flask backend
    cloudManagerUpdate(sensors);

    // OLED update every 500 ms
    if (now - lastOLED >= OLED_INTERVAL) {

        lastOLED = now;

        oledManagerUpdate(sensors);
    }

    // Serial output every 2 seconds
    if (now - lastSerial >= SERIAL_INTERVAL) {

        lastSerial = now;

        Serial.println("\n===== SYSTEM STATUS =====");

        if (sensors.dhtValid) {

            Serial.printf(
                "Temperature: %.1f C\n",
                sensors.temperature
            );

            Serial.printf(
                "Humidity: %.1f %%\n",
                sensors.humidity
            );

        } else {

            Serial.println("DHT11: ERROR");
        }

        Serial.print("Presence: ");

        Serial.println(
            sensors.presence ? "DETECTED" : "NOT DETECTED"
        );

        if (sensors.pzemValid) {

            Serial.printf("Voltage: %.2f V\n", sensors.voltage);
            Serial.printf("Current: %.3f A\n", sensors.current);
            Serial.printf("Power: %.2f W\n", sensors.power);
            Serial.printf("Energy: %.4f kWh\n", sensors.energy);

        } else {

            Serial.println("PZEM: WAITING FOR DATA");
        }

        Serial.println("=========================");
    }
}

