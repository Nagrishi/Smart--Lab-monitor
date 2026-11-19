#pragma once

#include "sensor_data.h"

// Initialize cloud communication
void cloudManagerInit();

// Upload sensor data when the interval is reached
void cloudManagerUpdate(const SensorData &data);