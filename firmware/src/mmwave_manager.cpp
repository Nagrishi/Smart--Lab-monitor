
#include <Arduino.h>

#include "config.h"
#include "mmwave_manager.h"

static unsigned long sensorStartTime = 0;

const unsigned long MMWAVE_WARMUP_TIME = 5000;

void mmwaveManagerInit() {

    pinMode(MMWAVE_PIN, INPUT);

    sensorStartTime = millis();

    Serial.println("mmWave initialized");
    Serial.println("Waiting for sensor stabilization...");
}

void mmwaveManagerUpdate(SensorData &data) {

    int rawValue = digitalRead(MMWAVE_PIN);

    data.presence =
        (rawValue == MMWAVE_ACTIVE_STATE);

    data.presenceValid =
        (millis() - sensorStartTime >= MMWAVE_WARMUP_TIME);

    // Debug output for Serial Monitor
   /* Serial.printf(
        "MMWAVE RAW: %d | PRESENCE: %s | VALID: %s\n",
        rawValue,
        data.presence ? "YES" : "NO",
        data.presenceValid ? "YES" : "NO"
    );*/
}

