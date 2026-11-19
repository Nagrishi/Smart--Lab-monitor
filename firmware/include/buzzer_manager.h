
#pragma once

#include "sensor_data.h"

// Initialize buzzer
void buzzerManagerInit();

// Evaluate alarm conditions and control buzzer
void buzzerManagerUpdate(const SensorData &data);

// Get current buzzer state
bool buzzerManagerIsActive();

// Get temperature warning state
bool buzzerManagerTemperatureWarning();
