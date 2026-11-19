
#include <Arduino.h>

#include "config.h"
#include "buzzer_manager.h"

// Store current alarm states
static bool alarmActiveState = false;
static bool temperatureWarningState = false;

// Initialize buzzer
void buzzerManagerInit() {

    pinMode(BUZZER_PIN, OUTPUT);

    digitalWrite(BUZZER_PIN, BUZZER_OFF);

    alarmActiveState = false;
    temperatureWarningState = false;

    Serial.println("Buzzer initialized");
}

// Evaluate alarm conditions
void buzzerManagerUpdate(const SensorData &data) {

    // Condition 1: Temperature >= 45 C
    bool highTemperature =
        data.dhtValid &&
        (data.temperature >= TEMP_ALARM_LIMIT);

    // Condition 2: No person detected
    bool noPersonDetected =
        data.presenceValid &&
        !data.presence;

    // Either condition activates the alarm
    bool alarmActive =
        highTemperature || noPersonDetected;

    // Save alarm states for other managers
    temperatureWarningState = highTemperature;
    alarmActiveState = alarmActive;

    // Control physical buzzer
    digitalWrite(
        BUZZER_PIN,
        alarmActive ? BUZZER_ON : BUZZER_OFF
    );
}

// Return buzzer ON/OFF state
bool buzzerManagerIsActive() {

    return alarmActiveState;
}

// Return temperature warning state
bool buzzerManagerTemperatureWarning() {

    return temperatureWarningState;
}
